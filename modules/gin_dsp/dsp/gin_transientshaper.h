/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from MGA TranShaper.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn. Used under the terms
 of its permission notice, which allows use, modification and distribution.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Transient shaper - independent control over attack and sustain.

    Unlike a compressor, there is no threshold. The gain comes from the ratio
    between three envelope followers running on the same detector signal: a
    fast one (100 Hz), a baseline (10 Hz) and a slow one (1 Hz). When the fast
    envelope runs ahead of the baseline the signal is in its attack; when the
    slow one sits above the baseline it is in its tail. Because both are
    ratios, the effect is level independent - it works the same on a quiet
    drum loop as on a hot one, with nothing to re-tune as the source changes.

    Positive attack sharpens transients, negative softens them. Positive
    sustain lengthens the tail, negative shortens it - a quick way to dry out
    an over-reverberant room mic.

    Key Features:
    - Attack and sustain shaped independently, with no threshold to set
    - Mid/side weighting for both detection and processing, so transients can
      be shaped on the sides only
    - Optional band limiting - the untouched part of the spectrum is passed
      through dry, so only the kick region gets shaped
    - Gain smoothing to trade snap against artefacts

    Usage:
    @code
    TransientShaper shaper;
    shaper.setSampleRate (44100.0);
    shaper.setAttack (2.0f);      // sharpen the attack
    shaper.setSustain (-1.0f);    // and dry out the tail

    shaper.process (buffer);      // stereo, in place
    @endcode

    Note: stereo only - process() expects a two channel buffer.

    @see Dynamics, Maximizer
*/
class TransientShaper
{
public:
    TransientShaper() = default;
    ~TransientShaper() = default;

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        recalc();
        reset();
    }

    /** Attack shaping, -4 to +4. Positive sharpens, negative softens, 0 is neutral. */
    void setAttack (float a)                { attack = a; }

    /** Sustain shaping, -8 to +8. Positive lengthens the tail, negative shortens it. */
    void setSustain (float s)               { sustain = s; }

    /** What the detector listens to: 0 is mid only, 0.5 both, 1 side only. */
    void setDetectorBalance (float b)
    {
        detSide = std::min (b, 0.5f);
        detMid  = std::min (1.0f - b, 0.5f);
    }

    /** Where the gain is applied: 0 is mid only, 0.5 both, 1 side only. */
    void setProcessBalance (float b)
    {
        procSide = std::min (b, 0.5f);
        procMid  = std::min (1.0f - b, 0.5f);
    }

    /** Restricts shaping to a band. Everything outside it is passed through
        dry and unshaped, so a 40-200 Hz band shapes the kick and leaves the
        rest of the mix alone.

        @param lowCutHz     bottom of the shaped band, 0 for no low cut
        @param highCutHz    top of the shaped band, 0 or >= nyquist for none
    */
    void setBand (float lowCutHz, float highCutHz)
    {
        lowCut  = lowCutHz;
        highCut = highCutHz;
        recalc();
    }

    /** Gain smoothing, 0 (snappy, 10 kHz) to 1 (smooth, 0.1 Hz). */
    void setSmoothing (float s)
    {
        smoothing = juce::jlimit (0.0f, 1.0f, s);
        recalc();
    }

    void setOutputGain (float db)           { outputGain = juce::Decibels::decibelsToGain (db); }

    void reset()
    {
        envState = envAttState = envRelState = gainState = 0.0f;
        lowCutFilter[0].reset();  lowCutFilter[1].reset();
        highCutFilter[0].reset(); highCutFilter[1].reset();
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        jassert (buffer.getNumChannels() >= 2);
        if (buffer.getNumChannels() < 2)
            return;

        juce::ScopedNoDenormals noDenormals;

        auto* l = buffer.getWritePointer (0);
        auto* r = buffer.getWritePointer (1);

        const int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; i++)
        {
            float inL = l[i], inR = r[i];

            // Band limiting: what the filters take out is kept aside and added
            // back untouched, so the shaped band is the only thing that moves.
            float dryL = 0.0f, dryR = 0.0f;

            if (highCutActive)
            {
                auto wetL = highCutFilter[0].process (inL);
                auto wetR = highCutFilter[1].process (inR);
                dryL += inL - wetL;
                dryR += inR - wetR;
                inL = wetL;
                inR = wetR;
            }

            if (lowCutActive)
            {
                auto wetL = lowCutFilter[0].process (inL);
                auto wetR = lowCutFilter[1].process (inR);
                dryL += inL - wetL;
                dryR += inR - wetR;
                inL = wetL;
                inR = wetR;
            }

            auto mid  = (inL + inR) * 0.5f;
            auto side = (inL - inR) * 0.5f;

            auto detector = std::max (std::abs (mid * detMid), std::abs (side * detSide)) + 1.0e-5f;

            envState    = envA    * detector + envB    * envState;
            envAttState = envAttA * detector + envAttB * envAttState;
            envRelState = envRelA * detector + envRelB * envRelState;

            auto env    = std::sqrt (envState);
            auto envAtt = std::sqrt (envAttState);
            auto envRel = std::sqrt (envRelState);

            // Attack is how far the fast envelope has run ahead of the
            // baseline, sustain how far the slow one lags behind it. Both are
            // ratios, which is what makes this level independent.
            auto gain = std::exp (std::log (std::max (envAtt / env, 1.0f)) * attack
                                + std::log (std::max (envRel / env, 1.0f)) * sustain);

            gainState = gainA * gain + gainB * gainState;

            // Smoothing only ever holds the gain back, never lets it overshoot
            gain = std::min (std::sqrt (std::max (gainState, 0.0f)), gain);
            gain = std::min (gain, 64.0f);

            mid  *= std::pow (gain, procMid);
            side *= std::pow (gain, procSide);

            l[i] = (dryL + (mid + side)) * outputGain;
            r[i] = (dryR + (mid - side)) * outputGain;
        }
    }

private:
    //==============================================================================
    static void onePole (double freq, double sr, float& a, float& b)
    {
        b = float (std::exp (-2.0 * juce::MathConstants<double>::pi * freq / sr));
        a = 1.0f - b;
    }

    void recalc()
    {
        onePole (10.0,  sampleRate, envA,    envB);      // baseline
        onePole (100.0, sampleRate, envAttA, envAttB);   // fast, leads on transients
        onePole (1.0,   sampleRate, envRelA, envRelB);   // slow, lags into the tail

        auto smoothHz = std::pow (9999.9f, (1.0f - smoothing) * (1.0f - smoothing)) + 0.1f;
        onePole (smoothHz, sampleRate, gainA, gainB);

        auto nyquist = float (sampleRate / 2.1);

        lowCutActive = lowCut > 0.0f && lowCut < nyquist;
        if (lowCutActive)
            for (auto& f : lowCutFilter)
                f.setHighpass (lowCut, butterworthQ, sampleRate);

        highCutActive = highCut > 0.0f && highCut < nyquist;
        if (highCutActive)
            for (auto& f : highCutFilter)
                f.setLowpass (highCut, butterworthQ, sampleRate);
    }

    static constexpr double butterworthQ = 0.70710678118654752440;

    double sampleRate = 44100.0;

    float attack = 0.0f, sustain = 0.0f;
    float detMid = 0.5f, detSide = 0.5f;
    float procMid = 0.5f, procSide = 0.5f;
    float smoothing = 0.5f;
    float outputGain = 1.0f;
    float lowCut = 0.0f, highCut = 0.0f;
    bool lowCutActive = false, highCutActive = false;

    float envA = 0.0f, envB = 0.0f, envState = 0.0f;
    float envAttA = 0.0f, envAttB = 0.0f, envAttState = 0.0f;
    float envRelA = 0.0f, envRelB = 0.0f, envRelState = 0.0f;
    float gainA = 0.0f, gainB = 0.0f, gainState = 0.0f;

    Biquad lowCutFilter[2], highCutFilter[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransientShaper)
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from 50HzKicker.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn. Used under the terms
 of its permission notice, which allows use, modification and distribution.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Kick reinforcement - synthesises low end triggered by transients.

    A bandpass picks out the region a kick lives in and drives a transient
    detector. Whenever the fast envelope runs ahead of the tracked one - which
    is to say, whenever something hits - a sine is triggered at the chosen
    frequency and faded in proportion to how big the hit was, then mixed in
    under the original.

    This is not EQ. Boosting a kick that has no energy at 50 Hz only raises
    what is already there, whereas this puts a note where there wasn't one,
    which is how you fix a thin kick without touching the rest of the mix. The
    oscillator's phase is reset on every trigger, so successive hits always
    add in phase with themselves and never cancel.

    Feed it a side chain to key from something other than the signal being
    processed - a kick track keying a synthesised sub under a whole mix.

    Key Features:
    - Tunable detector band and oscillator frequency
    - Phase reset per hit, so hits reinforce rather than cancel
    - Independent wet and dry levels
    - Optional side chain key

    Usage:
    @code
    KickSynth kick;
    kick.setSampleRate (44100.0);
    kick.setDetectorFrequency (80.0f);   // listen around 80 Hz
    kick.setFrequency (50.0f);           // synthesise 50 Hz
    kick.setLength (0.3f);
    kick.setWet (0.25f);

    kick.process (buffer);
    @endcode

    Note: stereo only - the synthesised signal is mono and added to both.

    @see TransientShaper, Dynamics
*/
class KickSynth
{
public:
    KickSynth() = default;
    ~KickSynth() = default;

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        recalc();
        reset();
    }

    /** Centre of the band the transient detector listens to. */
    void setDetectorFrequency (float hz)
    {
        detectorFrequency = hz;
        recalc();
    }

    /** Frequency of the synthesised sine. */
    void setFrequency (float hz)
    {
        frequency = hz;
        recalc();
    }

    /** How long each hit rings for, 0 to 1. */
    void setLength (float l)
    {
        length = juce::jlimit (0.0f, 1.0f, l);
        recalc();
    }

    void setWet (float w)                   { wet = w; }
    void setDry (float d)                   { dry = d; }

    void reset()
    {
        detectorFilter.reset();
        outputFilter.reset();
        envelopeState = 0.0f;
        envelope = 0.03f;
        phase = 0.0f;
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        processInternal (buffer, nullptr);
    }

    /** Keys off `sidechain` instead of the signal being processed. */
    void process (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer& sidechain)
    {
        jassert (sidechain.getNumSamples() >= buffer.getNumSamples());

        if (sidechain.getNumChannels() <= 0 || sidechain.getNumSamples() < buffer.getNumSamples())
            processInternal (buffer, nullptr);
        else
            processInternal (buffer, &sidechain);
    }

private:
    void processInternal (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer* sidechain)
    {
        jassert (buffer.getNumChannels() >= 2);
        if (buffer.getNumChannels() < 2)
            return;

        juce::ScopedNoDenormals noDenormals;

        auto* l = buffer.getWritePointer (0);
        auto* r = buffer.getWritePointer (1);

        const float* keyL = sidechain != nullptr ? sidechain->getReadPointer (0) : nullptr;
        const float* keyR = sidechain != nullptr ? sidechain->getReadPointer (std::min (1, sidechain->getNumChannels() - 1))
                                                 : nullptr;

        const int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; i++)
        {
            auto key = keyL != nullptr ? keyL[i] + keyR[i] : l[i] + r[i];

            auto detected = detectorFilter.process (key);

            envelopeState = envA * std::abs (detected) + envB * envelopeState;
            auto fast = std::max (std::sqrt (envelopeState), 0.03f);

            // The tracked envelope chases the fast one, but only so quickly.
            // How far the fast one gets ahead is how much of a transient this
            // is, and that is what sets the level of the hit.
            if (envelope < fast)
                envelope = std::min (envelope / attackFactor, fast);
            else
                envelope = std::max (envelope * releaseFactor, fast);

            auto gain = std::min (std::max (fast / envelope, 1.0f) - 1.0f, 1.0f);

            if (gain == 0.0f)
                phase = 0.0f;

            auto synth = outputFilter.process (std::sin (phase) * gain * wet);

            phase += phaseIncrement;
            if (phase > juce::MathConstants<float>::twoPi)
                phase -= juce::MathConstants<float>::twoPi;

            l[i] = synth + l[i] * dry;
            r[i] = synth + r[i] * dry;
        }
    }

    //==============================================================================
    void recalc()
    {
        detectorFilter.setBandpass (detectorFrequency, bandpassQ, sampleRate);
        outputFilter.setBandpass (frequency, bandpassQ, sampleRate);

        phaseIncrement = float (2.0 * juce::MathConstants<double>::pi * frequency / sampleRate);

        envB = float (std::exp (-2.0 * juce::MathConstants<double>::pi * 10.0 / sampleRate));
        envA = 1.0f - envB;

        auto attackSeconds  = 0.050f + 0.450f * length;
        auto releaseSeconds = 0.025f + 0.175f * length;

        attackFactor  = juce::Decibels::decibelsToGain (float (-50.0 / (attackSeconds  * sampleRate)));
        releaseFactor = juce::Decibels::decibelsToGain (float (-50.0 / (releaseSeconds * sampleRate)));
    }

    double sampleRate = 44100.0;

    float detectorFrequency = 80.0f;
    float frequency = 50.0f;
    float length = 0.5f;
    float wet = 0.25f, dry = 1.0f;

    static constexpr double bandpassQ = 0.25;   // deliberately wide

    Biquad detectorFilter, outputFilter;

    float envA = 0.0f, envB = 0.0f, envelopeState = 0.0f, envelope = 0.03f;
    float attackFactor = 1.0f, releaseFactor = 1.0f;
    float phase = 0.0f, phaseIncrement = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KickSynth)
};

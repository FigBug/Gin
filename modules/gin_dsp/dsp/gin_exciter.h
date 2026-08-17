/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from MGA Exciter.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn. Used under the terms
 of its permission notice, which allows use, modification and distribution.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Aural exciter - adds harmonics generated from the top of the signal.

    The top end is split off, driven into a soft waveshaper to generate
    harmonics, highpassed again to throw away everything the shaper folded
    back down into the midrange, and then mixed in alongside the untouched
    signal. Because the dry path is never filtered, the original is still
    there in full - the effect only ever adds.

    This buys presence without the phase shift of an EQ boost, and because the
    harmonics are generated from the material itself they track it rather than
    lifting the noise floor along with everything else.

    Key Features:
    - Adjustable crossover, drive and harmonic content
    - Parallel by design - dry signal passes through untouched
    - Second highpass keeps the generated harmonics out of the midrange

    Usage:
    @code
    Exciter exciter;
    exciter.setSampleRate (44100.0);
    exciter.setFrequency (7000.0f);   // excite above 7 kHz
    exciter.setDrive (3.0f);
    exciter.setHarmonics (0.4f);
    exciter.setMix (0.5f);

    exciter.process (buffer);
    @endcode

    @see Distortion, SoftClipper
*/
class Exciter
{
public:
    Exciter() = default;
    ~Exciter() = default;

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        recalc();
        reset();
    }

    /** Crossover frequency - only content above this generates harmonics. */
    void setFrequency (float hz)
    {
        frequency = hz;
        recalc();
    }

    /** Drive into the shaper, 1 to 8. */
    void setDrive (float d)                 { drive = std::max (1.0f, d); }

    /** How much harmonic content the shaper generates, 0 to 1. */
    void setHarmonics (float h)
    {
        auto f = juce::jlimit (0.0f, 1.0f, h) * 0.99f;
        harmonics = 2.0f * f / std::max (1.0e-6f, 1.0f - std::sqrt (f));
    }

    /** How much of the generated signal is added, 0 to 2. The dry signal is
        always passed through at unity, so this only ever adds. */
    void setMix (float m)                   { mix = m; }

    void reset()
    {
        for (auto& c : chans)
            c = {};
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        juce::ScopedNoDenormals noDenormals;

        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        if (int (chans.size()) < numChannels)
            chans.resize (size_t (numChannels));

        for (int ch = 0; ch < numChannels; ch++)
        {
            auto& c = chans[size_t (ch)];
            auto* d = buffer.getWritePointer (ch);

            for (int i = 0; i < numSamples; i++)
            {
                auto dry = d[i];

                // Highpass by subtracting a one pole lowpass
                c.lp1 = a * dry + b * c.lp1;
                auto s = dry - c.lp1;

                s = juce::jlimit (-1.0f, 1.0f, s * drive);

                // Soft saturation - the harmonics come from here
                s = (1.0f + harmonics) * s / (1.0f + harmonics * std::abs (s));

                // And again, so what the shaper folded down into the midrange
                // does not muddy the signal it was supposed to brighten
                c.lp2 = a * s + b * c.lp2;
                s = s - c.lp2;

                d[i] = dry + s * mix;
            }
        }
    }

private:
    void recalc()
    {
        b = float (std::exp (-2.0 * juce::MathConstants<double>::pi * frequency / sampleRate));
        a = 1.0f - b;
    }

    struct Channel
    {
        float lp1 = 0.0f, lp2 = 0.0f;
    };

    double sampleRate = 44100.0;
    float frequency = 7000.0f;
    float drive = 1.0f;
    float harmonics = 0.0f;
    float mix = 0.5f;
    float a = 0.0f, b = 0.0f;

    std::vector<Channel> chans { 2 };
};

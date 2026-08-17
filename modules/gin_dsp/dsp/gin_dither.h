/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from MGA Dither.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn. Used under the terms
 of its permission notice, which allows use, modification and distribution.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Dither and noise shaping for bit depth reduction.

    Truncating to a lower bit depth correlates the error with the signal,
    which is what makes quiet fades sound grainy and reverb tails turn to
    gravel rather than simply getting quieter. Adding a small amount of noise
    before quantising decorrelates it: the error becomes hiss, which is far
    less objectionable than distortion that tracks the music.

    Noise shaping goes further and feeds the quantisation error back through a
    second order filter, pushing the noise out of the midrange where the ear
    is most sensitive.

    Key Features:
    - Rectangular, triangular and near-gaussian noise
    - Optional highpass shaped noise
    - Second order error feedback noise shaping
    - Any bit depth

    Usage:
    @code
    Dither dither;
    dither.setBitDepth (16);
    dither.setNoiseType (Dither::triangular);   // the usual choice
    dither.setNoiseShaping (0.5f);

    dither.process (buffer);
    @endcode

    Note: TPDF (triangular) at 2 LSB is the standard, safe default - it fully
    decorrelates the error at the cost of a little more noise than RPDF.

    @see BitCrusher
*/
class Dither
{
public:
    Dither() = default;
    ~Dither() = default;

    /**
        Probability distribution of the dither noise.

        - none: no dither, straight truncation - included for comparison
        - rectangular: RPDF, one random value. Quietest, but the error is not
          fully decorrelated and the noise floor still moves with the signal
        - triangular: TPDF, sum of two. The standard choice
        - gaussian: sum of ten, approaching a normal distribution
    */
    enum NoiseType
    {
        none = 0,
        rectangular,
        triangular,
        gaussian
    };

    /**
        Spectrum of the dither noise itself.

        - flat: white
        - highpass: the difference of successive values, tilting the noise up
          and away from where the ear is most sensitive
    */
    enum Shaping
    {
        flat = 0,
        highpass
    };

    /** Target bit depth. 16 for CD, 8 for something lo-fi on purpose. */
    void setBitDepth (int bits)
    {
        bitDepth = juce::jlimit (1, 32, bits);
        scale = std::pow (2.0f, float (bitDepth));
    }

    void setNoiseType (NoiseType t)         { noiseType = t; }
    void setShaping (Shaping s)             { shaping = s; }

    /** Dither depth in LSBs. 2 is the usual amount for TPDF. */
    void setAmount (float lsbs)             { amount = std::max (0.0f, lsbs); }

    /** Error feedback noise shaping, 0 to 1. */
    void setNoiseShaping (float ns)         { noiseShaping = juce::jlimit (0.0f, 1.0f, ns) * 0.5f; }

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
                c.lastRandom = c.random;
                c.random = nextNoise();

                auto n = shaping == highpass ? c.random - c.lastRandom : c.random;

                // Error feedback: push the last two quantisation errors back
                // in, filtered, so the noise they become is not flat
                auto x = d[i] + noiseShaping * (c.e1 + c.e1 - c.e2);

                auto noise = n * amount + 0.5f;   // 0.5 makes the floor round to nearest
                auto out = juce::jlimit (-1.0f, 1.0f, std::floor (x * scale + noise) / scale);

                c.e2 = c.e1;
                c.e1 = x - out;

                d[i] = out;
            }
        }
    }

private:
    float nextNoise()
    {
        switch (noiseType)
        {
            case none:
                return 0.0f;

            case rectangular:
                return random.nextFloat() - 0.5f;

            case triangular:
                return (random.nextFloat() + random.nextFloat()) * 0.5f - 0.5f;

            case gaussian:
            {
                float sum = 0.0f;
                for (int i = 0; i < 10; i++)
                    sum += random.nextFloat();
                return sum / 10.0f - 0.5f;
            }
        }

        return 0.0f;
    }

    struct Channel
    {
        float e1 = 0.0f, e2 = 0.0f;
        float random = 0.0f, lastRandom = 0.0f;
    };

    juce::Random random;
    std::vector<Channel> chans { 2 };

    int bitDepth = 16;
    float scale = 65536.0f;
    float amount = 2.0f;
    float noiseShaping = 0.0f;
    NoiseType noiseType = triangular;
    Shaping shaping = flat;
};

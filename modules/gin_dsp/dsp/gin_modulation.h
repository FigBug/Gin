/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Audio modulation, for a chorus or phaser etc
*/
class Modulation
{
public:
    Modulation (float maxDelay_ = 30.0f / 1000.0f)
        : maxDelay (maxDelay_)
    {
    }

    void setSampleRate (double sr)
    {
        sampleRate = sr;

        delayLine.setSize (2, (maxDelay + depth) + 0.1, sampleRate);
        delayLine.clear();
        phase = 0.0f;
    }

    void reset()
    {
        phase = 0;
        delayLine.clear();
    }

    void setParams (float delay_, float rate_, float depth_, float width_, float mix_)
    {
        delay = delay_;
        rate  = rate_;
        depth = depth_;
        width = width_;
        mix   = mix_;
    }
    
    void process (juce::AudioSampleBuffer& buffer)
    {
        WetDryMix wetDry (mix);

        float delta = float (rate / sampleRate);

        int numSamples = buffer.getNumSamples ();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        for (int i = 0; i < numSamples; ++i)
        {
            float totalDelayL = delay + depth / 2.0f + depth * std::sin (juce::MathConstants<float>::pi * 2 * (phase + width / 2.0f)) / 2.0f;
            float totalDelayR = delay + depth / 2.0f + depth * std::sin (juce::MathConstants<float>::pi * 2 * (phase - width / 2.0f)) / 2.0f;

            delayLine.write (0, l[i]);
            delayLine.write (1, r[i]);
            delayLine.writeFinished();

            l[i] = delayLine.readLagrange (0, totalDelayL) * wetDry.wetGain + l[i] * wetDry.dryGain;
            r[i] = delayLine.readLagrange (1, totalDelayR) * wetDry.wetGain + r[i] * wetDry.dryGain;

            phase += delta;
            if (phase >= 1.0f)
                phase = 0.0f;
        }
    }

private:
    const float maxDelay;

    DelayLine delayLine {2};

    double sampleRate = 44100;

    float phase = 0.0f;
    float delay = 0.02f, rate = 1.0f, depth = 0.03f, width = 0.5f, mix = 0.0f;
};

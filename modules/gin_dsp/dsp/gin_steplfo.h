/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** LFO with editable number of steps
*/
class StepLFO
{
public:
    //==============================================================================
    StepLFO() = default;

    //==============================================================================
    void setSampleRate (double sr)      { sampleRate = sr;      }
    void setFreq (float f)              { frequency = f;        }
    void setNumPoints (int n)           { points.resize (n);    }
    void setPoint (int n, float v)      { points.set (n, v);    }

    void reset()
    {
        output     = 0.0f;
        phase      = 0.0f;
    }

    void noteOn (float phase_ = -1.0f)
    {
        output  = 0.0f;
        phase   = phase_ < 0.0f ? 0.0f : int (phase_ * points.size()) / float (points.size());
    }

    float process (int numSamples)
    {
        jassert (sampleRate > 0);

        float step = 0.0f;
        if (frequency > 0.0001f)
            step = float (frequency / sampleRate) / float (points.size());

        for (int i = 0; i < numSamples; i++)
        {
            phase += step;
            while (phase >= 1.0f)
                phase -= 1.0f;
        }

        return updateCurrentValue();
    }

    float getOutput()
    {
        return juce::jlimit (-1.0f, 1.0f, output);
    }
    
    float getCurrentPhase()
    {
        return phase;
    }

private:
    float updateCurrentValue()
    {
        output = points[int (phase * float (points.size()))];

        return juce::jlimit (-1.0f, 1.0f, (output));
    }

    double sampleRate = 0.0;
    float frequency = 0.0f, phase = 0.0f, output = 0.0f;

    juce::Array<float> points;
};

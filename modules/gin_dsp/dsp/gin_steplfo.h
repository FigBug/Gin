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
    StepLFO()
    {
    }
    
    //==============================================================================
    void setSampleRate (double sr)      { sampleRate = sr;      }
    void setFreq (float f)              { frequency = f;        }
    void setNumPoints (int n)           { points.resize (n);    }
    void setPoint (int n, float v)      { points.set (n, v);    }
    
    void reset()
    {
        output     = 0.0f;
        phase      = 0.0f;
        curPhase   = 0.0f;
    }
    
    void noteOn()
    {
        output     = 0.0f;
        phase      = 0.0f;
        curPhase   = 0.0f;
    }

    float process (int numSamples)
    {
        jassert (sampleRate > 0);

        float step = 0.0f;
        if (frequency > 0.0001f)
            step = float (frequency / sampleRate);

        for (int i = 0; i < numSamples; i++)
        {
            float maxPhase = points.size();

            phase += step;
            while (phase >= maxPhase)
                phase -= maxPhase;

            float newCurPhase = std::fmod (phase, maxPhase);
            if (newCurPhase < 0) newCurPhase += maxPhase;

            curPhase = newCurPhase;
        }
        
        return updateCurrentValue();
    }
    
    float getOutput()
    {
        return jlimit (-1.0f, 1.0f, output);
    }

private:
    float updateCurrentValue()
    {
        output = points[int (curPhase)];

        return jlimit (-1.0f, 1.0f, (output));
    }

    double sampleRate = 0.0;
    float frequency = 0.0f, phase = 0.0f, curPhase = 0.0f, output = 0.0f;

    Array<float> points;
};

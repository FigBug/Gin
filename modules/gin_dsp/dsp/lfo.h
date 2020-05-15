/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================

class LFO
{
public:
    //==============================================================================
    enum class WaveShape : int
    {
        none,
        sine,
        triangle,
        sawUp,
        sawDown,
        square,
        squarePos,
        sampleAndHold,
        noise,
        stepUp4,
        stepup8,
        stepDown4,
        stepDown8,
    };

    //==============================================================================
    struct Parameters
    {
        WaveShape waveShape = WaveShape::sine;
        float frequency = 0, phase = 0, offset = 0, depth = 0, delay = 0, fade = 0;
    };

    //==============================================================================
    LFO()
    {
        juce::Random rnd { 1 };

        for (int i = 0; i < 1000; i++)
            randomPoints.add (rnd.nextFloat() * 2 - 1);
    }

    //==============================================================================
    void setSampleRate (double sr)      { sampleRate = sr;  }
    void setParameters (Parameters p)   { parameters = p;   }
    void reset()
    {
        output     = 0.0f;
        phase      = 0.0f;
        curPhase   = 0.0f;
        curFade    = 1.0f;
        fadeDelta  = 0.0f;
        delaySteps = 0;
    }
    
    void noteOn()
    {
        if (parameters.fade <= 0)
            curFade = 1.0f;
        else
            curFade = 0.0f;
        
        fadeDelta  = float (1.0f / (sampleRate * parameters.fade));
        delaySteps = roundToInt (sampleRate * parameters.delay);
    }

    float process (int numSamples)
    {
        jassert (sampleRate > 0);

        float step = 0.0f;
        if (parameters.frequency > 0.0001f)
            step = float (parameters.frequency / sampleRate);

        for (int i = 0; i < numSamples; i++)
        {
            if (delaySteps > 0)
            {
                delaySteps--;
            }
            else
            {
                curFade = jlimit (0.0f, 1.0f, curFade + fadeDelta);

                float maxPhase = 1.0f;
                if (parameters.waveShape == WaveShape::sampleAndHold || parameters.waveShape == WaveShape::noise)
                    maxPhase = 1000.0f;

                phase += step;
                while (phase >= maxPhase)
                    phase -= maxPhase;

                float newCurPhase = std::fmod (phase + parameters.phase, maxPhase);
                if (newCurPhase < 0) newCurPhase += maxPhase;

                curPhase = newCurPhase;
            }
        }
        
        return updateCurrentValue();
    }
    
    float getOutput()
    {
        return jlimit (-1.0f, 1.0f, (curFade * output * parameters.depth + parameters.offset));
    }

private:
    float lerp (float t, float a, float b)  { return a + t * (b - a); }

    float updateCurrentValue()
    {
        if (delaySteps == 0)
        {
            switch (parameters.waveShape)
            {
                case WaveShape::none:
                    output = 0;
                    break;
                case WaveShape::sine:
                    output = std::sin (curPhase * MathConstants<float>::pi * 2);
                    break;
                case WaveShape::triangle:
                    output = (curPhase < 0.5f) ? (4.0f * curPhase - 1.0f) : (-4.0f * curPhase + 3.0f);
                    break;
                case WaveShape::sawUp:
                    output = curPhase * 2.0f - 1.0f;
                    break;
                case WaveShape::sawDown:
                    output = (1.0f - curPhase) * 2.0f - 1.0f;
                    break;
                case WaveShape::square:
                    output = (curPhase < 0.5f) ? 1.0f : -1.0f;
                    break;
                case WaveShape::squarePos:
                    output = (curPhase < 0.5f) ? 1.0f : 0.0f;
                    break;
                case WaveShape::sampleAndHold:
                    output = randomPoints[int (curPhase)];
                    break;
                case WaveShape::noise:
                {
                    int p = int (curPhase);
                    float t = curPhase - p;
                    output = lerp (t, randomPoints[p], randomPoints[p + 1]);
                    break;
                }
                case WaveShape::stepUp4:
                    output = int (curPhase * 4) / 4.0f * 8.0f/3.0f - 1;
                    break;
                case WaveShape::stepup8:
                    output = int (curPhase * 8) / 8.0f * 16.0f/7.0f - 1;
                    break;
                case WaveShape::stepDown4:
                    output = -(int (curPhase * 4) / 4.0f * 8.0f/3.0f - 1);
                    break;
                case WaveShape::stepDown8:
                    output = -(int (curPhase * 8) / 8.0f * 16.0f/7.0f - 1);
                    break;
            }
        }

        return jlimit (-1.0f, 1.0f, (curFade * output * parameters.depth + parameters.offset));
    }

    Parameters parameters;

    double sampleRate = 0;
    float phase = 0, curPhase = 0, output = 0, fadeDelta = 0, curFade = 0;
    int delaySteps = 0;

    Array<float> randomPoints;
};

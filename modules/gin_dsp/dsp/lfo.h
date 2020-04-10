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
    void setSampleRate (double sr)      { sampleRate = sr;  }
    void setParameters (Parameters p)   { parameters = p;   }
    void reset()
    {
        output     = 0.0f;
        phase      = 0.0f;
        curPhase   = 0.0f;
        nextStep   = false;
        curFade    = 1.0f;
        fadeDelta  = 0.0f;
        delaySteps = 0.0f;
    }
    
    void noteOn()
    {
        if (parameters.fade <= 0)
            curFade = 1.0f;
        
        fadeDelta  = float (1.0f / (sampleRate * parameters.fade));
        delaySteps = roundToInt (sampleRate * parameters.delay);
    }

    void process (int numSamples)
    {
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
                
                phase += step;
                if (phase >= 1.0f)
                    phase -= 1.0f;

                float newCurPhase = std::fmod (phase + parameters.phase, 1.0f);
                jassert (newCurPhase >= 0 && newCurPhase <= 1.0);

                if (newCurPhase < curPhase)
                    nextStep = true;
                
                curPhase = newCurPhase;
            }
        }
    }

    float getCurrentValue()
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
                    if (compareAndReset (nextStep))
                        output = rnd.nextFloat() * 2.0f - 1.0f;
                    break;
                case WaveShape::noise:
                    break;
                case WaveShape::stepUp4:
                    if (compareAndReset (nextStep))
                        output = int (curPhase * 4) / 4 * 2 - 1;
                    break;
                case WaveShape::stepup8:
                    if (compareAndReset (nextStep))
                        output = int (curPhase * 8) / 8 * 2 - 1;
                    break;
                case WaveShape::stepDown4:
                    if (compareAndReset (nextStep))
                        output = -(int (curPhase * 4) / 4 * 2 - 1);
                    break;
                case WaveShape::stepDown8:
                    if (compareAndReset (nextStep))
                        output = -(int (curPhase * 8) / 8 * 2 - 1);
                    break;
            }
        }

        return jlimit (-1.0f, 1.0f, (output * parameters.depth + parameters.offset));
    }

private:
    Parameters parameters;

    double sampleRate = 0;
    float phase = 0, curPhase = 0, output = 0, fadeDelta = 0, curFade = 0;
    bool nextStep = false;
    int delaySteps = 0;

    juce::Random rnd { 1 };
};

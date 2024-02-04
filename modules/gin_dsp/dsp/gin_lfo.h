/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** LFO with many shapes
*/
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
        stepUp3,
        stepUp4,
        stepup8,
        stepDown3,
        stepDown4,
        stepDown8,
        pyramid3,
        pyramid5,
        pyramid9,
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

    void noteOn (float phase_ = -1.0f)
    {
        if (parameters.fade <= 0)
            curFade = 1.0f;
        else
            curFade = 0.0f;

        curPhase   = 0.0f;
        phase      = phase_ < 0.0f ? 0.0f : phase_;
        fadeDelta  = float (1.0f / (sampleRate * parameters.fade));
        delaySteps = juce::roundToInt (sampleRate * parameters.delay);

        float maxPhase = 1.0f;
        float newCurPhase = std::fmod (phase + parameters.phase, maxPhase);
        if (newCurPhase < 0)
            newCurPhase += maxPhase;
        curPhase = newCurPhase;

        updateCurrentValue();
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
                curFade = juce::jlimit (0.0f, 1.0f, curFade + fadeDelta);

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
        return juce::jlimit (-1.0f, 1.0f, (curFade * output * parameters.depth + parameters.offset));
    }

    float getOutputUnclamped()
    {
        return curFade * output * parameters.depth + parameters.offset;
    }

    float getCurrentPhase()
    {
        return phase;
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
                    output = std::sin (curPhase * juce::MathConstants<float>::pi * 2);
                    break;
                case WaveShape::triangle:
                {
                    float p = std::fmod (curPhase + 0.25f, 1.0f);
                    output = (p < 0.5f) ? (4.0f * p - 1.0f) : (-4.0f * p + 3.0f);
                    break;
                }
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
                    float t = curPhase - float ( p );
                    output = lerp (t, randomPoints[p], randomPoints[p + 1]);
                    break;
                }
                case WaveShape::stepUp3:
                    output = float ( int (curPhase * 3) ) / 3.0f * 6.0f/2.0f - 1;
                    break;
                case WaveShape::stepUp4:
                    output = float ( int (curPhase * 4) ) / 4.0f * 8.0f/3.0f - 1;
                    break;
                case WaveShape::stepup8:
                    output = float ( int (curPhase * 8) ) / 8.0f * 16.0f/7.0f - 1;
                    break;
                case WaveShape::stepDown3:
                    output = -( float ( int (curPhase * 3) ) / 3.0f * 6.0f/2.0f - 1);
                    break;
                case WaveShape::stepDown4:
                    output = -( float ( int (curPhase * 4) ) / 4.0f * 8.0f/3.0f - 1);
                    break;
                case WaveShape::stepDown8:
                    output = -( float ( int (curPhase * 8) ) / 8.0f * 16.0f/7.0f - 1);
                    break;
                case WaveShape::pyramid3:
                {
                    static float vals[] = { 0.0f, 1.0f, 0.0f, -1.0f };
                    output = vals[int (phase * juce::numElementsInArray (vals))];
                    break;
                }
                case WaveShape::pyramid5:
                {
                    static float vals[] = { 0.0f, 0.5f, 1.0f, 0.5f, 0.0f, -0.5f, -1.0f, -0.5f };
                    output = vals[int (phase * juce::numElementsInArray (vals))];
                    break;
                }
                case WaveShape::pyramid9:
                {
                    static float vals[] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f, 0.75f, 0.5f, 0.25f, 0.0f,
                                            -0.25f, -0.5f, -0.75f, -1.0f, -0.75f, -0.5f, -0.25f };
                    output = vals[int (phase * juce::numElementsInArray (vals))];
                    break;
                }
            }
        }

        return juce::jlimit (-1.0f, 1.0f, (curFade * output * parameters.depth + parameters.offset));
    }

    Parameters parameters;

    double sampleRate = 0.0;
    float phase = 0.0f, curPhase = 0.0f, output = 0.0f, fadeDelta = 0.0f, curFade = 1.0f;
    int delaySteps = 0;

    juce::Array<float> randomPoints;
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Multi-Segment Envelope Generator (MSEG) for complex modulation curves.

    MSEG provides a sophisticated LFO/envelope generator with multiple breakpoints
    and adjustable curves between them. Each segment can have independent curve
    shaping, allowing for complex modulation patterns beyond simple LFO waveforms.

    Key Features:
    - Configurable breakpoints (up to maxPoints, default 200)
    - Curve shaping between points (exponential/logarithmic)
    - Loop mode with definable start/end points
    - Delay and fade-in on note-on
    - Depth and offset controls
    - Phase control for starting position
    - ValueTree serialization for preset saving

    Each Point has:
    - time: Position in the cycle (0.0 to 1.0)
    - value: Output value at this point (-inf to +inf, typically -1 to 1)
    - curve: Curve shape (-inf to +inf, 0=linear, positive=exponential, negative=logarithmic)

    Parameters:
    - frequency: Rate in Hz (for looping mode)
    - phase: Starting phase (0.0 to 1.0)
    - offset: DC offset added to output
    - depth: Output scaling factor
    - delay: Initial delay before envelope starts (seconds)
    - fade: Fade-in time (seconds)
    - loop: Enable looping between start/end points

    Usage:
    @code
    MSEG::Data msegData;
    MSEG mseg(msegData);
    mseg.setSampleRate(44100.0);

    // Define envelope shape
    msegData.points[0] = {0.0f, 0.0f, 0.0f};   // Start at 0
    msegData.points[1] = {0.2f, 1.0f, 2.0f};   // Rise with curve
    msegData.points[2] = {0.8f, 0.5f, -1.0f};  // Decay with curve
    msegData.points[3] = {1.0f, 0.0f, 0.0f};   // Return to 0
    msegData.numPoints = 4;
    msegData.startIndex = 0;
    msegData.endIndex = 3;

    MSEG::Parameters params;
    params.frequency = 2.0f;  // 2 Hz
    params.loop = true;
    params.depth = 1.0f;
    params.offset = 0.0f;
    mseg.setParameters(params);

    mseg.noteOn();
    float modValue = mseg.process(numSamples);
    @endcode

    @see StepLFO, LFO, AnalogADSR
*/
class MSEG
{
public:
    //==============================================================================
    struct Parameters
    {
        float frequency = 0, phase = 0, offset = 0, depth = 0, delay = 0, fade = 0;
        bool loop = true;
    };

    struct Point
    {
        float time  = 0.0f;
        float value = 0.0f;
        float curve = 0.0f;
    };

    struct Data
    {
        juce::Array<Point> points;
        int numPoints = 0;
        int maxPoints = 200;
        int startIndex = 0;
        int endIndex = 2;

        void toValueTree (juce::ValueTree v)
        {
            v.removeAllChildren (nullptr);
            v.setProperty ("startIndex", startIndex, nullptr);
            v.setProperty ("endIndex",   endIndex,   nullptr);

            for (auto i = 0; i < numPoints; i++)
            {
                juce::ValueTree c (juce::Identifier ("PT"));

                c.setProperty ("t", points[i].time,  nullptr);
                c.setProperty ("v", points[i].value, nullptr);
                c.setProperty ("c", points[i].curve, nullptr);

                v.addChild (c, -1, nullptr);
            }
        }

        void fromValueTree (const juce::ValueTree& v)
        {
            if (! v.isValid())
            {
                reset();
                return;
            }

            numPoints   = v.getNumChildren();
            startIndex  = v.getProperty ("startIndex");
            endIndex    = v.getProperty ("endIndex");


            int i = 0;
            for (auto c : v)
            {
                auto& p = points.getReference (i);
                p.time  = c.getProperty ("t");
                p.value = c.getProperty ("v");
                p.curve = c.getProperty ("c");

                i++;
            }
        }

        void reset()
        {
            points.set (0, { 0.0f, 0.0f, 5.5f });
            points.set (1, { 0.5f, 1.0f, 5.5f });
            points.set (2, { 1.0f, 0.0f, 5.5f });

            numPoints   = 3;
            startIndex  = 0;
            endIndex    = 2;
        }
    };

    //==============================================================================
    MSEG (Data& d)
        : data (d)
    {
        data.points.resize (data.maxPoints);

        jassert (data.maxPoints >= 3);

        if (data.numPoints < 2)
            data.reset();
    }

    int getMaxPoints() { return data.maxPoints; }
    juce::Array<Point>& getPoints() { return data.points; }

    //==============================================================================
    void setSampleRate (double sr)      { sampleRate = sr;  }
    void setParameters (Parameters p)   { parameters = p;   }
    void reset()
    {
        output     = 0.0f;
        phase      = 0.0f;
        curFade    = 1.0f;
        fadeDelta  = 0.0f;
        delaySteps = 0;
    }

    void setPoints (const juce::Array<Point>& p)
    {
        data.numPoints = p.size();
        for (auto i = 0; i < data.numPoints; i++)
            data.points[i] = p[i];
    }

    void noteOn (float phase_ = -1.0f)
    {
        if (parameters.fade <= 0)
            curFade = 1.0f;
        else
            curFade = 0.0f;

        phase      = phase_ < 0.0f ? parameters.phase : phase_;
        fadeDelta  = float (1.0f / (sampleRate * parameters.fade));
        delaySteps = juce::roundToInt (sampleRate * parameters.delay);

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

                phase += step;

                if (parameters.loop)
                {
                    auto pStart = data.points[data.startIndex].time;
                    auto pEnd   = data.points[data.endIndex].time;

                    if (pEnd - pStart < 0.0001)
                    {
                        if (phase > pEnd)
                            phase = pEnd;
                    }
                    else
                    {
                        while (phase >= pEnd)
                            phase -= pEnd - pStart;
                    }
                }
                else
                {
                    if (phase >= 1.0f)
                        phase = 1.0f;
                }
            }
        }

        return updateCurrentValue();
    }

    float getOutput()
    {
        return curFade * output * parameters.depth + parameters.offset;
    }

    float getCurrentPhase()
    {
        return phase;
    }

    // Get value at phase, not taking into account phase offset, fade, delay, depth, etc
    float getValueAt (float atPhase)
    {
        for (auto i = 0; i < data.numPoints - 1; i++)
        {
            const auto& p1 = data.points.getReference (i + 0);
            const auto& p2 = data.points.getReference (i + 1);

            if (atPhase >= p1.time && atPhase < p2.time)
            {
                auto dy = p2.value - p1.value;
                auto dx = p2.time  - p1.time;
                auto fr = (dx > 0.0f) ? ((atPhase - p1.time) / dx) : 1.0f;

                return dy * curve (fr, p1.curve) + p1.value;
            }
        }
        return 0;
    }

private:
    Data& data;

    float updateCurrentValue()
    {
        if (delaySteps == 0)
        {
            for (auto i = 0; i < data.numPoints - 1; i++)
            {
                const auto& p1 = data.points.getReference (i + 0);
                const auto& p2 = data.points.getReference (i + 1);

                if (phase >= p1.time && phase < p2.time)
                {
                    auto dy = p2.value - p1.value;
                    auto dx = p2.time  - p1.time;
                    auto fr = (dx > 0.0f) ? ((phase - p1.time) / dx) : 1.0f;

                    output  = dy * curve (fr, p1.curve) + p1.value;
                    break;
                }
            }
        }

        return curFade * output * parameters.depth + parameters.offset;
    }

    inline float curve (float fraction, float curve)
    {
        if (curve < 0.0f)
            return ((1.0f + -curve) * (fraction) / (1.0f + -curve * (fraction)));

        return ((1.0f + curve) * ((fraction) - 1) / (1.0f + curve * (1.0f - (fraction))) + 1);
    }

    Parameters parameters;

    double sampleRate = 0.0;
    float phase = 0.0f, output = 0.0f, fadeDelta = 0.0f, curFade = 1.0f;
    int delaySteps = 0;
};

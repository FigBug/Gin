/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Step sequencer-style LFO with programmable values per step.

    StepLFO provides a stepped low-frequency oscillator where each step can have
    an independently programmed value. Unlike smooth LFOs, this creates stepped/
    quantized modulation patterns useful for rhythmic effects and sequenced modulation.

    Key Features:
    - Configurable number of steps
    - Independent value per step (-1.0 to 1.0)
    - Adjustable frequency (rate)
    - Phase reset on note-on for tempo-synced patterns
    - Constant output per step (no interpolation)

    Usage:
    @code
    StepLFO stepLFO;
    stepLFO.setSampleRate(44100.0);
    stepLFO.setNumPoints(8); // 8-step sequence
    stepLFO.setFreq(2.0f);   // 2 Hz (complete cycle every 0.5 seconds)

    // Program step values
    stepLFO.setPoint(0, 1.0f);   // Step 1: max
    stepLFO.setPoint(1, 0.5f);   // Step 2: half
    stepLFO.setPoint(2, 0.0f);   // Step 3: center
    stepLFO.setPoint(3, -0.5f);  // Step 4: negative
    stepLFO.setPoint(4, -1.0f);  // Step 5: min
    stepLFO.setPoint(5, -0.5f);
    stepLFO.setPoint(6, 0.0f);
    stepLFO.setPoint(7, 0.5f);

    stepLFO.noteOn(); // Reset to beginning

    // In audio callback
    float modValue = stepLFO.process(numSamples);
    @endcode

    @see MSEG, LFO
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

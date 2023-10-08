/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/** Linear ADSR. Good for a modulation source
*/
class ADSR
{
public:
    ADSR()                                      {}
    ~ADSR()                                     {}

    enum State
    {
        idle,
        attack,
        decay,
        sustain,
        release,
        finished,
    };

    void setSampleRate (double sr)              { sampleRate = sr;      }

    float process();
    float process (int num);

    void process (juce::AudioSampleBuffer& buffer);
    void process (juce::AudioSampleBuffer& buffer, int startSample, int numSamples);

    float getOutput()
    {
        if (state == idle)
            return (attackDelta == 0.0f) ? 1.0f : 0.0f;

        jassert (output >= 0.0f && output <= 1.0f);
        return output;
    }

    std::pair<int, float> getCurrentPhase()
    {
        if (state == attack)
            return {0, output};
        if (state == decay)
            return {1, output};
        if (state == sustain)
            return {2, output};
        if (state == release)
            return {3, output};

        return {0,0.0f};
    }

    State getState()                            { return state;         }

    void noteOn()                               { state = attack;       }
    void noteOff()                              { state = release;      }

    void setAttack (float seconds);
    void setDecay (float seconds);
    void setRelease (float seconds);
    void setSustainLevel (float level);

    void reset();

protected:
    State state = idle;
    double sampleRate = 44100.0;
    float output = 0.0f, attackDelta = 0.0f, decayDelta = 0.0f, releaseDelta = 0.0f, sustainLevel = 0.0f;
};

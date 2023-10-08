/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
*/

#pragma once

/** Analog ADSR. Good for controlling audio levels.
    Based on the ADSR from the Will Pirkle book
*/
class AnalogADSR
{
public:
    AnalogADSR();
    ~AnalogADSR() = default;

    enum class State
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };

    void setSampleRate (double sr)              { sampleRate = sr;      }

    // Returns next ADSR value
    float process();
    float process (int num);
    
    // Fills a buffer with ADSR values
    void process (juce::AudioSampleBuffer& buffer);
    void process (juce::AudioSampleBuffer& buffer, int startSample, int numSamples);
    
    // Multiplies a buffer by ADSE values
    void processMultiplying (juce::AudioSampleBuffer& buffer);
    void processMultiplying (juce::AudioSampleBuffer& buffer, int startSample, int numSamples);

    float getOutput()                           { return output;        }
    State getState()                            { return state;         }

    void noteOn();
    void noteOff();

    void setAttack (float seconds);
    void setDecay (float seconds);
    void setRelease (float seconds);
    void setSustainLevel (float level);

    void reset();

    std::pair<int, float> getCurrentPhase()
    {
        if (state == State::attack)
            return {0, output};
        if (state == State::decay)
            return {1, output};
        if (state == State::sustain)
            return {2, output};
        if (state == State::release)
            return {3, output};

        return {0,0.0f};
    }

protected:
    void calculateAttack();
    void calculateDecay();
    void calculateRelease();

    State state = State::idle;
    double sampleRate = 44100.0;
    float attack = 0.0f, decay = 0.0f, sustain = 0.0f, release = 0.0f;
    float attackCoeff = 0.0f, decayCoeff = 0.0f, releaseCoeff = 0.0f;
    float attackOffset = 0.0f, decayOffset = 0.0f, releaseOffset = 0.0f;

    float output = 0.0f;
};

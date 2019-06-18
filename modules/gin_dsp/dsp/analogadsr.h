/*
 ==============================================================================
 
 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.
 
 ==============================================================================
*/

#pragma once

class AnalogADSR
{
public:
    AnalogADSR()                                {}
    ~AnalogADSR()                               {}
    
    enum class State
    {
        idle,
        attack,
        decay,
        sustain,
        release
    };
    
    void setSampleRate (double sr)              { sampleRate = sr;      }
    
	float process();
    void process (AudioSampleBuffer& buffer);
    void process (AudioSampleBuffer& buffer, int startSample, int numSamples);
    
    float getOutput()                           { return output;        }
    State getState()                            { return state;         }
    
    void noteOn();
    void noteOff();
    
    void setAttack (float seconds);
    void setDecay (float seconds);
    void setRelease (float seconds);
	void setSustainLevel (float level);
    
    void reset();

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




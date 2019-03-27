/*
 ==============================================================================
 
 ADSR.h
 Created: 24 Jan 2018 7:06:35pm
 Author:  Roland Rabien
 
 ==============================================================================
 */

#pragma once

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
        release
    };
    
    void setSampleRate (double sr)              { sampleRate = sr;      }
    
	float process();
    void process (AudioSampleBuffer& buffer);
    void process (AudioSampleBuffer& buffer, int startSample, int numSamples);
    
    float getOutput()                           { return output;        }
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




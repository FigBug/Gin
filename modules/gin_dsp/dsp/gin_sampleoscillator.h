/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** Oscillator for playing samples
*/
class SampleOscillator
{
public:
    SampleOscillator() = default;
    
    enum Direction
    {
        forwards,
        backwards,
        pingpong
    };
    
    void setSampleRateAndBlockSize (double sr, int bs);
    void setSample (Sample*);
    void setRange (int s, int e);
    void setLooping (bool l, int s = 0, int e = -1);
    void setDirection (Direction d);
    void setCrossfadeDuration (double s);
    
    void noteOn();
    void noteOff();
    
    bool read (double note, juce::AudioSampleBuffer& buffer, int start = 0, int length = -1);
    
private:
    void fillFifo (int samplesNeeded);
    
    Direction direction = forwards;
    int start = 0, end = 0, loopStart = 0, loopEnd = 0, pos = 0, currentDirection = 1;
    bool looping = false, triggered = false, finished = false;
    Sample* sample = nullptr;
    
    ResamplingFifo resampler {128, 2, 44100};
};

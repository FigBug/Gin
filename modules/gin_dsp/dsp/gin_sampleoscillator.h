/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Sample-based oscillator with pitch tracking and looping capabilities.

    SampleOscillator plays back audio samples at different pitches, similar to
    a sampler. It supports looping, bidirectional playback, sample range selection,
    and automatic resampling to match the target pitch.

    Key Features:
    - Pitch-shifted sample playback (follows MIDI note input)
    - Loop modes: forward, backward, ping-pong
    - Configurable loop points and sample range
    - Automatic high-quality resampling
    - Crossfade support for smooth loop transitions
    - Note on/off triggering

    Direction Modes:
    - forwards: Normal forward playback
    - backwards: Reverse playback
    - pingpong: Alternates between forward and backward

    Usage:
    @code
    SampleOscillator osc;
    osc.setSampleRateAndBlockSize(44100.0, 512);
    osc.setSample(&mySample);
    osc.setLooping(true, 1000, 5000); // Loop between samples 1000-5000
    osc.setDirection(SampleOscillator::forwards);
    osc.noteOn(); // Trigger playback

    // Pitch-shifted playback - plays at MIDI note 60 (C4)
    bool isStillPlaying = osc.read(60.0, audioBuffer);
    @endcode

    @see Sample, ResamplingFifo
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

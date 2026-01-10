/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
*/

#pragma once

/**
    Analog-modeled ADSR envelope generator for smooth, musical envelopes.

    AnalogADSR provides an envelope generator that emulates the behavior of
    analog synthesizer envelopes with smooth, exponential curves. It's ideal
    for controlling audio levels (amplitude envelopes) and produces more
    natural-sounding results than linear envelopes.

    Based on the ADSR implementation from Will Pirkle's book "Designing Software
    Synthesizer Plug-Ins in C++".

    Key Features:
    - Exponential attack, decay, and release curves (analog-like)
    - Per-sample or block processing
    - Can fill buffer with envelope values or multiply existing audio
    - State tracking (idle, attack, decay, sustain, release)
    - Reset capability for retriggering

    Envelope Stages:
    - Attack: Time to rise from 0 to peak (1.0)
    - Decay: Time to fall from peak to sustain level
    - Sustain: Held level while note is on (0.0 to 1.0)
    - Release: Time to fall from current level to 0 after note off

    Usage:
    @code
    AnalogADSR envelope;
    envelope.setSampleRate(44100.0);
    envelope.setAttack(0.01f);   // 10ms attack
    envelope.setDecay(0.1f);     // 100ms decay
    envelope.setSustainLevel(0.7f); // 70% sustain level
    envelope.setRelease(0.5f);   // 500ms release

    envelope.noteOn();
    // ... process audio ...
    envelope.processMultiplying(audioBuffer); // Apply envelope to audio
    // ... later ...
    envelope.noteOff(); // Begin release phase
    @endcode

    @see ADSR
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

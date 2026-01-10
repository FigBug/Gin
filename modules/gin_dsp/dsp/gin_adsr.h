/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

/**
    Linear ADSR envelope generator optimized for modulation sources.

    ADSR provides a simple, linear envelope generator with attack, decay, sustain,
    and release stages. Unlike AnalogADSR which uses exponential curves for audio,
    this class uses linear ramps making it ideal for modulation sources (LFO depth,
    filter cutoff, etc.) where linear response is often preferred.

    Key Features:
    - Linear attack, decay, and release stages
    - Constant sustain level while note is held
    - Per-sample or block processing
    - State tracking for envelope phase
    - Can fill buffer with envelope values

    Envelope Stages:
    - Attack: Linear rise from 0 to 1
    - Decay: Linear fall from 1 to sustain level
    - Sustain: Held at sustain level while note is on
    - Release: Linear fall from current level to 0

    Usage:
    @code
    ADSR envelope;
    envelope.setSampleRate(44100.0);
    envelope.setAttack(0.01f);       // 10ms attack
    envelope.setDecay(0.1f);         // 100ms decay
    envelope.setSustainLevel(0.7f);  // 70% sustain
    envelope.setRelease(0.5f);       // 500ms release

    envelope.noteOn();
    float modValue = envelope.process(blockSize);
    // ... later ...
    envelope.noteOff();
    @endcode

    @see AnalogADSR
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

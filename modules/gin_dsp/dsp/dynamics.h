/*
==============================================================================

This file is part of the GIN library.
Copyright (c) 2019 - Roland Rabien.

==============================================================================
*/

#pragma once

// Compressor / Gate / Expander / Limiter based on algorithm from
// Designing Audio Effect Plug-ins in C++ by Will Pirkle

//================================================================================
class EnvelopeDetector
{
public:
    EnvelopeDetector() = default;
    ~EnvelopeDetector() = default;

    enum Mode
    {
        peak,
        ms,
        rms
    };

    void setSampleRate (double f)           { sampleRate = f;       }

    void setParams (float attackS, float releaseS, bool analogTC, Mode detect, bool logDetector);
                                          
    void reset();
    float process (float input);

protected:
    void setAttackTime (float attackS);
    void setReleaseTime (float releaseS);

    double sampleRate = 44100.0;
    Mode mode = peak;
    float attackTime = 0.0f, releaseTime = 0.0f, envelope = 0.0;
    bool analogTC = false, logDetector = false;
};

//================================================================================
class Dynamics
{
public:
    Dynamics() = default;
    ~Dynamics() = default;

    enum Type
    {
        compressor,
        limiter,
        expander,
        gate,
    };

    void setSampleRate (double sampleRate);
    void setMode (Type t);
    void setParams (float attackS, float releaseS, float thresh, float ratio, float kneeWidth);
    
    void setInputGain (float g)     { inputGain = g;    }
    void setOutputGain (float g)    { outputGain = g;   }

    void reset();
    void process (AudioSampleBuffer& buffer);

private:
    float calcGain (float detectorValue);
    
    EnvelopeDetector leftEnvelope, rightEnvelope;

    double sampleRate = 44100.0;
    Type type = compressor;

    bool stereoLink = true;
    float inputGain = 1.0f, outputGain = 1.0f;
    float threshold = 0.0f, ratio = 0.0f, kneeWidth = 0.0f;
};

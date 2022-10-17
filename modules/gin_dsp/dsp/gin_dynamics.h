/*
==============================================================================

This file is part of the GIN library.
Copyright (c) 2019 - Roland Rabien.

==============================================================================
*/

#pragma once
//================================================================================
/** EnvelopeDetector for Dynamics
*/
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

    void setParams (float attackS, float holdS, float releaseS, bool analogTC, Mode detect, bool logDetector);

    void reset();
    float process (float input);

protected:
    void setAttackTime (float attackS);
    void setHoldTime (float holdS);
    void setReleaseTime (float releaseS);

    double sampleRate = 44100.0;
    Mode mode = peak;
    float attackTime = 0.0f, releaseTime = 0.0f, envelope = 0.0;
    float holdTime = 0.0f, holdRemaining = 0.0f;
    bool analogTC = false, logDetector = false;
};

//================================================================================
/**
 Compressor / Gate / Expander / Limiter based on algorithm from
 Designing Audio Effect Plug-ins in C++ by Will Pirkle

 */
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
    void setNumChannels (int ch);
    void setMode (Type t)                   { type = t;             }
    void setLinked (bool l)                 { channelsLinked = l;   }

    void setParams (float attackS, float holdS, float releaseS, float thresh, float ratio, float kneeWidth);

    void setInputGain (float g)             { inputGain = g;    }
    void setOutputGain (float g)            { outputGain = g;   }

    void reset();
    void process (juce::AudioSampleBuffer& buffer, juce::AudioSampleBuffer* envelopeOut = nullptr);

    const LevelTracker& getInputTracker()       { return inputTracker;      }
    const LevelTracker& getOutputTracker()      { return outputTracker;     }
    const LevelTracker& getReductionTracker()   { return reductionTracker;  }

    float calcCurve (float detectorValue);

private:
    juce::OwnedArray<EnvelopeDetector> envelopes;
    LevelTracker inputTracker, outputTracker, reductionTracker {-30.0f};

    double sampleRate = 44100.0;
    Type type = compressor;

    int channels = 0;
    bool channelsLinked = true;
    float inputGain = 1.0f, outputGain = 1.0f;
    float threshold = 0.0f, ratio = 0.0f, kneeWidth = 0.0f;
};

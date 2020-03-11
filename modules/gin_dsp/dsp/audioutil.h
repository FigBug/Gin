/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
inline Value findValue (ValueTree& state, Identifier name, var value)
{
    if (state.hasProperty (name))
        return state.getPropertyAsValue (name, nullptr);

    state.setProperty (name, value, nullptr);
    return state.getPropertyAsValue (name, nullptr);
}

//==============================================================================
inline double getMidiNoteInHertz (const double noteNumber, const double frequencyOfA = 440.0)
{
    return frequencyOfA * pow (2.0, (noteNumber - 69) / 12.0);
}

//==============================================================================
inline float velocityToGain (float velocity, float sensitivity = 1.0f)
{
    float v = velocity * sensitivity + 1.0f - sensitivity;
    return v * std::pow (25.0f, v) * 0.04f;
}

//==============================================================================
void applyGain (AudioSampleBuffer& buffer, LinearSmoothedValue<float>& gain);

void applyGain (AudioSampleBuffer& buffer, int channel, LinearSmoothedValue<float>& gain);

void clip (AudioSampleBuffer& buffer, float low = -1.0f, float high = 1.0f);

//==============================================================================
class LevelTracker
{
public:
    LevelTracker (float decayPerSecond = 30.0f);

    void trackBuffer (const float* buffer, int numSamples);
    void trackBuffer (AudioSampleBuffer& buffer);

    float getLevel() const;
    bool getClip() const    { return clip;  }
    void clearClip()        { clip = false; }

protected:
    float peakTime  = 0.0f;
    float peakLevel = -100.0f;
    float decayRate = 0.0f;
    bool clip       = false;
};

//==============================================================================
// Type string for a midi message
String getMidiMessageType (const MidiMessage& msg);

//==============================================================================
class WetDryMix
{
public:
    WetDryMix (float mix)
        : wetGain (mixToGain (mix)),
          dryGain (mixToGain (1.0f - mix))
    {
    }

    float mixToGain (float mix)
    {
        jassert (mix >= 0.0f && mix <= 1.0f);
        return std::sin (mix * (0.5f * MathConstants<float>::pi));
    }
    
    float wetGain, dryGain;
};

//==============================================================================
inline AudioSampleBuffer sliceBuffer (AudioSampleBuffer& input, int start, int length)
{
    return AudioSampleBuffer (input.getArrayOfWritePointers(), input.getNumChannels(), start, length);
}

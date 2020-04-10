/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

#define Q 0.70710678118655f

//==============================================================================
inline Value findValue (ValueTree& state, Identifier name, var value)
{
    if (state.hasProperty (name))
        return state.getPropertyAsValue (name, nullptr);

    state.setProperty (name, value, nullptr);
    return state.getPropertyAsValue (name, nullptr);
}

//==============================================================================
template <class T>
inline T getMidiNoteInHertz (const T noteNumber, const T frequencyOfA = (T)440.0)
{
    return T (frequencyOfA * std::pow (2.0, (noteNumber - 69.0) / 12.0));
}

template <class T>
inline T getMidiNoteFromHertz (const T freq, const T frequencyOfA = (T)440.0)
{
    return T (12 * std::log2 (double (freq) / frequencyOfA) + 69.0);
}

//==============================================================================
inline float velocityToGain (float velocity, float sensitivity = 1.0f)
{
    float v = velocity * sensitivity + 1.0f - sensitivity;
    return jlimit (0.0f, 1.0f, v * std::pow (25.0f, v) * 0.04f);
}

//==============================================================================
void applyGain (AudioSampleBuffer& buffer, LinearSmoothedValue<float>& gain);

void applyGain (AudioSampleBuffer& buffer, int channel, LinearSmoothedValue<float>& gain);

void clip (AudioSampleBuffer& buffer, float low = -1.0f, float high = 1.0f);

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

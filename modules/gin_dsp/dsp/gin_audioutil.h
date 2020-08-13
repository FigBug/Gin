/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

inline constexpr float Q = 0.70710678118655f;

//==============================================================================
inline juce::Value findValue (juce::ValueTree& state, juce::Identifier name, juce::var value)
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
    return juce::jlimit (0.0f, 1.0f, v * std::pow (25.0f, v) * 0.04f);
}

//==============================================================================
void applyGain (juce::AudioSampleBuffer& buffer, juce::LinearSmoothedValue<float>& gain);

void applyGain (juce::AudioSampleBuffer& buffer, int channel, juce::LinearSmoothedValue<float>& gain);

void clip (juce::AudioSampleBuffer& buffer, float low = -1.0f, float high = 1.0f);

class GainProcessor
{
public:
    void setGain (float g)  { gain = g;         }
    void reset()            { lastGain = gain;  }

    void process (juce::AudioSampleBuffer& buffer)
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), lastGain, gain);
        lastGain = gain;
    }

private:
    float gain = 1.0, lastGain = 1.0;
};

//==============================================================================
// Type string for a midi message
juce::String getMidiMessageType (const juce::MidiMessage& msg);

//==============================================================================
/** Mix between wet and dry with a nice curve */
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
        return std::sin (mix * (0.5f * juce::MathConstants<float>::pi));
    }

    float wetGain, dryGain;
};

//==============================================================================
/** Get a section of an audio buffer */
inline juce::AudioSampleBuffer sliceBuffer (juce::AudioSampleBuffer& input, int start, int length)
{
    return juce::AudioSampleBuffer (input.getArrayOfWritePointers(), input.getNumChannels(), start, length);
}

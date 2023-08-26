/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "gin_scratchbuffer.h"

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

    void setMix (float mix)
    {
        wetGain = mixToGain (mix);
        dryGain = mixToGain (1.0f - mix);
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

//==============================================================================
/** Get a buffer but mono */
inline ScratchBuffer monoBuffer (juce::AudioSampleBuffer& input)
{
    ScratchBuffer output (1, input.getNumSamples());
    output.clear();

    juce::FloatVectorOperations::addWithMultiply (output.getWritePointer (0), input.getReadPointer (0), 0.5f, input.getNumSamples());
    juce::FloatVectorOperations::addWithMultiply (output.getWritePointer (0), input.getReadPointer (1), 0.5f, input.getNumSamples());

    return output;
}

/** Computes the linear interpolation between a and b, if the parameter t is inside [0, 1] */
template <class T>
T lerp (T a, T b, T t)
{
    return a + t * (b - a);
}

/** Sign +1 or -1 of number */
template <class T>
inline int sign ( T x )
{
    return ( x > 0 ? 1 : -1 );
}

//==============================================================================
/** Distortion */
namespace Distortion
{
    inline float saturate (float input, float drive, float lowclip, float highclip)
    {
        input = juce::jlimit (lowclip, highclip, input);
        return input - drive * input * std::fabs (input);
    }

    inline void processBlock (juce::AudioSampleBuffer& buffer, float drive, float lowclip, float highclip)
    {
        if (drive <= 0.0f)
            return;

        float gain = drive < 0.5f ? 2.0f * drive + 1.0f : drive * 4.0f;

        for (auto ch = 0; ch < buffer.getNumChannels(); ch++)
        {
            auto data = buffer.getWritePointer (ch);
            for (auto s = buffer.getNumSamples(); --s >= 0;)
                data[s] = saturate (data[s], drive, lowclip, highclip) * gain;
        }
    }
}

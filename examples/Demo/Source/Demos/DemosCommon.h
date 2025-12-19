/*
  ==============================================================================

    DemosCommon.h
    Common utilities and shared resources for demo components

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

inline juce::ThreadPool& getDemoThreadPool()
{
    static juce::ThreadPool pool (juce::SystemStats::getNumCpus());
    return pool;
}

//==============================================================================
// Base class for all demo components
// Provides optional audio processing capability via processBlock
class DemoComponent : public juce::Component
{
public:
    DemoComponent() = default;
    ~DemoComponent() override = default;

    // Override this to process audio when the demo is active
    virtual void processBlock (juce::AudioBuffer<float>& buffer)
    {
        juce::ignoreUnused (buffer);
    }

    // Called when audio playback is about to start
    virtual void prepareToPlay (double sampleRate, int samplesPerBlock)
    {
        juce::ignoreUnused (sampleRate, samplesPerBlock);
    }

    // Called when audio playback stops
    virtual void releaseResources() {}
};

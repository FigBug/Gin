#pragma once

#include <JuceHeader.h>

class UnitTestsAudioProcessor : public gin::Processor
{
public:
    UnitTestsAudioProcessor();
    ~UnitTestsAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestsAudioProcessor)
};

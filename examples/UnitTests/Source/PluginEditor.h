#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class UnitTestsAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::Timer
{
public:
    UnitTestsAudioProcessorEditor (UnitTestsAudioProcessor&);
    ~UnitTestsAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void runUnitTests();

    juce::TextEditor logTextEditor;
    int numFailures = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestsAudioProcessorEditor)
};

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class PrintfLogger : public juce::Logger
{
public:
    void logMessage (const juce::String& message) override
    {
        printf ("%s\n", message.toRawUTF8());

        #if JUCE_WINDOWS
        outputDebugString (message);
        #endif
    }
};

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

    PrintfLogger logger;

    juce::TextEditor logTextEditor;
    int numFailures = 0;

    juce::OwnedArray<juce::Component> demoComponents;
    int currentDemoIndex = 0;

    int timerCount = 0;
    bool unitTestsStarted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UnitTestsAudioProcessorEditor)
};

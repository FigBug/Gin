/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Demos/DemosCommon.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public juce::Component,
                             public juce::ListBoxModel,
                             public juce::DragAndDropContainer,
                             public juce::AudioSource
{
public:
    //==============================================================================
    MainContentComponent();
    ~MainContentComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // AudioSource
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;

    juce::ListBox demoList;
    juce::OwnedArray<DemoComponent> demoComponents;

private:
    int getNumRows() override   { return demoComponents.size(); }
    void paintListBoxItem (int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged (int lastRowSelected) override;

    DemoComponent* getActiveDemo();

    juce::AudioDeviceManager audioDeviceManager;
    juce::AudioSourcePlayer audioSourcePlayer;
    double currentSampleRate = 44100.0;
    int currentBlockSize = 512;

    juce::Component::SafePointer<gin::ComponentViewer> componentViewer;
    juce::TextButton toggleComponentViewer {"Component Viewer"};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

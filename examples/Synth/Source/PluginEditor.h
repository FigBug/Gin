#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Panels.h"
#include "Editor.h"

//==============================================================================
class SynthAudioProcessorEditor : public gin::ProcessorEditor,
                                  public juce::DragAndDropContainer
{
public:
    SynthAudioProcessorEditor (SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void addMenuItems (juce::PopupMenu& m) override;

private:
    SynthAudioProcessor& wtProc;

    gin::TriggeredScope scope { wtProc.scopeFifo };
    gin::SynthesiserUsage usage { wtProc };
    
    Editor editor { wtProc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthAudioProcessorEditor)
};

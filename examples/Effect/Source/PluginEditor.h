#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class EffectAudioProcessorEditor : public gin::ProcessorEditor
{
public:
    EffectAudioProcessorEditor (EffectAudioProcessor&);
    ~EffectAudioProcessorEditor() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectAudioProcessorEditor)
};

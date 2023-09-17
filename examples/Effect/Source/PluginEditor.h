#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class EffectAudioProcessorEditor : public gin::ProcessorEditor
{
public:
    EffectAudioProcessorEditor (EffectAudioProcessor&);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectAudioProcessorEditor)
};

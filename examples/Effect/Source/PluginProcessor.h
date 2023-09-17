#pragma once

#include <JuceHeader.h>

//==============================================================================
class EffectAudioProcessor : public gin::Processor
{
public:
    //==============================================================================
    EffectAudioProcessor();
    ~EffectAudioProcessor() override;

    //==============================================================================
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    gin::Parameter::Ptr levelParam, panParam, modeParam, invertParam;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EffectAudioProcessor)
};

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Panels.h"

//==============================================================================
class Editor : public juce::Component
{
public:
    Editor (SynthAudioProcessor& proc_)
        : proc ( proc_ )
    {
        addAndMakeVisible (oscillators);
        addAndMakeVisible (filters);
        addAndMakeVisible (fltADSR);
    }

    void resized() override
    {
        auto rc = getLocalBounds();

        auto rcOsc = rc.removeFromTop (163);
        oscillators.setBounds (rcOsc.removeFromLeft (224));
        rcOsc.removeFromLeft (1);

        rc.removeFromTop (1);

        auto rcFlt = rc.removeFromTop (163);
        filters.setBounds (rcFlt.removeFromLeft (168));  rcFlt.removeFromLeft (1);
        fltADSR.setBounds (rcFlt.removeFromLeft (186));  rcFlt.removeFromLeft (1);
    }

    SynthAudioProcessor& proc;

    OscillatorBox   oscillators { "oscillator 1", proc };
    FilterBox       filters     { "filter 1", proc };
    FilterADSRArea  fltADSR     { proc };
};

//==============================================================================
class SynthAudioProcessorEditor : public gin::ProcessorEditor
{
public:
    SynthAudioProcessorEditor (SynthAudioProcessor&);
    ~SynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SynthAudioProcessor& vaProc;

    Editor editor { vaProc };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthAudioProcessorEditor)
};

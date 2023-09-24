#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Panels.h"

//==============================================================================
class Editor : public juce::Component
{
public:
    Editor (SynthAudioProcessor& proc_);

    void setupCallbacks();
    void resized() override;

private:
    SynthAudioProcessor& proc;

    OscillatorBox oscillator                { "oscillator", proc };

    ADSRBox adsr                            { "adsr", proc };
    FilterBox filter                        { "filter", proc };

    LFOBox lfo                              { "LFO", proc };
    ModBox mod                              { "mod", proc };
    MatrixBox mtx                           { "mtx", proc };
    GlobalBox global                        { "global", proc };

    DelayBox delay { proc };

    gin::Layout layout { *this };
};

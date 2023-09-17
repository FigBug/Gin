#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EffectAudioProcessorEditor::EffectAudioProcessorEditor (EffectAudioProcessor& p)
    : ProcessorEditor (p)
{
    setGridSize (6, 1);
    
    addControl (new gin::Select (p.modeParam), 1, 0);
    addControl (new gin::Knob (p.levelParam), 2, 0);
    addControl (new gin::Knob (p.panParam, true), 3, 0);
    addControl (new gin::Switch (p.invertParam), 4, 0);
}


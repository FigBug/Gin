#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EffectAudioProcessorEditor::EffectAudioProcessorEditor (EffectAudioProcessor& p)
    : ProcessorEditor (p)
{
    setGridSize (6, 1);
    
    addAndMakeVisible (controls.add (new gin::Select (p.modeParam)));
    addAndMakeVisible (controls.add (new gin::Knob (p.levelParam)));
    addAndMakeVisible (controls.add (new gin::Knob (p.panParam)));
    addAndMakeVisible (controls.add (new gin::Switch (p.invertParam)));
    
    componentForParam (*p.modeParam)->setBounds (getGridArea (1, 0));
    componentForParam (*p.levelParam)->setBounds (getGridArea (2, 0));
    componentForParam (*p.panParam)->setBounds (getGridArea (3, 0));
    componentForParam (*p.invertParam)->setBounds (getGridArea (4, 0));
}

EffectAudioProcessorEditor::~EffectAudioProcessorEditor()
{
}


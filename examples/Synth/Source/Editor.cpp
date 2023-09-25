#include "Editor.h"

Editor::Editor (SynthAudioProcessor& proc_)
    : proc ( proc_ )
{
    addAndMakeVisible (oscillator);
    addAndMakeVisible (lfo);
    addAndMakeVisible (adsr);
    addAndMakeVisible (filter);
    addAndMakeVisible (mod);
    addAndMakeVisible (mtx);
    addAndMakeVisible (global);
    addAndMakeVisible (delay);

    setupCallbacks();
}

void Editor::setupCallbacks()
{
}

void Editor::resized()
{
#if JUCE_DEBUG
    auto f = juce::File (__FILE__).getChildFile ("../../Resources/layout.json");

    layout.setLayout ("layout.json", f);
#else
    layout.setLayout ("layout.json");
#endif
}

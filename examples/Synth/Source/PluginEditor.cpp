#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SynthAudioProcessorEditor::SynthAudioProcessorEditor (SynthAudioProcessor& p)
    : ProcessorEditor (p), wtProc (p)
{
    scope.setName ("scope");
    scope.setNumChannels (2);
    scope.setTriggerMode (gin::TriggeredScope::TriggerMode::Up);
    scope.setColour (gin::TriggeredScope::traceColourId + 0, findColour(gin::PluginLookAndFeel::accentColourId, true).withAlpha (0.7f));
    scope.setColour (gin::TriggeredScope::traceColourId + 1, findColour(gin::PluginLookAndFeel::accentColourId, true).withAlpha (0.7f));
    scope.setColour (gin::TriggeredScope::lineColourId, juce::Colours::transparentBlack);

    addAndMakeVisible (editor);
    addAndMakeVisible (scope);
    
    usage.panic.onClick = [this] { wtProc.presetLoaded = true; };
    addAndMakeVisible (usage);
    
    usage.setBounds (45, 12, 80, 16);
    scope.setBounds (520, 5, 65, 30);

    setSize (617, 450);
}

SynthAudioProcessorEditor::~SynthAudioProcessorEditor()
{
}

//==============================================================================
void SynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    ProcessorEditor::paint (g);

    titleBar.setShowBrowser (true);

    g.fillAll (findColour (gin::PluginLookAndFeel::blackColourId));
}

void SynthAudioProcessorEditor::resized()
{
    ProcessorEditor::resized ();

    auto rc = getLocalBounds().reduced (1);
    rc.removeFromTop (40);

    editor.setBounds (rc);
    patchBrowser.setBounds (rc);
}

void SynthAudioProcessorEditor::addMenuItems (juce::PopupMenu& m)
{
    m.addSeparator();
    m.addItem ("MPE", true, wtProc.globalParams.mpe->getUserValueBool(), [this]
    {
        wtProc.globalParams.mpe->setUserValue (wtProc.globalParams.mpe->getUserValueBool() ? 0.0f : 1.0f);
    });

    auto setSize = [this] (float scale)
    {
        if (auto p = findParentComponentOfClass<gin::ScaledPluginEditor>())
            p->setScale (scale);
    };

    juce::PopupMenu um;
    um.addItem ("50%",  [setSize] { setSize (0.50f); });
    um.addItem ("75%",  [setSize] { setSize (0.75f); });
    um.addItem ("100%", [setSize] { setSize (1.00f); });
    um.addItem ("150%", [setSize] { setSize (1.50f); });
    um.addItem ("200%", [setSize] { setSize (2.00f); });

    m.addSubMenu ("UI Size", um);
}

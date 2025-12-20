/*
  ==============================================================================

    ValueTreeJsonDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct ValueTreeJsonDemo : public DemoComponent,
                           private juce::TextEditor::Listener
{
    ValueTreeJsonDemo()
    {
        setName ("ValueTree Json");

        addAndMakeVisible (xmlIn);
        addAndMakeVisible (xmlOut);
        addAndMakeVisible (jsonOut);

        xmlIn.setTextToShowWhenEmpty ("Add some ValueTree XML here and hit enter", juce::Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("JSON will appear here", juce::Colours::white.withAlpha (0.5f));
        xmlOut.setTextToShowWhenEmpty ("And hopeful original xml will reappear here", juce::Colours::white.withAlpha (0.5f));

        xmlIn.setMultiLine (true);
        jsonOut.setReadOnly (true);
        jsonOut.setMultiLine (true);
        xmlOut.setMultiLine (true);
        xmlOut.setReadOnly (true);

        xmlIn.addListener (this);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        int h = rc.getHeight() / 3;

        xmlIn.setBounds (rc.removeFromTop (h));
        jsonOut.setBounds (rc.removeFromTop (h));
        xmlOut.setBounds (rc);
    }

    void textEditorReturnKeyPressed (juce::TextEditor&) override
    {
        auto vt1 = juce::ValueTree::fromXml (xmlIn.getText());

        auto json = gin::valueTreeToJSON (vt1);
        auto vt2 = gin::valueTreeFromJSON (json);

        jsonOut.setText (json);
        xmlOut.setText (vt2.toXmlString());
    }

    juce::TextEditor xmlIn, xmlOut, jsonOut;
};

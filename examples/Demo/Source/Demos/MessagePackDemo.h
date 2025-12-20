/*
  ==============================================================================

    MessagePackDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct MessagePackDemo : public DemoComponent,
                         private juce::TextEditor::Listener
{
    MessagePackDemo()
    {
        setName ("Message Pack");

        addAndMakeVisible (jsonIn);
        addAndMakeVisible (raw);
        addAndMakeVisible (jsonOut);

        jsonIn.setTextToShowWhenEmpty ("Add some json here and hit enter", juce::Colours::white.withAlpha (0.5f));
        raw.setTextToShowWhenEmpty ("Base64 MessagePack will appear here", juce::Colours::white.withAlpha (0.5f));
        jsonOut.setTextToShowWhenEmpty ("And hopeful original json will reappear here", juce::Colours::white.withAlpha (0.5f));

        jsonIn.setMultiLine (true);
        raw.setReadOnly (true);
        jsonOut.setMultiLine (true);
        jsonOut.setReadOnly (true);

        jsonIn.addListener (this);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        int h = (rc.getHeight() - 20) / 2;

        jsonIn.setBounds (rc.removeFromTop (h));
        jsonOut.setBounds (rc.removeFromBottom (h));
        raw.setBounds (rc);
    }

    void textEditorReturnKeyPressed (juce::TextEditor&) override
    {
        juce::var v1 = juce::JSON::parse (jsonIn.getText());
        auto mb = gin::MessagePack::toMessagePack (v1);

        raw.setText (juce::Base64::toBase64 (mb.getData(), mb.getSize()), juce::dontSendNotification);

        juce::var v2 = gin::MessagePack::parse (mb);
        jsonOut.setText (juce::JSON::toString (v2));
    }

    juce::TextEditor jsonIn, raw, jsonOut;
};

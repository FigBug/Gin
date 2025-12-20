/*
  ==============================================================================

    SharedMemoryDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct SharedMemoryDemo : public DemoComponent,
                          private juce::TextEditor::Listener,
                          private juce::Timer
{
    SharedMemoryDemo()
    {
        setName ("Shared Memory");
        addAndMakeVisible (text);
        text.addListener (this);
        text.setText ("Launch two copies of the app and then type in this box");

        startTimerHz (30);
    }

    void resized() override
    {
        text.setBounds (getLocalBounds());
    }

    void textEditorTextChanged (juce::TextEditor&) override
    {
        strncpy ((char*)mem.getData(), text.getText().toRawUTF8(), size_t (mem.getSize() - 1));
    }

    void timerCallback() override
    {
        if (! text.hasKeyboardFocus (true))
        {
            juce::String fromMem ((char*)mem.getData(), size_t (mem.getSize()));

            if (fromMem != text.getText())
                text.setText (fromMem);
        }
    }

    gin::SharedMemory mem {"demo", 1024};
    juce::TextEditor text;
};

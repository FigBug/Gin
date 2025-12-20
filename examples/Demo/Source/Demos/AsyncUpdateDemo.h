/*
  ==============================================================================

    AsyncUpdateDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct AsyncUpdateDemo : public DemoComponent,
                         private juce::Thread,
                         private gin::RealtimeAsyncUpdater
{
    AsyncUpdateDemo() : Thread ("AsyncUpdateDemo")
    {
        setName ("AsyncUpdate");

        addAndMakeVisible (text);
        text.setReadOnly (true);

        startThread();
    }

    ~AsyncUpdateDemo() override
    {
        stopThread (1000);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        text.setBounds (rc);
    }

    void run() override
    {
        while (! threadShouldExit())
        {
            count += 1;
            triggerAsyncUpdate();
        }
    }

    void handleAsyncUpdate() override
    {
        text.setText (juce::String (count.get()));
    }

    juce::TextEditor text;
    juce::Atomic<int> count;
};

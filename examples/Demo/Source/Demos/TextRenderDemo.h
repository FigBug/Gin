/*
  ==============================================================================

    TextRenderDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct TextRenderDemo final : public DemoComponent
{
    TextRenderDemo()
    {
        setName ("Wave to Text Render");

        addAndMakeVisible (text);
        text.setFont (juce::Font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 10.0f, juce::Font::plain)));
        text.setReadOnly (true);
        text.setMultiLine (true, false);

        juce::AudioSampleBuffer buffer (2, 512);

        gin::BandLimitedLookupTables bllt;
        gin::StereoOscillator osc (bllt);

        gin::StereoOscillator::Params p;
        p.wave = gin::Wave::sine;

        osc.noteOn();
        osc.process (69, p, buffer);

        auto txt = gin::TextRenderer::renderBuffer (buffer, 7, 4);
        text.setText (txt, juce::dontSendNotification);
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        text.setBounds (rc);
    }

    juce::TextEditor text;
};

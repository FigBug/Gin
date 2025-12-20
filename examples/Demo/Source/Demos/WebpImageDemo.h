/*
  ==============================================================================

    WebpImageDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct WebpImageDemo : public DemoComponent
{
    WebpImageDemo()
    {
        setName ("Webp Image");

        gin::WEBPImageFormat webp;

        {
            juce::MemoryBlock mb (BinaryData::_1_webp, BinaryData::_1_webpSize);
            juce::MemoryInputStream is (mb, false);

            if (webp.canUnderstand (is))
                source1 = webp.decodeImage (is);
        }
        {
            juce::MemoryBlock mb (BinaryData::_2_webp, BinaryData::_2_webpSize);
            juce::MemoryInputStream is (mb, false);

            if (webp.canUnderstand (is))
                source2 = webp.decodeImage (is);
        }
    }

    void paint (juce::Graphics& g) override
    {
        auto rc = getLocalBounds();

        g.fillAll (juce::Colours::black);
        if (source1.isValid())
            g.drawImage (source1, rc.removeFromLeft (rc.getWidth() / 2).toFloat(), juce::RectanglePlacement::centred);
        if (source2.isValid())
            g.drawImage (source2, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image source1, source2;
};

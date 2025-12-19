/*
  ==============================================================================

    BmpImageDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct BmpImageDemo : public DemoComponent
{
    BmpImageDemo()
    {
        setName ("BMP Image");

        gin::BMPImageFormat bmp;

        {
            juce::MemoryBlock mb (BinaryData::ballon_bmp, BinaryData::ballon_bmpSize);
            juce::MemoryInputStream is (mb, false);

            source1 = bmp.decodeImage (is);
        }
        {
            juce::MemoryBlock mb (BinaryData::ballon_8bit_bmp, BinaryData::ballon_8bit_bmpSize);
            juce::MemoryInputStream is (mb, false);

            source2 = bmp.decodeImage (is);
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

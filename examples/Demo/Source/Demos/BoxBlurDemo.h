/*
  ==============================================================================

    BoxBlurDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct BoxBlurDemo : public DemoComponent,
                     private juce::Slider::Listener
{
    BoxBlurDemo()
    {
        setName ("Box Blur Effects");

        auto source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);
        sourceARGB = source.convertedToFormat (juce::Image::ARGB);
        sourceRGB = source.convertedToFormat (juce::Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (radius);
        radius.addListener (this);
        radius.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        radius.setRange (2, 254);
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        radius.setBounds (rc.removeFromLeft (w));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        auto rc = getLocalBounds();
        int w = rc.getWidth() / 3;

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceARGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc.removeFromLeft (w));

            auto img = sourceRGB.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (rc);

            auto img = sourceBW.createCopy();
            gin::applyStackBlur (img, (int) radius.getValue());
            g.drawImage (img, getLocalBounds().toFloat(), juce::RectanglePlacement::centred);
        }
    }

    juce::Image convertToBW (const juce::Image& src)
    {
        auto dst = juce::Image (juce::Image::SingleChannel, src.getWidth(), src.getHeight(), true);

        for (int y = 0; y < src.getHeight(); y++)
        {
            for (int x = 0; x < src.getWidth(); x++)
            {
                auto colour = src.getPixelAt (x, y);
                uint8_t bw = (colour.getRed() + colour.getGreen() + colour.getBlue()) / 3;
                dst.setPixelAt (x, y, juce::Colour (bw, bw, bw, bw));
            }
        }

        return dst;
    }

    juce::Image sourceARGB, sourceRGB, sourceBW;
    juce::Slider radius;
};

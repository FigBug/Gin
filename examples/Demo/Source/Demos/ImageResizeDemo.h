/*
  ==============================================================================

    ImageResizeDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct ImageResizeDemo : public DemoComponent,
                         private juce::Slider::Listener
{
    ImageResizeDemo()
    {
        setName ("Image Resize");

        auto source = juce::ImageFileFormat::loadFrom (BinaryData::pencils_jpeg, BinaryData::pencils_jpegSize);

        source = gin::applyResize (source, 0.97f, gin::ResizeAlgorirm::avir);

        sourceARGB = source.convertedToFormat (juce::Image::ARGB);
        sourceRGB = source.convertedToFormat (juce::Image::RGB);
        sourceBW = convertToBW (source);

        addAndMakeVisible (zoom);
        zoom.addListener (this);
        zoom.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

        zoom.setRange (0.1, 4.0);
        zoom.setValue (1.0);
    }

    void sliderValueChanged (juce::Slider*) override
    {
        repaint();
    }

    void resized() override
    {
        auto rc = getLocalBounds().removeFromBottom (20);
        int w = rc.getWidth() / 3;
        zoom.setBounds (rc.removeFromLeft (w));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto t1 = juce::Time::getMillisecondCounterHiRes();
        auto zoomed1 = gin::applyResize (sourceARGB, (float) zoom.getValue(), gin::ResizeAlgorirm::avir);
        auto t2 = juce::Time::getMillisecondCounterHiRes();
        auto zoomed2 = gin::applyResize (sourceARGB, (float) zoom.getValue(), gin::ResizeAlgorirm::lanczos);
        auto t3 = juce::Time::getMillisecondCounterHiRes();

        printf ("ImageResizeDemo: AVIR:     %.1f ms\n", t2 - t1);
        printf ("ImageResizeDemo: Lanczos:  %.1f ms\n", t3 - t2);
        printf ("ImageResizeDemo: Ratio:    %.1f x\n", (t2 - t1) / (t3 - t2));

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (getLocalBounds().removeFromLeft (getWidth() / 2 + 1));
            g.drawImageAt (zoomed1, getWidth() / 2 - zoomed1.getWidth() / 2, getHeight() / 2 - zoomed1.getHeight() / 2);
        }

        {
            juce::Graphics::ScopedSaveState sss (g);
            g.reduceClipRegion (getLocalBounds().removeFromRight (getWidth() / 2 + 1));
            g.drawImageAt (zoomed2, getWidth() / 2 - zoomed2.getWidth() / 2, getHeight() / 2 - zoomed2.getHeight() / 2);
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
    juce::Slider zoom;
};

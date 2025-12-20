/*
  ==============================================================================

    PerlinNoiseDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct PerlinNoiseDemo : public DemoComponent
{
public:
    PerlinNoiseDemo()
    {
        setName ("Perlin Noise");

        gin::PerlinNoise<float> n (5);

        for (int x = 0; x < 500; x++)
            for (int y = 0; y < 500; y++)
                img.setPixelAt (x, y, juce::Colour::greyLevel (n.noise (x / 12.0f, y / 12.0f)));
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);

        auto rc = getLocalBounds();

        g.drawImage (img, rc.toFloat(), juce::RectanglePlacement::centred);
    }

    juce::Image img {juce::Image::ARGB, 500, 500, true};
};

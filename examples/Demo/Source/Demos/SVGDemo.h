/*
  ==============================================================================

    SVGDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct SVGDemo : public DemoComponent
{
    SVGDemo()
    {
        setName ("SVG");
    }

    void paint (juce::Graphics& g) override
    {
        auto svg = juce::String (BinaryData::SVG_example_markup_grid_svg, BinaryData::SVG_example_markup_grid_svgSize);
        auto img = gin::rasterizeSVG (svg, getWidth(), getHeight());

        g.drawImageAt (img, 0, 0);
    }
};

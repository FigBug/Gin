/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

namespace SVG
{
    enum class Engine
    {
        nanosvg,
        juce
    };

    juce::Image renderToImage (const juce::String& svgText, int width, int height,
                               Engine engine = Engine::juce);
    juce::Image renderToImage (const juce::String& svgText, int width, int height,
                               juce::Colour backgroundColour, Engine engine = Engine::juce);

    juce::Path renderToPath (const juce::String& svgText);
    juce::Path renderToPath (const juce::String& svgText, juce::Rectangle<float> targetBounds);

    void render (const juce::String& svgText, juce::Graphics& g,
                 juce::Rectangle<float> targetBounds, Engine engine = Engine::juce);
}

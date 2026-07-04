/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** SVG parsing and rendering helpers built on NanoSVG.

    Why use these instead of JUCE's built-in SVG support? These helpers honour
    the source SVG's view box (width/height) when fitting into a target
    rectangle, preserving aspect ratio. JUCE's Drawable::createFromSVG and
    Path::createPathFromString collapse shapes into the path's own bounding
    box, which loses the view box and distorts artwork that has padding or
    off-centre content.
*/
namespace SVG
{
    /** Rendering backend.

        - juce:    rasterises by replaying shapes through juce::Graphics
                   (vector — scales cleanly, respects host transforms).
        - nanosvg: rasterises with NanoSVG's built-in rasteriser into a
                   bitmap (typically faster, but pixel-bound).
    */
    enum class Engine
    {
        nanosvg,
        juce
    };

    /** Renders svgText into a width x height ARGB image with a transparent
        background. The SVG is scaled uniformly to fit and centred — its view
        box aspect ratio is preserved. */
    juce::Image renderToImage (const juce::String& svgText, int width, int height,
                               Engine engine = Engine::juce);

    /** As above, but fills the image with backgroundColour before drawing. */
    juce::Image renderToImage (const juce::String& svgText, int width, int height,
                               juce::Colour backgroundColour, Engine engine = Engine::juce);

    /** Returns all visible shapes flattened into a single juce::Path in the
        SVG's own coordinate space. Note: the resulting path's bounding box is
        the union of its shapes, NOT the SVG view box, so transforming it
        directly will not preserve the view box — use the targetBounds
        overload below if you need that. */
    juce::Path renderToPath (const juce::String& svgText);

    /** Returns the SVG as a juce::Path fitted into targetBounds. Unlike the
        single-argument overload, this version uses the view box for the fit,
        so padding and off-centre content are preserved. */
    juce::Path renderToPath (const juce::String& svgText, juce::Rectangle<float> targetBounds,
                             juce::RectanglePlacement placement = juce::RectanglePlacement::centred);

    /** Draws svgText into targetBounds on the given Graphics context. The
        view box is preserved via a centred uniform scale. */
    void render (const juce::String& svgText, juce::Graphics& g,
                 juce::Rectangle<float> targetBounds, Engine engine = Engine::juce);
}

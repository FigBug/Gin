/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Converts SVG text to a rasterized JUCE Image.

    This function parses SVG markup and renders it to a bitmap image at the
    specified dimensions. The SVG will be scaled to fit the given width and height.

    @warning This function may lock the image thread during rendering, which could
             cause delays if called frequently or on the message thread.

    @param svgText  SVG markup as a string
    @param w        Width of the output image in pixels
    @param h        Height of the output image in pixels
    @returns        A JUCE Image containing the rendered SVG
*/
juce::Image rasterizeSVG (juce::String svgText, int w, int h);

/** Parses an SVG path string into a JUCE Path.

    Similar to juce::Drawable::parseSVGPath(), but with extended support for
    parsing lists of points. This allows you to create paths from SVG path
    data strings.

    Example SVG path syntax: "M 10,10 L 100,100 L 100,10 Z"

    @param txt  SVG path data string
    @returns    A JUCE Path representing the parsed SVG path
*/
juce::Path parseSVGPath ( const juce::String& txt );

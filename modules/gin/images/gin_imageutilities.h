/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Converts an SVG to Image.

    WARNING: This will lock the image thread
*/
juce::Image rasterizeSVG (juce::String svgText, int w, int h);

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================*/
/**
    Button that draws an SVG
*/
class SVGButton : public juce::TextButton
{
public:
    SVGButton (const juce::String& name, const juce::String& rawSVG_)
        : juce::TextButton (name), rawSVG (rawSVG_)
    {
    }

    // Handle the drawing in your look and feel

    juce::String rawSVG;
};

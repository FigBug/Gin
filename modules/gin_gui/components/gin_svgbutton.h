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
    SVGButton (const juce::String& name, const juce::String& rawSVG_, int inset_ = 0)
        : juce::TextButton (name), rawSVG (rawSVG_), inset (inset_)
    {
    }

    // Handle the drawing in your look and feel

    juce::String rawSVG;
    int inset = 0;
};

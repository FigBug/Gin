/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/**
    Button that displays an SVG graphic instead of or alongside text.

    SVGButton extends TextButton to support rendering SVG graphics. The SVG
    is stored as raw XML text and can be drawn by a custom LookAndFeel.
    The button provides an inset parameter to control padding around the SVG.

    Note: The actual SVG rendering must be implemented in your LookAndFeel's
    drawButtonBackground() or drawButtonText() methods. This class only stores
    the SVG data and inset value.

    Usage:
    @code
    String svgXml = "<svg>...</svg>";
    SVGButton button("IconButton", svgXml, 4); // 4 pixel inset

    // In your LookAndFeel:
    void drawButtonBackground(Graphics& g, Button& btn, ...) override
    {
        if (auto* svg = dynamic_cast<SVGButton*>(&btn))
        {
            auto drawable = Drawable::createFromSVG(*XmlDocument::parse(svg->rawSVG));
            drawable->drawWithin(g, btn.getLocalBounds().reduced(svg->inset).toFloat(), ...);
        }
    }
    @endcode

    @see juce::TextButton, juce::Drawable
*/
class SVGButton : public juce::TextButton
{
public:
    SVGButton (const juce::String& name, const juce::String& rawSVG_, int inset_ = 0)
        : juce::TextButton (name), rawSVG (rawSVG_), inset (inset_)
    {
    }

    void setColour (juce::Colour c)
    {
        colour = c;
        repaint();
    }

    void clearColour()
    {
        colour.reset();
        repaint();
    }

    std::optional<juce::Colour> getColour() const
    {
        return colour;
    }

    // Handle the drawing in your look and feel

    juce::String rawSVG;
    int inset = 0;
    std::optional<juce::Colour> colour;
};

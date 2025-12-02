#pragma once

/**
    Asset manager for common plugin UI images and icons.

    Images provides pre-loaded juce::Path objects for commonly used icons
    in plugin interfaces. These paths can be used with SVG buttons and
    other components that render vector graphics.

    Available Icons:
    - monoPath: Mono/monophonic mode icon
    - polyPath: Poly/polyphonic mode icon
    - powerPath: Power on/off icon

    Usage:
    @code
    Images images;

    // Use with SVG button
    SVGButton powerBtn("power", images.powerPath);

    // Or draw directly
    void paint(juce::Graphics& g)
    {
        g.fillPath(images.monoPath);
    }
    @endcode

    @see SVGButton, PluginLookAndFeel
*/
class Images
{
public:
    Images();

    juce::Path monoPath, polyPath, powerPath;
};

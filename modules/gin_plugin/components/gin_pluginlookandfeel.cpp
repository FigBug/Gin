#include "pluginlookandfeel.h"
#include "resources.cpp"

//==============================================================================
PluginLookAndFeel::PluginLookAndFeel()
{
    typeface = Typeface::createSystemTypefaceFor (Resources::BarlowThin_ttf, Resources::BarlowThin_ttfSize);
    
    setColour (TriggeredScope::lineColourId, defaultColour (4));
    setColour (TriggeredScope::backgroundColourId, Colours::transparentWhite);
    setColour (TriggeredScope::traceColourId, defaultColour (4));
    
    setColour (XYScope::lineColourId, defaultColour (4));
    setColour (XYScope::backgroundColourId, Colours::transparentWhite);
    setColour (XYScope::traceColourId, defaultColour (4));
}

Typeface::Ptr PluginLookAndFeel::getTypefaceForFont (const Font& font)
{
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName())
        return typeface;
    
    return LookAndFeel_V4::getTypefaceForFont (font);
}

//==============================================================================
PluginLookAndFeelWrapper::PluginLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (this);
}

PluginLookAndFeelWrapper::~PluginLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (nullptr);
}

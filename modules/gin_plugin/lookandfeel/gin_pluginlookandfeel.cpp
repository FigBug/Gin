//==============================================================================
PluginLookAndFeel::PluginLookAndFeel()
{
    typeface = juce::Typeface::createSystemTypefaceFor (Resources::BarlowThin_ttf, Resources::BarlowThin_ttfSize);
    
    setColour (TriggeredScope::lineColourId, defaultColour (4));
    setColour (TriggeredScope::backgroundColourId, juce::Colours::transparentWhite);
    setColour (TriggeredScope::traceColourId, defaultColour (4));
    
    setColour (XYScope::lineColourId, defaultColour (4));
    setColour (XYScope::backgroundColourId, juce::Colours::transparentWhite);
    setColour (XYScope::traceColourId, defaultColour (4));
}

juce::Typeface::Ptr PluginLookAndFeel::getTypefaceForFont (const juce::Font& font)
{
    if (font.getTypefaceName() == juce::Font::getDefaultSansSerifFontName())
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


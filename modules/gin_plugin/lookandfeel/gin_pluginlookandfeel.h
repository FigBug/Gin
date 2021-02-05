#pragma once

//==============================================================================
/** Plugin specific addition to GinLookAndFeel
    Still black and white
*/
class PluginLookAndFeel : public GinLookAndFeel
{
public:
    PluginLookAndFeel();
    
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font& f) override;
        
private:
    juce::Typeface::Ptr typeface;
};

//==============================================================================
/** Auto sets and clears PluginLookAndFeel as default.
    Wrap this up in a SharedResourcePointer
*/
class PluginLookAndFeelWrapper : public PluginLookAndFeel
{
public:
    PluginLookAndFeelWrapper();
    ~PluginLookAndFeelWrapper();
};

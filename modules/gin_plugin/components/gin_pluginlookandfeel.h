#pragma once

//==============================================================================
/** Plugin specific addition to GinLookAndFeel
    Still black and white
*/
class PluginLookAndFeel : public GinLookAndFeel
{
public:
    PluginLookAndFeel();
    
    Typeface::Ptr getTypefaceForFont (const Font& f) override;
        
private:
    Typeface::Ptr typeface;
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

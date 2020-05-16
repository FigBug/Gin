#pragma once

//==============================================================================
class PluginLookAndFeel : public GinLookAndFeel
{
public:
    PluginLookAndFeel();
    
    Typeface::Ptr getTypefaceForFont (const Font& f) override;
        
private:
    Typeface::Ptr typeface;
};

//==============================================================================
class PluginLookAndFeelWrapper : public PluginLookAndFeel
{
public:
    PluginLookAndFeelWrapper();
    ~PluginLookAndFeelWrapper();
};

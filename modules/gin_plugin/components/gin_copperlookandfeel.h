#pragma once

//==============================================================================
/** Plugin specific addition to GinLookAndFeel
    Still black and white
*/
class CopperLookAndFeel : public PluginLookAndFeel
{
public:
    CopperLookAndFeel();
    
    Typeface::Ptr getTypefaceForFont (const Font& f) override;
        
private:
    Typeface::Ptr typeface;
};

//==============================================================================
/** Auto sets and clears CopperLookAndFeel as default.
    Wrap this up in a SharedResourcePointer
*/
class CopperLookAndFeelWrapper : public CopperLookAndFeel
{
public:
    CopperLookAndFeelWrapper();
    ~CopperLookAndFeelWrapper();
};

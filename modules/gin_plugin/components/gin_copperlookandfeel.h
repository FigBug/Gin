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

    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider&) override;

    void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const Slider::SliderStyle, Slider&) override;


    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonText (Graphics&, TextButton&, bool isMouseOverButton, bool isButtonDown) override;

    void drawComboBox (Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox&) override;

    void positionComboBoxText (ComboBox&, Label&) override;

    void drawTextEditorOutline (Graphics&, int width, int height, TextEditor&) override;

    int getAlertWindowButtonHeight() override    { return 20; }

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

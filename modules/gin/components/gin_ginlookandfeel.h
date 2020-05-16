#pragma once

//==============================================================================
class GinLookAndFeel : public LookAndFeel_V4
{
public:
    GinLookAndFeel();
    
    enum ColourIds
    {
        colourId1                  = 0x8700001,
        colourId2                  = 0x8700002,
        colourId3                  = 0x8700003,
        colourId4                  = 0x8700004,
        colourId5                  = 0x8700005,
    };
    
    Colour defaultColour (int idx);

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
};

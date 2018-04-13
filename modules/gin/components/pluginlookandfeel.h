#pragma once

//==============================================================================
class PluginLookAndFeel : public LookAndFeel_V4
{
public:
    PluginLookAndFeel();
    
    void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           Slider&) override;
    
    void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;
    
    void drawButtonText (Graphics&, TextButton&, bool isMouseOverButton, bool isButtonDown) override;

    void drawComboBox (Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       ComboBox&) override;

    void positionComboBoxText (ComboBox&, Label&) override;

};

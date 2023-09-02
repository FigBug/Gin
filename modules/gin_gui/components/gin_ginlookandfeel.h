#pragma once

//==============================================================================
/**
    The GinLookAndFeel. Black background with while text and controls.
    Very minimalist.
*/
class GinLookAndFeel : public juce::LookAndFeel_V4
{
public:
    GinLookAndFeel();

    enum ColourIds
    {
        whiteColourId            = 0x1500000,
        blackColourId            = 0x1500001,
        displayColourId          = 0x1500002,
        backgroundColourId       = 0x1500003,
        groupColourId            = 0x1500004,
        lightGroupColourId       = 0x1500005,
        grey30ColourId           = 0x1500006,
        grey45ColourId           = 0x1500007,
        grey60ColourId           = 0x1500008,
        grey90ColourId           = 0x1500009,
        glass1ColourId           = 0x150000a,
        glass2ColourId           = 0x150000b,
        matte1ColourId           = 0x150000c,
        matte2ColourId           = 0x150000d,
        title1ColourId           = 0x150000e,
        title2ColourId           = 0x150000f,
        accentColourId           = 0x1500010,

        // Obsolete colours
        colourId1                = 0x8700001,
        colourId2                = 0x8700002,
        colourId3                = 0x8700003,
        colourId4                = 0x8700004,
        colourId5                = 0x8700005,
    };

    juce::Colour defaultColour (int idx);

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider&) override;

    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle, juce::Slider&) override;


    void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                               bool isMouseOverButton, bool isButtonDown) override;

    void drawButtonText (juce::Graphics&, juce::TextButton&, bool isMouseOverButton, bool isButtonDown) override;

    void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox&) override;

    void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

    void drawTextEditorOutline (juce::Graphics&, int width, int height, juce::TextEditor&) override;

    int getAlertWindowButtonHeight() override    { return 20; }

    void drawCornerResizer (juce::Graphics& g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/) override;
};

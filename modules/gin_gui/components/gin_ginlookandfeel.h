#pragma once

//==============================================================================
/**
    Minimalist dark look and feel with black background and white controls.

    GinLookAndFeel provides a clean, modern dark theme optimized for audio
    applications and development tools. It features a dark background with
    white/gray controls and text for reduced eye strain and professional appearance.

    Key Features:
    - Dark theme (black background, white text)
    - Minimalist, flat design
    - Customizable color palette via ColourIds
    - Custom drawing for sliders, buttons, combo boxes
    - Glass and matte gradient options
    - Accent color support

    The look and feel can be customized by setting various ColourIds either
    globally via LookAndFeel::setColour() or per-component via Component::setColour().

    Usage:
    @code
    GinLookAndFeel laf;
    LookAndFeel::setDefaultLookAndFeel(&laf);

    // Customize colors
    laf.setColour(GinLookAndFeel::accentColourId, Colours::cyan);
    laf.setColour(GinLookAndFeel::backgroundColourId, Colour(0xff1a1a1a));
    @endcode

    @see juce::LookAndFeel_V4
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

#pragma once

#include "gin_images.h"

//==============================================================================
/** Plugin specific addition to GinLookAndFeel
    Still black and white
*/
class CopperLookAndFeel : public PluginLookAndFeel
{
public:
    CopperLookAndFeel();
    
    juce::Typeface::Ptr getTypefaceForFont (const juce::Font& f) override;

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

    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu (juce::ComboBox& box, juce::Label&) override;

    int getAlertWindowButtonHeight() override       { return 20; }
    juce::Font getLabelFont (juce::Label&) override { return juce::Font (juce::FontOptions (10)); }

    juce::SharedResourcePointer<Images> images;

private:
    juce::Typeface::Ptr typeface;
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

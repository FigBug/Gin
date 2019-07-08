#include "pluginlookandfeel.h"

//==============================================================================
PluginLookAndFeel::PluginLookAndFeel()
{
    setColour (Label::textColourId, Colours::white.withAlpha (0.9f));

    setColour (Slider::trackColourId, Colours::white);
    setColour (Slider::rotarySliderFillColourId, Colours::white);

    setColour (TextButton::buttonColourId, Colours::black);
    setColour (TextButton::buttonOnColourId, Colours::white);
    setColour (TextButton::textColourOffId, Colours::white);
    setColour (TextButton::textColourOnId, Colours::black);

    setColour (ComboBox::backgroundColourId, Colours::transparentWhite);
    setColour (ComboBox::outlineColourId, Colours::white);
    
    setColour (TextEditor::backgroundColourId, Colours::transparentWhite);
    setColour (TextEditor::textColourId, Colours::white);
    setColour (TextEditor::highlightColourId, Colours::white);
    setColour (TextEditor::highlightedTextColourId, Colours::black);
    setColour (TextEditor::outlineColourId, Colours::white);
    setColour (TextEditor::focusedOutlineColourId, Colours::white);
    setColour (TextEditor::shadowColourId, Colours::transparentWhite);

#if JUCE_LINUX
    Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName ("Verdana");
#elif JUCE_WINDOWS
    Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName ("Tahoma");
#else
    Desktop::getInstance().getDefaultLookAndFeel().setDefaultSansSerifTypefaceName ("Geneva");
#endif
}

void PluginLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float minSliderPos, float maxSliderPos,
                                          const Slider::SliderStyle, Slider& slider)
{
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
    auto rc = Rectangle<int> (x, y, width, height);
    rc = rc.withSizeKeepingCentre (width, jmin (height, 4));

    g.setColour (slider.findColour (Slider::trackColourId).withAlpha (0.05f));
    g.fillRect (rc);

    if (slider.isEnabled())
        g.setColour (slider.findColour (Slider::trackColourId).withAlpha (isMouseOver ? 0.95f : 0.9f));

    if (slider.isHorizontal())
        g.fillRect (Rectangle<float> (static_cast<float> (rc.getX()), rc.getY() + 0.5f, sliderPos - rc.getX(), rc.getHeight() - 1.0f));
    else
        g.fillRect (Rectangle<float> (rc.getX() + 0.5f, sliderPos, rc.getWidth() - 1.0f, rc.getY() + (rc.getHeight() - sliderPos)));
}

void PluginLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                          const float rotaryStartAngleIn, const float rotaryEndAngle, Slider& slider)
{
    float rotaryStartAngle = rotaryStartAngleIn;
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    if (radius > 12.0f)
    {
        const float thickness = 0.8f;

        g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (0.05f));

        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
            g.fillPath (filledArc);
        }

        if (slider.isEnabled())
            g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.95f : 0.9f));

        if (slider.getProperties().contains ("fromCentre"))
            rotaryStartAngle = (rotaryStartAngle + rotaryEndAngle) / 2;

        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, angle, thickness);
            g.fillPath (filledArc);
        }

    }
    else
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (Colour (0x80808080));

        Path p;
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType (rw * 0.1f).createStrokedPath (p, p);

        p.addLineSegment (Line<float> (0.0f, 0.0f, 0.0f, -radius), rw * 0.2f);

        g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
    }
}

void PluginLookAndFeel::drawButtonBackground (Graphics& g, Button& b, const Colour&,
                                              bool, bool)
{
    if (b.isMouseOver() && b.isEnabled())
    {
        g.setColour (b.findColour (TextButton::buttonOnColourId).withMultipliedAlpha (0.2f));
        g.fillRect (b.getLocalBounds());
    }
    
    g.setColour (b.findColour (TextButton::buttonOnColourId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f));
    if (b.getToggleState())
        g.fillRect (b.getLocalBounds());
    else
        g.drawRect (b.getLocalBounds());
}

void PluginLookAndFeel::drawButtonText (Graphics& g, TextButton& b, bool, bool)
{
    g.setFont (getTextButtonFont (b, b.getHeight()));

    g.setColour (b.findColour (b.getToggleState() ? TextButton::textColourOnId : TextButton::textColourOffId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f));
    g.drawText (b.getButtonText(), b.getLocalBounds(), Justification::centred);
}

void PluginLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool /*isButtonDown*/,
                                      int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                      ComboBox& box)
{
    const Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRect (boxBounds.toFloat());

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (boxBounds);
}

void PluginLookAndFeel::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 1, box.getHeight() - 1);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (Justification::centred);
}

void PluginLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor&)
{
    g.setColour (Colours::white);
    g.drawRect (0, 0, width, height);
}

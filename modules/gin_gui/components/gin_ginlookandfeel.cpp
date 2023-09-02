
//==============================================================================
GinLookAndFeel::GinLookAndFeel()
{
    setColour (GinLookAndFeel::colourId1, juce::Colour (0xff020202));
    setColour (GinLookAndFeel::colourId2, juce::Colour (0xff393d3f));
    setColour (GinLookAndFeel::colourId3, juce::Colour (0xffc6c5b9));
    setColour (GinLookAndFeel::colourId4, juce::Colour (0xfff4f4f9));
    setColour (GinLookAndFeel::colourId5, juce::Colour (0xfffdfdff));

    setColour (juce::Label::textColourId, defaultColour (4).withAlpha (0.9f));

    setColour (juce::Slider::trackColourId, defaultColour (4));
    setColour (juce::Slider::rotarySliderFillColourId, defaultColour (4));

    setColour (juce::TextButton::buttonColourId, defaultColour (0));
    setColour (juce::TextButton::buttonOnColourId, defaultColour (4));
    setColour (juce::TextButton::textColourOffId, defaultColour (4));
    setColour (juce::TextButton::textColourOnId, defaultColour (0));

    setColour (juce::ComboBox::backgroundColourId, juce::Colours::transparentWhite);
    setColour (juce::ComboBox::outlineColourId, defaultColour (4));

    setColour (juce::ListBox::backgroundColourId, juce::Colours::transparentWhite);

    setColour (juce::TextEditor::backgroundColourId, juce::Colours::transparentWhite);
    setColour (juce::TextEditor::textColourId, defaultColour (4));
    setColour (juce::TextEditor::highlightColourId, defaultColour (4));
    setColour (juce::TextEditor::highlightedTextColourId, defaultColour (0));
    setColour (juce::TextEditor::outlineColourId, defaultColour (4));
    setColour (juce::TextEditor::focusedOutlineColourId, defaultColour (4));
    setColour (juce::TextEditor::shadowColourId, juce::Colours::transparentWhite);
}

juce::Colour GinLookAndFeel::defaultColour (int idx)
{
    switch (idx)
    {
        case 0: return findColour (GinLookAndFeel::colourId1);
        case 1: return findColour (GinLookAndFeel::colourId2);
        case 2: return findColour (GinLookAndFeel::colourId3);
        case 3: return findColour (GinLookAndFeel::colourId4);
        case 4: return findColour (GinLookAndFeel::colourId5);
    }
    return juce::Colours::transparentWhite;
}

void GinLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                       const juce::Slider::SliderStyle, juce::Slider& slider)
{
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
    auto rc = juce::Rectangle<int> (x, y, width, height);
    rc = rc.withSizeKeepingCentre (width, std::min (height, 4));

    g.setColour (slider.findColour (juce::Slider::trackColourId).withAlpha (0.1f));
    g.fillRect (rc);

    if (slider.isEnabled())
        g.setColour (slider.findColour (juce::Slider::trackColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

    auto rcf = rc.toFloat ();

    if (slider.isHorizontal())
        g.fillRect (juce::Rectangle<float> (rcf.getX(), rcf.getY() + 0.5f, sliderPos - rcf.getX(), rcf.getHeight() - 1.0f));
    else
        g.fillRect (juce::Rectangle<float> (rcf.getX() + 0.5f, sliderPos, rcf.getWidth() - 1.0f, rcf.getY() + (rcf.getHeight() - sliderPos)));
}

void GinLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngleIn, const float rotaryEndAngle, juce::Slider& slider)
{
    float rotaryStartAngle = rotaryStartAngleIn;
    const float radius = float ( std::min (width / 2, height / 2) ) - 2.0f;
    const float centreX = float ( x + width ) * 0.5f;
    const float centreY = float ( y + height ) * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    if (radius > 12.0f)
    {
        const float thickness = 0.8f;

        g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId).withAlpha (0.1f));

        {
            juce::Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
            g.fillPath (filledArc);
        }

        if (slider.isEnabled())
            g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

        if (slider.getProperties().contains ("fromCentre"))
            rotaryStartAngle = (rotaryStartAngle + rotaryEndAngle) / 2;

        {
            juce::Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, angle, thickness);
            g.fillPath (filledArc);
        }

    }
    else
    {
        if (slider.isEnabled())
            g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 1.0f : 0.7f));
        else
            g.setColour (juce::Colour (0x80808080));

        juce::Path p;
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        juce::PathStrokeType (rw * 0.1f).createStrokedPath (p, p);

        p.addLineSegment (juce::Line<float> (0.0f, 0.0f, 0.0f, -radius), rw * 0.2f);

        g.fillPath (p, juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    }
}

void GinLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&,
                                           bool, bool)
{
    if (b.isMouseOver() && b.isEnabled())
    {
        g.setColour (b.findColour (juce::TextButton::buttonOnColourId).withMultipliedAlpha (0.2f));
        g.fillRect (b.getLocalBounds());
    }

    g.setColour (b.findColour (juce::TextButton::buttonOnColourId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f));
    if (b.getToggleState())
        g.fillRect (b.getLocalBounds());
    else
        g.drawRect (b.getLocalBounds());
}

void GinLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool)
{
    g.setColour (b.findColour (b.getToggleState() ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f));

    auto text = b.getButtonText();
    if (text.startsWith ("svg:"))
    {
        auto path = parseSVGPath (text.substring (4));
        auto font = getTextButtonFont (b, b.getHeight());

        auto sz = font.getHeight();
        auto rc = b.getLocalBounds().toFloat().withSizeKeepingCentre (sz, sz);
        g.fillPath (path, path.getTransformToScaleToFit (rc, true));
    }
    else
    {
        g.setFont (getTextButtonFont (b, b.getHeight()));
        g.drawText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred);
    }
}

void GinLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   juce::ComboBox& box)
{
    const juce::Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));
    g.fillRect (boxBounds.toFloat());

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRect (boxBounds);
}

void GinLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 1, box.getHeight() - 1);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (juce::Justification::centred);
}

void GinLookAndFeel::drawTextEditorOutline (juce::Graphics& g, int width, int height, juce::TextEditor&)
{
    g.setColour (defaultColour (4));
    g.drawRect (0, 0, width, height);
}

void GinLookAndFeel::drawCornerResizer (juce::Graphics& g, int w, int h, bool /*isMouseOver*/, bool /*isMouseDragging*/)
{
    w /= 2;
    h /= 2;

    auto lineThickness = std::min ((float) w, (float) h) * 0.025f;

    for (float i = 0.0f; i < 1.0f; i += 0.3f)
    {
        g.setColour (juce::Colours::lightgrey);

        g.drawLine ((float) w + w * i,
                    (float) h + h + 1.0f,
                    (float) w + w + 1.0f,
                    (float) h + h * i,
                    lineThickness);

        g.setColour (juce::Colours::darkgrey);

        g.drawLine ((float) w + w * i + lineThickness,
                    (float) h + h + 1.0f,
                    (float) w + w + 1.0f,
                    (float) h + h * i + lineThickness,
                    lineThickness);
    }
}

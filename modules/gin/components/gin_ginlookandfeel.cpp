
//==============================================================================
GinLookAndFeel::GinLookAndFeel()
{
    setColour (GinLookAndFeel::colourId1, Colour (0xff020202));
    setColour (GinLookAndFeel::colourId2, Colour (0xff393d3f));
    setColour (GinLookAndFeel::colourId3, Colour (0xffc6c5b9));
    setColour (GinLookAndFeel::colourId4, Colour (0xfff4f4f9));
    setColour (GinLookAndFeel::colourId5, Colour (0xfffdfdff));

    setColour (Label::textColourId, defaultColour (4).withAlpha (0.9f));

    setColour (Slider::trackColourId, defaultColour (4));
    setColour (Slider::rotarySliderFillColourId, defaultColour (4));

    setColour (TextButton::buttonColourId, defaultColour (0));
    setColour (TextButton::buttonOnColourId, defaultColour (4));
    setColour (TextButton::textColourOffId, defaultColour (4));
    setColour (TextButton::textColourOnId, defaultColour (0));

    setColour (ComboBox::backgroundColourId, Colours::transparentWhite);
    setColour (ComboBox::outlineColourId, defaultColour (4));

    setColour (ListBox::backgroundColourId, Colours::transparentWhite);

    setColour (TextEditor::backgroundColourId, Colours::transparentWhite);
    setColour (TextEditor::textColourId, defaultColour (4));
    setColour (TextEditor::highlightColourId, defaultColour (4));
    setColour (TextEditor::highlightedTextColourId, defaultColour (0));
    setColour (TextEditor::outlineColourId, defaultColour (4));
    setColour (TextEditor::focusedOutlineColourId, defaultColour (4));
    setColour (TextEditor::shadowColourId, Colours::transparentWhite);
}

Colour GinLookAndFeel::defaultColour (int idx)
{
    switch (idx)
    {
        case 0: return findColour (GinLookAndFeel::colourId1);
        case 1: return findColour (GinLookAndFeel::colourId2);
        case 2: return findColour (GinLookAndFeel::colourId3);
        case 3: return findColour (GinLookAndFeel::colourId4);
        case 4: return findColour (GinLookAndFeel::colourId5);
    }
    return Colours::transparentWhite;
}

void GinLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                       const Slider::SliderStyle, Slider& slider)
{
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
    auto rc = Rectangle<int> (x, y, width, height);
    rc = rc.withSizeKeepingCentre (width, jmin (height, 4));

    g.setColour (slider.findColour (Slider::trackColourId).withAlpha (0.1f));
    g.fillRect (rc);

    if (slider.isEnabled())
        g.setColour (slider.findColour (Slider::trackColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

    if (slider.isHorizontal())
        g.fillRect (Rectangle<float> (static_cast<float> (rc.getX()), rc.getY() + 0.5f, sliderPos - rc.getX(), rc.getHeight() - 1.0f));
    else
        g.fillRect (Rectangle<float> (rc.getX() + 0.5f, sliderPos, rc.getWidth() - 1.0f, rc.getY() + (rc.getHeight() - sliderPos)));
}

void GinLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
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

        g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (0.1f));

        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
            g.fillPath (filledArc);
        }

        if (slider.isEnabled())
            g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

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

void GinLookAndFeel::drawButtonBackground (Graphics& g, Button& b, const Colour&,
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

void GinLookAndFeel::drawButtonText (Graphics& g, TextButton& b, bool, bool)
{
    g.setColour (b.findColour (b.getToggleState() ? TextButton::textColourOnId : TextButton::textColourOffId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f));

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
        g.drawText (b.getButtonText(), b.getLocalBounds(), Justification::centred);
    }
}

void GinLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   ComboBox& box)
{
    const Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRect (boxBounds.toFloat());

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (boxBounds);
}

void GinLookAndFeel::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 1, box.getHeight() - 1);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (Justification::centred);
}

void GinLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor&)
{
    g.setColour (defaultColour (4));
    g.drawRect (0, 0, width, height);
}

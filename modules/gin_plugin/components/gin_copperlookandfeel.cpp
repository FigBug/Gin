//==============================================================================
CopperLookAndFeel::CopperLookAndFeel()
{
    typeface = Typeface::createSystemTypefaceFor (Resources::InterRegular_otf, Resources::InterRegular_otfSize);

    setColour (whiteColourId, Colour (0xffFFFFFF));
    setColour (blackColourId, Colour (0xff000000));
    setColour (displayColourId, Colour (0xff0D0E0F));
    setColour (backgroundColourId, Colour (0xff16171A));
    setColour (groupColourId, Colour (0xff28292E));
    setColour (lightGroupColourId, Colour (0xff35373D));
    setColour (grey30ColourId, Colour (0xff585A62));
    setColour (grey45ColourId, Colour (0xff797C84));
    setColour (grey60ColourId, Colour (0xff9B9EA5));
    setColour (grey90ColourId, Colour (0xffE6E6E9));
    setColour (glass1ColourId, Colour (0xff0f1012));
    setColour (glass2ColourId, Colour (0xff0F1012));
    setColour (matte1ColourId, Colour (0xff18191C));
    setColour (matte2ColourId, Colour (0xff131417));
    setColour (title1ColourId, Colour (0xff2A2C30));
    setColour (title2ColourId, Colour (0xff25272B));
    setColour (accentColourId, Colour (0xffCC8866));
}

Typeface::Ptr CopperLookAndFeel::getTypefaceForFont (const Font& font)
{
    if (font.getTypefaceName() == Font::getDefaultSansSerifFontName())
        return typeface;

    return LookAndFeel_V4::getTypefaceForFont (font);
}

void CopperLookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
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
    {
        float t = rc.getY() + 0.5f;
        float h = rc.getHeight() - 1.0f;

        if (slider.getProperties().contains ("fromCentre"))
        {
            auto c = rc.getCentreX();
            if (sliderPos < c)
                g.fillRect (Rectangle<float> (sliderPos, t, c - sliderPos, h));
            else
                g.fillRect (Rectangle<float> (float (c), t, sliderPos - c, h));
        }
        else
        {
            g.fillRect (Rectangle<float> (float (rc.getX()), float (y), sliderPos - rc.getX(), h));
        }
    }
    else
    {
        g.fillRect (Rectangle<float> (rc.getX() + 0.5f, sliderPos, rc.getWidth() - 1.0f, rc.getY() + (rc.getHeight() - sliderPos)));
    }
}

void CopperLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
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

    const float thickness = (radius - 1) / radius;

    g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (0.1f));

    {
        Path filledArc;
        filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
        g.fillPath (filledArc);
    }

    if (slider.isEnabled())
        g.setColour (slider.findColour (Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

    auto fillStartAngle = rotaryStartAngle;
    if (slider.getProperties().contains ("fromCentre"))
        fillStartAngle = (rotaryStartAngle + rotaryEndAngle) / 2;

    {
        Path filledArc;
        filledArc.addPieSegment (rx, ry, rw, rw, fillStartAngle, angle, thickness);
        g.fillPath (filledArc);
    }

    if (slider.getProperties().contains ("modDepth"))
    {
        auto depth = (float)slider.getProperties()["modDepth"];
        bool bipolar = (bool)slider.getProperties()["modBipolar"];

        g.setColour (Colours::red.withAlpha (0.8f));

        Path filledArc;
        if (bipolar)
        {
            auto a = jlimit (rotaryStartAngle, rotaryEndAngle, angle - depth * (rotaryEndAngle - rotaryStartAngle));
            auto b = jlimit (rotaryStartAngle, rotaryEndAngle, angle + depth * (rotaryEndAngle - rotaryStartAngle));
            filledArc.addPieSegment (rx, ry, rw, rw, std::min (a, b), std::max (a, b), thickness);
        }
        else
        {
            auto modPos = jlimit (rotaryStartAngle, rotaryEndAngle, angle + depth * (rotaryEndAngle - rotaryStartAngle));
            filledArc.addPieSegment (rx, ry, rw, rw, angle, modPos, thickness);
        }

        g.fillPath (filledArc);
    }

    if (slider.getProperties().contains ("modValues") && slider.isEnabled())
    {
        g.setColour (Colours::red.withAlpha (0.8f));

        auto varArray = slider.getProperties()["modValues"];
        if (varArray.isArray())
        {
            for (auto value : *varArray.getArray())
            {
                float modAngle = float (value) * (rotaryEndAngle - rotaryStartAngle) + rotaryStartAngle;

                float modX = centreX + std::sin (modAngle) * radius;
                float modY = centreY - std::cos (modAngle) * radius;

                g.fillEllipse (modX - 2, modY - 2, 4.0f, 4.0f);
            }
        }
    }
}

void CopperLookAndFeel::drawButtonBackground (Graphics& g, Button& b, const Colour&,
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

void CopperLookAndFeel::drawButtonText (Graphics& g, TextButton& b, bool, bool)
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

void CopperLookAndFeel::drawComboBox (Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   ComboBox& box)
{
    const Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour (box.findColour (ComboBox::backgroundColourId));
    g.fillRect (boxBounds.toFloat());

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRect (boxBounds);
}

void CopperLookAndFeel::positionComboBoxText (ComboBox& box, Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 1, box.getHeight() - 1);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (Justification::centred);
}

void CopperLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor&)
{
    g.setColour (defaultColour (4));
    g.drawRect (0, 0, width, height);
}


//==============================================================================
CopperLookAndFeelWrapper::CopperLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (this);
}

CopperLookAndFeelWrapper::~CopperLookAndFeelWrapper()
{
    LookAndFeel::setDefaultLookAndFeel (nullptr);
}

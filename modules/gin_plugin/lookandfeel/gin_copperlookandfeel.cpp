//==============================================================================
CopperLookAndFeel::CopperLookAndFeel()
{
    typeface = juce::Typeface::createSystemTypefaceFor (Resources::InterRegular_otf, Resources::InterRegular_otfSize);

    setColour (whiteColourId, juce::Colour (0xffFFFFFF));
    setColour (blackColourId, juce::Colour (0xff000000));
    setColour (displayColourId, juce::Colour (0xff0D0E0F));
    setColour (backgroundColourId, juce::Colour (0xff16171A));
    setColour (groupColourId, juce::Colour (0xff28292E));
    setColour (lightGroupColourId, juce::Colour (0xff35373D));
    setColour (grey30ColourId, juce::Colour (0xff585A62));
    setColour (grey45ColourId, juce::Colour (0xff797C84));
    setColour (grey60ColourId, juce::Colour (0xff9B9EA5));
    setColour (grey90ColourId, juce::Colour (0xffE6E6E9));
    setColour (glass1ColourId, juce::Colour (0xff0f1012));
    setColour (glass2ColourId, juce::Colour (0xff0F1012));
    setColour (matte1ColourId, juce::Colour (0xff18191C));
    setColour (matte2ColourId, juce::Colour (0xff131417));
    setColour (title1ColourId, juce::Colour (0xff2A2C30));
    setColour (title2ColourId, juce::Colour (0xff25272B));
    setColour (accentColourId, juce::Colour (0xffCC8866));

    setColour (juce::TextButton::textColourOnId, findColour (accentColourId));
    setColour (juce::TextButton::textColourOffId, findColour (grey60ColourId));

    setColour (juce::TextEditor::textColourId, findColour (grey60ColourId));

    setColour (juce::ComboBox::textColourId, findColour (accentColourId));
    setColour (juce::ComboBox::backgroundColourId, findColour (glass1ColourId));
    setColour (juce::ComboBox::outlineColourId, findColour (blackColourId));
}

juce::Typeface::Ptr CopperLookAndFeel::getTypefaceForFont (const juce::Font& font)
{
    if (font.getTypefaceName() == juce::Font::getDefaultSansSerifFontName())
        return typeface;

    return LookAndFeel_V4::getTypefaceForFont (font);
}

void CopperLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                       float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                                       const juce::Slider::SliderStyle, juce::Slider& slider)
{
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
    auto rc = juce::Rectangle<int> (x, y, width, height);
    rc = rc.withSizeKeepingCentre (width, juce::jmin (height, 4));

    g.setColour (slider.findColour (juce::Slider::trackColourId).withAlpha (0.1f));
    g.fillRect (rc);

    if (slider.isEnabled())
        g.setColour (slider.findColour (juce::Slider::trackColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

    if (slider.isHorizontal())
    {
        float t = rc.getY() + 0.5f;
        float h = rc.getHeight() - 1.0f;

        if (slider.getProperties().contains ("fromCentre"))
        {
            auto c = rc.getCentreX();
            if (sliderPos < c)
                g.fillRect (juce::Rectangle<float> (sliderPos, t, c - sliderPos, h));
            else
                g.fillRect (juce::Rectangle<float> (float (c), t, sliderPos - c, h));
        }
        else
        {
            g.fillRect (juce::Rectangle<float> (float (rc.getX()), float (y), sliderPos - rc.getX(), h));
        }
    }
    else
    {
        g.fillRect (juce::Rectangle<float> (rc.getX() + 0.5f, sliderPos, rc.getWidth() - 1.0f, rc.getY() + (rc.getHeight() - sliderPos)));
    }
}

void CopperLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngleIn, const float rotaryEndAngle, juce::Slider& slider)
{
    float rotaryStartAngle = rotaryStartAngleIn;
    const float radius = juce::jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    const float thickness = (radius - 1) / radius;

    g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId).withAlpha (0.1f));

    {
        juce::Path filledArc;
        filledArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
        g.fillPath (filledArc);
    }

    if (slider.isEnabled())
        g.setColour (slider.findColour (juce::Slider::rotarySliderFillColourId).withAlpha (isMouseOver ? 0.95f : 0.85f));

    auto fillStartAngle = rotaryStartAngle;
    if (slider.getProperties().contains ("fromCentre"))
        fillStartAngle = (rotaryStartAngle + rotaryEndAngle) / 2;

    {
        juce::Path filledArc;
        filledArc.addPieSegment (rx, ry, rw, rw, fillStartAngle, angle, thickness);
        g.fillPath (filledArc);
    }

    if (slider.getProperties().contains ("modDepth"))
    {
        auto depth = (float)slider.getProperties()["modDepth"];
        bool bipolar = (bool)slider.getProperties()["modBipolar"];

        g.setColour (juce::Colours::red.withAlpha (0.8f));

        juce::Path filledArc;
        if (bipolar)
        {
            auto a = juce::jlimit (rotaryStartAngle, rotaryEndAngle, angle - depth * (rotaryEndAngle - rotaryStartAngle));
            auto b = juce::jlimit (rotaryStartAngle, rotaryEndAngle, angle + depth * (rotaryEndAngle - rotaryStartAngle));
            filledArc.addPieSegment (rx, ry, rw, rw, std::min (a, b), std::max (a, b), thickness);
        }
        else
        {
            auto modPos = juce::jlimit (rotaryStartAngle, rotaryEndAngle, angle + depth * (rotaryEndAngle - rotaryStartAngle));
            filledArc.addPieSegment (rx, ry, rw, rw, angle, modPos, thickness);
        }

        g.fillPath (filledArc);
    }

    if (slider.getProperties().contains ("modValues") && slider.isEnabled())
    {
        g.setColour (juce::Colours::red.withAlpha (0.8f));

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

void CopperLookAndFeel::drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool)
{
}

void CopperLookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool)
{
    auto c = b.findColour (b.getToggleState() ? juce::TextButton::textColourOnId : juce::TextButton::textColourOffId).withMultipliedAlpha (b.isEnabled() ? 1.0f : 0.5f);

    if (b.isMouseOver() && b.isEnabled())
        c = c.brighter();

    g.setColour (c);

    auto text = b.getButtonText();
    if (text.startsWith ("svg:"))
    {
        auto path = parseSVGPath (text.substring (4));
        auto font = getTextButtonFont (b, b.getHeight());

        int sz = std::min (b.getHeight(), b.getWidth());

        auto rc = b.getLocalBounds().toFloat().withSizeKeepingCentre (float (sz), float (sz));
        g.fillPath (path, path.getTransformToScaleToFit (rc, true));
    }
    else
    {
        g.setFont (getTextButtonFont (b, b.getHeight()));
        g.drawText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred);
    }
}

void CopperLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                   int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/,
                                   juce::ComboBox& box)
{
    const juce::Rectangle<int> boxBounds (0, 0, width, height);

    juce::ColourGradient grad (findColour (PluginLookAndFeel::glass1ColourId), 0, 0,
                               findColour (PluginLookAndFeel::glass2ColourId), 0, float (height), false);
    g.setGradientFill (grad);
    g.fillRoundedRectangle (boxBounds.toFloat(), boxBounds.getHeight() / 2.0f);

    g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat(), boxBounds.getHeight() / 2.0f, 1.0f);
}

void CopperLookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (1, 1, box.getWidth() - 1, box.getHeight() - 1);
    label.setFont (getComboBoxFont (box));
    label.setJustificationType (juce::Justification::centred);
}

void CopperLookAndFeel::drawTextEditorOutline (juce::Graphics& g, int width, int height, juce::TextEditor&)
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

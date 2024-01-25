
//==============================================================================
Readout::Readout (Parameter* parameter_)
  : parameter (parameter_)
{
    setEditable (true, true);
    parameter->addListener (this);
    setText (parameter->getUserValueText(), juce::dontSendNotification);

    setBorderSize (juce::BorderSize<int> (0));
}

Readout::~Readout()
{
    parameter->removeListener (this);
}

void Readout::valueUpdated (Parameter*)
{
    setText (parameter->getUserValueText(), juce::dontSendNotification);
}

void Readout::paint (juce::Graphics& g)
{
    if (! isBeingEdited())
        Label::paint (g);
}

juce::TextEditor* Readout::createEditorComponent()
{
    auto e = juce::Label::createEditorComponent();
    e->removeColour (juce::TextEditor::textColourId);
    e->setColour (juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    e->setColour (juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
    e->applyFontToAllText (getLookAndFeel().getLabelFont (*this), true);
    e->setJustification (getJustificationType());
    return e;
}

void Readout::textWasEdited()
{
    float v = getText().getFloatValue();
    parameter->setUserValueAsUserAction (v);

    repaint();
}

//==============================================================================
ParamComponent::ParamComponent (Parameter* parameter_)
  : Component (""), parameter (parameter_)
{
    setName (parameter->getShortName());
}

//==============================================================================
HorizontalFader::HorizontalFader (Parameter* p, bool fromCentre)
  : ParamComponent (p),
    value (parameter),
    fader (parameter, juce::Slider::LinearHorizontal, juce::Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (fader);

    fader.setSkewFactor (parameter->getSkew());
    if (fromCentre)
        fader.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    value.setJustificationType (juce::Justification::centred);
    name.setJustificationType (juce::Justification::centredRight);
}

void HorizontalFader::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced (4);

    name.setBounds (r.removeFromLeft (90));
    value.setBounds (r.removeFromRight (30));
    fader.setBounds (r.reduced (2));
}
//==============================================================================
Switch::Switch (Parameter* p)
  : ParamComponent (p),
    button (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&button);

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    name.setJustificationType (juce::Justification::centred);
}

void Switch::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced (2);

    auto extra = r.getHeight() - r.getWidth();
    auto rc = r.removeFromBottom (std::max (15, extra));

    name.setBounds (rc);
    button.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
}

//==============================================================================
Select::Select (Parameter* p)
  : ParamComponent (p),
    comboBox (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&comboBox);

    comboBox.setScrollWheelEnabled (true);

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    name.setJustificationType (juce::Justification::centred);
}

void Select::resized()
{
    if (showName)
    {
        juce::Rectangle<int> r = getLocalBounds().reduced (2);
        auto rc = r.removeFromBottom (15);

        name.setBounds (rc);
        comboBox.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
    }
    else
    {
        comboBox.setBounds (getLocalBounds());
    }
}

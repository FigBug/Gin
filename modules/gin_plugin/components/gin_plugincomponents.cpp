
//==============================================================================
Readout::Readout (Parameter* parameter_)
  : parameter (parameter_)
{
    parameter->addListener (this);
    setText (parameter->getUserValueText(), juce::dontSendNotification);

    addChildComponent (editor);
    editor.addListener (this);
    editor.setOpaque (false);

    setBorderSize (juce::BorderSize<int> (0));
}

Readout::~Readout()
{
    parameter->removeListener (this);
}

void Readout::parameterChanged (Parameter*)
{
    setText (parameter->getUserValueText(), juce::dontSendNotification);
}

void Readout::paint (juce::Graphics& g)
{
    if (! editing)
        Label::paint (g);
}

void Readout::resized()
{
    editor.setBounds (getLocalBounds());
}

void Readout::mouseDown (const juce::MouseEvent&)
{
    editing = true;
    editor.setVisible (true);
    editor.setFont (getFont());
    editor.setText (parameter->getUserValueText(), juce::dontSendNotification);
    editor.setJustificationType (getJustificationType());
    editor.setSelectAllWhenFocused (true);
    editor.toFront (true);
    repaint();
}

void Readout::sltextEditorReturnKeyPressed (SingleLineTextEditor&)
{
    float v = editor.getText().getFloatValue();
    parameter->setUserValueAsUserAction (v);

    editor.setVisible (false);
    editing = false;
    repaint();
}

void Readout::sltextEditorEscapeKeyPressed (SingleLineTextEditor&)
{
    editor.setVisible (false);
    editing = false;
    repaint();
}

void Readout::sltextEditorFocusLost (SingleLineTextEditor&)
{
    float v = editor.getText().getFloatValue();
    parameter->setUserValueAsUserAction (v);

    editor.setVisible (false);
    editing = false;
    repaint();
}

//==============================================================================
ParamComponent::ParamComponent (Parameter* parameter_)
  : Component (""), parameter (parameter_)
{

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
    auto rc = r.removeFromBottom (15);

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

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    name.setJustificationType (juce::Justification::centred);
}

void Select::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    comboBox.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
}

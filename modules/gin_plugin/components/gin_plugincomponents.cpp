
//==============================================================================
Readout::Readout (Parameter* parameter_)
  : parameter (parameter_)
{
    parameter->addListener (this);
    setText (parameter->getUserValueText(), dontSendNotification);

    addChildComponent (editor);
    editor.addListener (this);
    editor.setOpaque (false);

    setBorderSize (BorderSize<int> (0));
}

Readout::~Readout()
{
    parameter->removeListener (this);
}

void Readout::parameterChanged (Parameter*)
{
    setText (parameter->getUserValueText(), dontSendNotification);
}

void Readout::paint (Graphics& g)
{
    if (! editing)
        Label::paint (g);
}

void Readout::resized()
{
    editor.setBounds (getLocalBounds());
}

void Readout::mouseDown (const MouseEvent&)
{
    editing = true;
    editor.setVisible (true);
    editor.setFont (getFont());
    editor.setText (parameter->getUserValueText(), dontSendNotification);
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
    fader (parameter, Slider::LinearHorizontal, Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (fader);

    fader.setSkewFactor (parameter->getSkew());
    if (fromCentre)
        fader.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), dontSendNotification);
    value.setJustificationType (Justification::centred);
    name.setJustificationType (Justification::centredRight);
}

void HorizontalFader::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (4);

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

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Switch::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
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

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Select::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    comboBox.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
}

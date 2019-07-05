/*
  ==============================================================================

    slComponents.cpp
    Created: 3 Nov 2015 9:33:48am
    Author:  Roland Rabien

  ==============================================================================
*/

#include "../components/plugincomponents.h"

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
Knob::Knob (Parameter* parameter, bool fromCentre)
  : ParamComponent (parameter),
    value (parameter),
    knob (parameter, Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (knob);

    knob.setSkewFactor (parameter->getSkew());
    if (fromCentre)
        knob.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), dontSendNotification);
    value.setJustificationType (Justification::centredTop);
    name.setJustificationType (Justification::centredBottom);

    value.setFont (value.getFont().withHeight (15.0));
}

void Knob::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (4);

    name.setBounds (r.removeFromTop (20));
    value.setBounds (r.removeFromBottom (20));
    knob.setBounds (r.reduced (2));
}
//==============================================================================
HorizontalFader::HorizontalFader (Parameter* parameter, bool fromCentre)
  : ParamComponent (parameter),
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

    name.setFont (value.getFont().withHeight (11.0));
    value.setFont (value.getFont().withHeight (10.0));
}

void HorizontalFader::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (4);

    name.setBounds (r.removeFromLeft (80));
    value.setBounds (r.removeFromRight (30));
    fader.setBounds (r.reduced (2));
}
//==============================================================================
Switch::Switch (Parameter* parameter)
  : ParamComponent (parameter),
    button (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&button);

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Switch::resized()
{
    Rectangle<int> r = getLocalBounds().withSizeKeepingCentre (getWidth() - 10, 20);

    button.setBounds (r);
    name.setBounds (r.translated (0, -20));

    int y = name.getY();
    if (y <= 0)
    {
        button.setTopLeftPosition (button.getX(), button.getY() + -y);
        name.setTopLeftPosition (name.getX(), name.getY() + -y);
    }

}

//==============================================================================
Select::Select (Parameter* parameter)
  : ParamComponent(parameter),
    comboBox (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&comboBox);

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Select::resized()
{
    Rectangle<int> r = getLocalBounds().withSizeKeepingCentre (getWidth() - 10, 20);

    comboBox.setBounds (r);
    name.setBounds (r.translated (0, -20));

    int y = name.getY();
    if (y <= 0)
    {
        comboBox.setTopLeftPosition (comboBox.getX(), comboBox.getY() + -y);
        name.setTopLeftPosition (name.getX(), name.getY() + -y);
    }
}

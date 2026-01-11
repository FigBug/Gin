
//==============================================================================
Fader::Fader (Parameter* p)
  : ParamComponent (p),
    value (parameter),
    fader (parameter)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (fader);

    fader.setTitle (parameter->getName (100));
    fader.setDoubleClickReturnValue (true, parameter->getUserDefaultValue());
    fader.setSkewFactor (parameter->getSkew(), parameter->isSkewSymmetric());
    fader.setName (parameter->getShortName());

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    name.setJustificationType (juce::Justification::centred);

    value.setTitle (parameter->getName (100));
    value.setJustificationType (juce::Justification::centred);
    value.setVisible (false);

    addMouseListener (this, true);
}

void Fader::setDisplayName (const juce::String& n)
{
    name.setText (n, juce::dontSendNotification);
}

void Fader::resized()
{
    auto r = getLocalBounds().reduced (2);
    auto labelHeight = 16;

    auto labelArea = r.removeFromBottom (labelHeight);
    name.setBounds (labelArea);
    value.setBounds (labelArea);

    fader.setBounds (r.reduced (2));
}

void Fader::parentHierarchyChanged()
{
    ParamComponent::parentHierarchyChanged();
    auto a = wantsAccessibleKeyboard (*this);
    name.setWantsKeyboardFocus (a);
    value.setWantsKeyboardFocus (a);
    fader.setWantsKeyboardFocus (a);

    if (wantsAccessibleKeyboard (*this))
    {
        name.setVisible (false);
        value.setVisible (true);
    }
    else
    {
        name.setVisible (true);
        value.setVisible (false);
    }
}

void Fader::mouseEnter (const juce::MouseEvent&)
{
    if (wantsAccessibleKeyboard (*this))
        return;

    if (! isTimerRunning() && isEnabled())
    {
        startTimer (100);
        name.setVisible (false);
        value.setVisible (true);
    }
}

void Fader::timerCallback()
{
    auto p = getMouseXYRelative();
    if (! getLocalBounds().contains (p) &&
        ! juce::ModifierKeys::getCurrentModifiers().isAnyMouseButtonDown() &&
        ! value.isBeingEdited())
    {
        if (wantsAccessibleKeyboard (*this))
        {
            name.setVisible (false);
            value.setVisible (true);
        }
        else
        {
            name.setVisible (true);
            value.setVisible (false);
        }

        stopTimer();
    }
}

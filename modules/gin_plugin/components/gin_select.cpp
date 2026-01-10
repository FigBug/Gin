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

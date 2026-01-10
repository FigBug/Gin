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

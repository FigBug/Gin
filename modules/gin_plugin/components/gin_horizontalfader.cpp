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

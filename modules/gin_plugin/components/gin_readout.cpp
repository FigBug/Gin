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
    if (applyValue (getText()))
    {
        float v = getText().getFloatValue();
        parameter->setUserValueAsUserAction (v);
    }
    repaint();
}

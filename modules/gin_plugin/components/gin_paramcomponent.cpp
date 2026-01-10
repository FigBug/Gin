//==============================================================================
ParamComponent::ParamComponent (Parameter* parameter_)
  : Component (""), parameter (parameter_)
{
    setName (parameter->getShortName());
    addMouseListener (this, true);
}

ParamComponent::~ParamComponent()
{
    removeMouseListener (this);
}

void ParamComponent::mouseUp (const juce::MouseEvent& e)
{
    // Ignore duplicate events within 50ms
    auto now = juce::Time::getCurrentTime();
    if ((now - lastMouseUpTime).inMilliseconds() < 50)
        return;

    lastMouseUpTime = now;

    if (e.mods.isPopupMenu())
        showMidiLearnMenu (e.eventComponent);
}

void ParamComponent::showMidiLearnMenu (juce::Component* targetComponent)
{
    if (! isEnabled())
        return;

    auto midiLearn = parameter->getMidiLearn();
    if (midiLearn == nullptr)
        return;

    juce::PopupMenu menu;
    menu.setLookAndFeel (&getLookAndFeel());

    menu.addSectionHeader ("MIDI Learn");

    const int mappedCC = midiLearn->getMappedCC (parameter);
    const bool isLearning = midiLearn->isLearning() && midiLearn->getLearningParameter() == parameter;

    if (isLearning)
        menu.addItem ("Cancel Learning", [midiLearn] { midiLearn->cancelLearning(); });
    else
        menu.addItem ("Start Learning", [this, midiLearn] { midiLearn->startLearning (parameter); });

    if (mappedCC >= 0)
        menu.addItem ("Clear CC " + juce::String (mappedCC), [this, midiLearn] { midiLearn->clearMapping (parameter); });

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (targetComponent));
}

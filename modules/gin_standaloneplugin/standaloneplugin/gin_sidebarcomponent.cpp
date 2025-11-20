SideBarComponent::SideBarComponent (StandaloneFilterWindow& f)
  : filterWindow (f),
    pluginWrapper (*f.pluginHolder->processor.get())
{
    setLookAndFeel (&lf);

    scope.setNumChannels (pluginWrapper.scopeFifo.getNumChannels());
    addAndMakeVisible (scope);
}

SideBarComponent::~SideBarComponent()
{
}

void SideBarComponent::resized()
{
    auto rc = getLocalBounds().reduced (2);

    scope.setBounds (rc.removeFromBottom (100));
}



SideBarComponent::SideBarComponent (StandaloneFilterWindow& f)
  : filterWindow (f),
    player (f.pluginHolder->player)
{
    setLookAndFeel (&lf);

    {
        auto c1 = juce::Colours::white.overlaidWith (juce::Colours::blue.withAlpha (0.3f));
        auto c2 = juce::Colours::white.overlaidWith (juce::Colours::yellow.withAlpha (0.3f));

        scope.setColour (gin::TriggeredScope::lineColourId, findColour (gin::PluginLookAndFeel::grey45ColourId));

        scope.setColour (gin::TriggeredScope::traceColourId + 0, c1);
        scope.setColour (gin::TriggeredScope::traceColourId + 1, c2);
        scope.setColour (gin::TriggeredScope::envelopeColourId + 0, c1);
        scope.setColour (gin::TriggeredScope::envelopeColourId + 1, c2);

        scope.setNumChannels (player.scopeFifo.getNumChannels());
        addAndMakeVisible (scope);
    }

    {
        auto c1 = juce::Colours::white.overlaidWith (juce::Colours::blue.withAlpha (0.3f));
        auto c2 = juce::Colours::white.overlaidWith (juce::Colours::yellow.withAlpha (0.3f));

        spectrum.setColour (gin::SpectrumAnalyzer::lineColourId, findColour (gin::PluginLookAndFeel::grey45ColourId));

        spectrum.setColour (gin::SpectrumAnalyzer::traceColourId + 0, c1);
        spectrum.setColour (gin::SpectrumAnalyzer::traceColourId + 1, c2);
        spectrum.setColour (gin::SpectrumAnalyzer::envelopeColourId + 0, c1);
        spectrum.setColour (gin::SpectrumAnalyzer::envelopeColourId + 1, c2);

        spectrum.setNumChannels (player.spectrumFifo.getNumChannels());
        addAndMakeVisible (spectrum);
    }
}

SideBarComponent::~SideBarComponent()
{
    setLookAndFeel (nullptr);
}

void SideBarComponent::paint (juce::Graphics& g)
{
    juce::ColourGradient grad (findColour (PluginLookAndFeel::matte1ColourId), 0, 0,
                               findColour (PluginLookAndFeel::matte2ColourId), 0, float (getHeight()), false);

    g.setGradientFill (grad);
    g.fillAll();
}

void SideBarComponent::resized()
{
    auto rc = getLocalBounds().reduced (2);

    scope.setBounds (rc.removeFromBottom (100));
    rc.removeFromBottom (8);
    spectrum.setBounds (rc.removeFromBottom (100));
}



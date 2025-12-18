namespace
{
    const char* playSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M8 5 L19 12 L8 19 Z" fill="currentColor" stroke="currentColor" stroke-width="1" stroke-linejoin="round"/></svg>)";
    const char* stopSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><rect x="6" y="6" width="12" height="12" rx="2" ry="2" fill="currentColor"/></svg>)";
}

SideBarComponent::SideBarComponent (StandaloneFilterWindow& f)
  : filterWindow (f),
    player (f.pluginHolder->player),
    midiPlay ("play", playSVG, 4),
    midiStop ("stop", stopSVG, 4)
{
    setLookAndFeel (&lf);

    if (filterWindow.pluginHolder->processor->acceptsMidi())
    {
        midi.setColour (MidiFileComponent::backgroundColourId, findColour (GinLookAndFeel::displayColourId));
        midi.setColour (MidiFileComponent::noteColourId, findColour (GinLookAndFeel::accentColourId));
        midi.setColour (MidiFileComponent::playheadColourId, findColour (GinLookAndFeel::whiteColourId));
        midi.setColour (MidiFileComponent::outlineColourId, findColour (GinLookAndFeel::grey45ColourId));
        midi.setColour (MidiFileComponent::dropHighlightColourId, findColour (GinLookAndFeel::accentColourId).withAlpha (0.2f));

        midi.onFileDropped = [this]
        {
            updatePlayStopButtons();
            saveMidiFilePath();
        };
        midi.onPlaybackStateChanged = [this] { updatePlayStopButtons(); };

        restoreMidiFilePath();

        addAndMakeVisible (midi);

        midiPlay.onClick = [this]
        {
            player.midiPlayer.play();
            updatePlayStopButtons();
        };
        addAndMakeVisible (midiPlay);

        midiStop.onClick = [this]
        {
            player.midiPlayer.stop();
            player.midiPlayer.setPlayheadPosition (0.0);
            updatePlayStopButtons();
        };
        addAndMakeVisible (midiStop);

        updatePlayStopButtons();
    }

    {
        auto c1 = juce::Colours::white.overlaidWith (juce::Colours::blue.withAlpha (0.3f));
        auto c2 = juce::Colours::white.overlaidWith (juce::Colours::yellow.withAlpha (0.3f));

        scope.setColour (TriggeredScope::lineColourId, findColour (GinLookAndFeel::grey45ColourId));

        scope.setColour (TriggeredScope::traceColourId + 0, c1);
        scope.setColour (TriggeredScope::traceColourId + 1, c2);
        scope.setColour (TriggeredScope::envelopeColourId + 0, c1);
        scope.setColour (TriggeredScope::envelopeColourId + 1, c2);

        scope.setNumChannels (player.scopeFifo.getNumChannels());
        addAndMakeVisible (scope);
    }

    {
        auto c1 = juce::Colours::white.overlaidWith (juce::Colours::blue.withAlpha (0.3f));
        auto c2 = juce::Colours::white.overlaidWith (juce::Colours::yellow.withAlpha (0.3f));

        spectrum.setColour (SpectrumAnalyzer::lineColourId, findColour (GinLookAndFeel::grey45ColourId));

        spectrum.setColour (SpectrumAnalyzer::traceColourId + 0, c1);
        spectrum.setColour (SpectrumAnalyzer::traceColourId + 1, c2);
        spectrum.setColour (SpectrumAnalyzer::envelopeColourId + 0, c1);
        spectrum.setColour (SpectrumAnalyzer::envelopeColourId + 1, c2);

        spectrum.setNumChannels (player.spectrumFifo.getNumChannels());
        addAndMakeVisible (spectrum);
    }
}

SideBarComponent::~SideBarComponent()
{
    setLookAndFeel (nullptr);
}

void SideBarComponent::updatePlayStopButtons()
{
    const bool isPlaying = player.midiPlayer.isPlaying();
    midiPlay.setEnabled (! isPlaying && player.midiPlayer.hasFileLoaded());
    midiStop.setEnabled (isPlaying || player.midiPlayer.getPlayheadPosition() > 0.0);
}

void SideBarComponent::saveMidiFilePath()
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        auto file = player.midiPlayer.getLoadedFile();
        props->setValue ("midiFilePath", file.getFullPathName());
    }
}

void SideBarComponent::restoreMidiFilePath()
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        auto path = props->getValue ("midiFilePath", "");
        if (path.isNotEmpty())
        {
            juce::File file (path);
            if (file.existsAsFile() && player.midiPlayer.load (file))
                updatePlayStopButtons();
        }
    }
}

void SideBarComponent::paint (juce::Graphics& g)
{
    juce::ColourGradient grad (findColour (GinLookAndFeel::matte1ColourId), 0, 0,
                               findColour (GinLookAndFeel::matte2ColourId), 0, float (getHeight()), false);

    g.setGradientFill (grad);
    g.fillAll();
}

void SideBarComponent::resized()
{
    auto rc = getLocalBounds().reduced (2);

    if (midi.isVisible())
    {
        midi.setBounds (rc.removeFromTop (35));
        rc.removeFromTop (2);

        auto buttonArea = rc.removeFromTop (16);

        midiPlay.setBounds (buttonArea.removeFromLeft (16));
        buttonArea.removeFromLeft (2);
        midiStop.setBounds (buttonArea.removeFromLeft (16));
    }

    scope.setBounds (rc.removeFromBottom (100));
    rc.removeFromBottom (8);
    spectrum.setBounds (rc.removeFromBottom (100));
}

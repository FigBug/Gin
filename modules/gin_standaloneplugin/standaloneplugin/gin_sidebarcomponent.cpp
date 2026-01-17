namespace
{
    const char* playSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M8 5 L19 12 L8 19 Z" fill="currentColor" stroke="currentColor" stroke-width="1" stroke-linejoin="round"/></svg>)";
    const char* stopSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><rect x="6" y="6" width="12" height="12" rx="2" ry="2" fill="currentColor"/></svg>)";
    const char* menuSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><rect x="4" y="6" width="16" height="2" rx="1" fill="currentColor"/><rect x="4" y="11" width="16" height="2" rx="1" fill="currentColor"/><rect x="4" y="16" width="16" height="2" rx="1" fill="currentColor"/></svg>)";
    const char* recordSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><circle cx="12" cy="12" r="8" fill="currentColor"/></svg>)";
    // Circle with T cutout (Triggered)
    const char* triggerSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path fill-rule="evenodd" d="M12 4a8 8 0 1 0 0 16 8 8 0 0 0 0-16zM8 8h8v2h-3v7h-2v-7H8V8z" fill="currentColor"/></svg>)";
    // Circle with R cutout (Retrospective)
    const char* retroSVG = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path fill-rule="evenodd" d="M12 4a8 8 0 1 0 0 16 8 8 0 0 0 0-16zM9 7h5a3 3 0 0 1 .5 5.96L17 17h-2.5l-2.3-4H11v4H9V7zm2 2v2h3a1 1 0 1 0 0-2h-3z" fill="currentColor"/></svg>)";
}

SideBarComponent::SideBarComponent (StandaloneFilterWindow& f)
  : filterWindow (f),
    player (f.pluginHolder->player),
    midiPlay ("play", playSVG, 4),
    midiStop ("stop", stopSVG, 4),
    midiMenu ("menu", menuSVG, 4),
    samplePlay ("play", playSVG, 4),
    sampleStop ("stop", stopSVG, 4),
    sampleMenu ("menu", menuSVG, 4),
    recordNormal ("record", recordSVG, 4),
    recordTriggered ("trigger", triggerSVG, 4),
    recordRetrospective ("retro", retroSVG, 4),
    recordStop ("stop", stopSVG, 4),
    recordMenu ("menu", menuSVG, 4)
{
    setLookAndFeel (&lf);

    if (filterWindow.pluginHolder->processor->acceptsMidi())
    {
        addAndMakeVisible (midiHeader);

        midi.setColour (MidiFileComponent::backgroundColourId, findColour (GinLookAndFeel::displayColourId));
        midi.setColour (MidiFileComponent::noteColourId, findColour (GinLookAndFeel::accentColourId));
        midi.setColour (MidiFileComponent::playheadColourId, findColour (GinLookAndFeel::whiteColourId));
        midi.setColour (MidiFileComponent::outlineColourId, findColour (GinLookAndFeel::grey45ColourId));
        midi.setColour (MidiFileComponent::dropHighlightColourId, findColour (GinLookAndFeel::accentColourId).withAlpha (0.2f));

        midi.onFileDropped = [this]
        {
            addToMidiMRU (player.midiPlayer.getLoadedFile());
            updatePlayStopButtons();
            saveMidiFilePath();
        };
        midi.onPlaybackStateChanged = [this] { updatePlayStopButtons(); };

        restoreMidiFilePath();

        addAndMakeVisible (midi);

        midiPlay.setTooltip ("Play MIDI file");
        midiPlay.onClick = [this]
        {
            player.midiPlayer.play();
            updatePlayStopButtons();
        };
        addAndMakeVisible (midiPlay);

        midiStop.setTooltip ("Stop MIDI playback");
        midiStop.onClick = [this]
        {
            player.midiPlayer.stop();
            player.midiPlayer.setPlayheadPosition (0.0);
            updatePlayStopButtons();
        };
        addAndMakeVisible (midiStop);

        midiMenu.setTooltip ("MIDI options");
        midiMenu.onClick = [this] { showMidiMenu(); };
        addAndMakeVisible (midiMenu);

        // Restore loop setting
        if (auto* props = filterWindow.pluginHolder->settings.get())
            player.midiPlayer.setLooping (props->getBoolValue ("midiLoop", true));

        updatePlayStopButtons();
    }

    // Check if processor has audio input
    if (filterWindow.pluginHolder->processor->getTotalNumInputChannels() > 0)
    {
        addAndMakeVisible (sampleHeader);

        sample.setBackgroundColour (findColour (GinLookAndFeel::displayColourId));
        sample.setLineColour (findColour (GinLookAndFeel::accentColourId));

        sample.onFileDropped = [this]
        {
            addToSampleMRU (player.samplePlayer.getLoadedFile());
            updateSamplePlayStopButtons();
            saveSampleFilePath();
        };
        sample.onPlaybackStateChanged = [this] { updateSamplePlayStopButtons(); };

        restoreSampleFilePath();

        addAndMakeVisible (sample);

        samplePlay.setTooltip ("Play audio sample");
        samplePlay.onClick = [this]
        {
            player.samplePlayer.play();
            updateSamplePlayStopButtons();
        };
        addAndMakeVisible (samplePlay);

        sampleStop.setTooltip ("Stop sample playback");
        sampleStop.onClick = [this]
        {
            player.samplePlayer.stop();
            player.samplePlayer.setPosition (0.0);
            updateSamplePlayStopButtons();
        };
        addAndMakeVisible (sampleStop);

        sampleMenu.setTooltip ("Sample options");
        sampleMenu.onClick = [this] { showSampleMenu(); };
        addAndMakeVisible (sampleMenu);

        // Restore loop setting
        if (auto* props = filterWindow.pluginHolder->settings.get())
            player.samplePlayer.setLooping (props->getBoolValue ("sampleLoop", true));

        updateSamplePlayStopButtons();
    }

    {
        addAndMakeVisible (scopeHeader);

        auto accent = findColour (GinLookAndFeel::accentColourId);

        scope.setColour (TriggeredScope::lineColourId, findColour (GinLookAndFeel::grey45ColourId));

        for (int i = 0; i < 8; ++i)
        {
            auto c = accent.withRotatedHue (-0.05f + i * 0.1f);
            scope.setColour (TriggeredScope::traceColourId + i, c);
            scope.setColour (TriggeredScope::envelopeColourId + i, c.withAlpha (0.5f));
        }

        addAndMakeVisible (scope);
    }

    {
        addAndMakeVisible (spectrumHeader);

        auto accent = findColour (GinLookAndFeel::accentColourId);

        spectrum.setColour (SpectrumAnalyzer::lineColourId, findColour (GinLookAndFeel::grey45ColourId));

        for (int i = 0; i < 8; ++i)
        {
            auto c = accent.withRotatedHue (-0.05f + i * 0.1f);
            spectrum.setColour (SpectrumAnalyzer::traceColourId + i, c);
            spectrum.setColour (SpectrumAnalyzer::envelopeColourId + i, c.withAlpha (0.5f));
        }

        addAndMakeVisible (spectrum);
    }

    // Only show XY scope if stereo output
    if (filterWindow.pluginHolder->processor->getTotalNumOutputChannels() >= 2)
    {
        addAndMakeVisible (xyHeader);

        auto accent = findColour (GinLookAndFeel::accentColourId);

        xyScope.setColour (XYScope::lineColourId, findColour (GinLookAndFeel::grey45ColourId));
        xyScope.setColour (XYScope::traceColourId, accent);

        addAndMakeVisible (xyScope);
    }

    {
        addAndMakeVisible (recordHeader);

        recordBuffer.setBackgroundColour (findColour (GinLookAndFeel::displayColourId));
        recordBuffer.setLineColour (findColour (GinLookAndFeel::accentColourId));
        addAndMakeVisible (recordBuffer);

        recordNormal.setTooltip ("Start/stop recording");
        recordNormal.onClick = [this]
        {
            if (player.audioRecorder.isRecording())
                player.audioRecorder.stopRecording();
            else
                player.audioRecorder.startRecording();
            updateRecordButtons();
        };
        addAndMakeVisible (recordNormal);

        recordTriggered.setTooltip ("Triggered recording (starts when audio > 0dB)");
        recordTriggered.onClick = [this]
        {
            if (player.audioRecorder.isRecording() || player.audioRecorder.isWaitingForTrigger())
                player.audioRecorder.stopRecording();
            else
                player.audioRecorder.startTriggeredRecording();
            updateRecordButtons();
        };
        addAndMakeVisible (recordTriggered);

        recordRetrospective.setTooltip ("Save last 30 seconds to file");
        recordRetrospective.onClick = [this]
        {
            player.audioRecorder.saveRetrospective();
        };
        addAndMakeVisible (recordRetrospective);

        recordStop.setTooltip ("Stop recording");
        recordStop.onClick = [this]
        {
            player.audioRecorder.stopRecording();
            updateRecordButtons();
        };
        addAndMakeVisible (recordStop);

        recordMenu.setTooltip ("Recording options");
        recordMenu.onClick = [this] { showRecordMenu(); };
        addAndMakeVisible (recordMenu);

        player.audioRecorder.onRecordingStarted = [this] { updateRecordButtons(); updateRecordButtonColours(); };
        player.audioRecorder.onRecordingStopped = [this] { updateRecordButtons(); updateRecordButtonColours(); };

        startTimerHz (6); // 6 Hz for 3 Hz flash (on/off)

        // Restore destination folder
        if (auto* props = filterWindow.pluginHolder->settings.get())
        {
            auto path = props->getValue ("recordDestFolder", "");
            if (path.isNotEmpty())
            {
                juce::File folder (path);
                if (folder.isDirectory())
                    player.audioRecorder.setDestFolder (folder);
            }
        }

        updateRecordButtons();
    }

    {
        addAndMakeVisible (outputHeader);

        outputGain.setSliderStyle (juce::Slider::LinearHorizontal);
        outputGain.setTextBoxStyle (juce::Slider::TextBoxRight, false, 60, 20);
        outputGain.setRange (-60.0, 12.0, 0.1);
        outputGain.setValue (0.0);
        outputGain.setTextValueSuffix (" dB");
        outputGain.setDoubleClickReturnValue (true, 0.0);
        outputGain.setTooltip ("Output gain");
        outputGain.onValueChange = [this]
        {
            auto dB = outputGain.getValue();
            auto gain = juce::Decibels::decibelsToGain (float (dB));
            player.setOutputGain (gain);

            if (auto* props = filterWindow.pluginHolder->settings.get())
                props->setValue ("outputGain", dB);
        };
        addAndMakeVisible (outputGain);

        // Restore output gain
        if (auto* props = filterWindow.pluginHolder->settings.get())
        {
            auto dB = props->getDoubleValue ("outputGain", 0.0);
            outputGain.setValue (dB, juce::dontSendNotification);
            player.setOutputGain (float (juce::Decibels::decibelsToGain (dB)));
        }
    }

    // Track initial channel counts
    lastInputChannels = filterWindow.pluginHolder->processor->getTotalNumInputChannels();
    lastOutputChannels = filterWindow.pluginHolder->processor->getTotalNumOutputChannels();

    // Listen for audio device changes
    filterWindow.getDeviceManager().addChangeListener (this);
}

SideBarComponent::~SideBarComponent()
{
    filterWindow.getDeviceManager().removeChangeListener (this);

    stopTimer();
    setLookAndFeel (nullptr);
}

void SideBarComponent::changeListenerCallback (juce::ChangeBroadcaster*)
{
    updateComponentVisibility();
}

void SideBarComponent::updateComponentVisibility()
{
    auto* processor = filterWindow.pluginHolder->processor.get();
    int inputChannels = processor->getTotalNumInputChannels();
    int outputChannels = processor->getTotalNumOutputChannels();

    if (inputChannels == lastInputChannels && outputChannels == lastOutputChannels)
        return;

    lastInputChannels = inputChannels;
    lastOutputChannels = outputChannels;

    // Update sample section visibility based on input channels
    bool showSample = inputChannels > 0 && processor->acceptsMidi() == false;
    sampleHeader.setVisible (showSample);
    sample.setVisible (showSample);
    samplePlay.setVisible (showSample);
    sampleStop.setVisible (showSample);
    sampleMenu.setVisible (showSample);

    // Update XY scope visibility based on output channels
    bool showXY = outputChannels >= 2;
    xyHeader.setVisible (showXY);
    xyScope.setVisible (showXY);

    resized();
}

void SideBarComponent::timerCallback()
{
    flashState = ! flashState;
    updateRecordButtonColours();
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

void SideBarComponent::addToMidiMRU (const juce::File& file)
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        juce::StringArray mru;
        mru.addTokens (props->getValue ("midiMRU", ""), "|", "");
        mru.removeEmptyStrings();

        // Remove if already exists
        mru.removeString (file.getFullPathName());

        // Add to front
        mru.insert (0, file.getFullPathName());

        // Keep only 10
        while (mru.size() > 10)
            mru.remove (mru.size() - 1);

        props->setValue ("midiMRU", mru.joinIntoString ("|"));
    }
}

juce::StringArray SideBarComponent::getMidiMRU()
{
    juce::StringArray mru;
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        mru.addTokens (props->getValue ("midiMRU", ""), "|", "");
        mru.removeEmptyStrings();
    }
    return mru;
}

void SideBarComponent::showMidiMenu()
{
    juce::PopupMenu menu;
    menu.setLookAndFeel (&getLookAndFeel());

    const bool isLooping = player.midiPlayer.isLooping();
    menu.addItem ("Loop", true, isLooping, [this, isLooping]
    {
        player.midiPlayer.setLooping (! isLooping);
        if (auto* props = filterWindow.pluginHolder->settings.get())
            props->setValue ("midiLoop", ! isLooping);
    });

    menu.addSeparator();

    menu.addItem ("Load File...", [this]
    {
        auto fc = std::make_shared<juce::FileChooser> ("Load MIDI File",
            juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
            "*.mid;*.midi");

        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fc->launchAsync (flags, [this, fc] (const juce::FileChooser&)
        {
            auto file = fc->getResult();
            if (file.existsAsFile() && player.midiPlayer.load (file))
            {
                addToMidiMRU (file);
                saveMidiFilePath();
                updatePlayStopButtons();
                midi.repaint();
            }
        });
    });

    menu.addItem ("Clear File", player.midiPlayer.hasFileLoaded(), false, [this]
    {
        player.midiPlayer.clear();

        if (auto* props = filterWindow.pluginHolder->settings.get())
            props->setValue ("midiFilePath", "");

        updatePlayStopButtons();
        midi.repaint();
    });

    // Add MRU list
    auto mru = getMidiMRU();
    if (mru.size() > 0)
    {
        menu.addSeparator();

        for (int i = 0; i < mru.size(); ++i)
        {
            juce::File file (mru[i]);
            if (file.existsAsFile())
            {
                menu.addItem (file.getFileName(), [this, file]
                {
                    if (player.midiPlayer.load (file))
                    {
                        addToMidiMRU (file);
                        saveMidiFilePath();
                        updatePlayStopButtons();
                        midi.repaint();
                    }
                });
            }
        }
    }

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (midiMenu));
}

void SideBarComponent::updateSamplePlayStopButtons()
{
    const bool isPlaying = player.samplePlayer.isPlaying();
    samplePlay.setEnabled (! isPlaying && player.samplePlayer.hasFileLoaded());
    sampleStop.setEnabled (isPlaying || player.samplePlayer.getPosition() > 0.0);
}

void SideBarComponent::saveSampleFilePath()
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        auto file = player.samplePlayer.getLoadedFile();
        props->setValue ("sampleFilePath", file.getFullPathName());
    }
}

void SideBarComponent::restoreSampleFilePath()
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        auto path = props->getValue ("sampleFilePath", "");
        if (path.isNotEmpty())
        {
            juce::File file (path);
            if (file.existsAsFile())
            {
                player.samplePlayer.load (file);
                sample.setBuffer (player.samplePlayer.getBuffer());
                updateSamplePlayStopButtons();
            }
        }
    }
}

void SideBarComponent::addToSampleMRU (const juce::File& file)
{
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        juce::StringArray mru;
        mru.addTokens (props->getValue ("sampleMRU", ""), "|", "");
        mru.removeEmptyStrings();

        // Remove if already exists
        mru.removeString (file.getFullPathName());

        // Add to front
        mru.insert (0, file.getFullPathName());

        // Keep only 10
        while (mru.size() > 10)
            mru.remove (mru.size() - 1);

        props->setValue ("sampleMRU", mru.joinIntoString ("|"));
    }
}

juce::StringArray SideBarComponent::getSampleMRU()
{
    juce::StringArray mru;
    if (auto* props = filterWindow.pluginHolder->settings.get())
    {
        mru.addTokens (props->getValue ("sampleMRU", ""), "|", "");
        mru.removeEmptyStrings();
    }
    return mru;
}

void SideBarComponent::showSampleMenu()
{
    juce::PopupMenu menu;
    menu.setLookAndFeel (&getLookAndFeel());

    const bool isLooping = player.samplePlayer.isLooping();
    menu.addItem ("Loop", true, isLooping, [this, isLooping]
    {
        player.samplePlayer.setLooping (! isLooping);
        if (auto* props = filterWindow.pluginHolder->settings.get())
            props->setValue ("sampleLoop", ! isLooping);
    });

    menu.addSeparator();

    menu.addItem ("Load File...", [this]
    {
        auto fc = std::make_shared<juce::FileChooser> ("Load Audio File",
            juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
            "*.wav;*.aiff;*.aif;*.mp3;*.ogg;*.flac");

        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;
        fc->launchAsync (flags, [this, fc] (const juce::FileChooser&)
        {
            auto file = fc->getResult();
            if (file.existsAsFile())
            {
                player.samplePlayer.load (file);
                sample.setBuffer (player.samplePlayer.getBuffer());
                addToSampleMRU (file);
                saveSampleFilePath();
                updateSamplePlayStopButtons();
                sample.repaint();
            }
        });
    });

    menu.addItem ("Clear File", player.samplePlayer.hasFileLoaded(), false, [this]
    {
        player.samplePlayer.clear();
        sample.clear();

        if (auto* props = filterWindow.pluginHolder->settings.get())
            props->setValue ("sampleFilePath", "");

        updateSamplePlayStopButtons();
        sample.repaint();
    });

    // Add MRU list
    auto mru = getSampleMRU();
    if (mru.size() > 0)
    {
        menu.addSeparator();

        for (int i = 0; i < mru.size(); ++i)
        {
            juce::File file (mru[i]);
            if (file.existsAsFile())
            {
                menu.addItem (file.getFileName(), [this, file]
                {
                    player.samplePlayer.load (file);
                    sample.setBuffer (player.samplePlayer.getBuffer());
                    addToSampleMRU (file);
                    saveSampleFilePath();
                    updateSamplePlayStopButtons();
                    sample.repaint();
                });
            }
        }
    }

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (sampleMenu));
}

void SideBarComponent::updateRecordButtons()
{
    const bool isRecording = player.audioRecorder.isRecording();
    const bool isWaiting = player.audioRecorder.isWaitingForTrigger();

    recordNormal.setEnabled (! isWaiting);
    recordTriggered.setEnabled (! isRecording || isWaiting);
    recordRetrospective.setEnabled (! isRecording && ! isWaiting);
    recordStop.setEnabled (isRecording || isWaiting);
}

void SideBarComponent::updateRecordButtonColours()
{
    const auto mode = player.audioRecorder.getRecordMode();
    auto accentColour = findColour (GinLookAndFeel::accentColourId);

    // Normal record button - accent only when recording via normal mode
    if (mode == AudioRecorder::RecordMode::normal)
        recordNormal.setColour (accentColour);
    else
        recordNormal.clearColour();

    // Triggered record button - flash when waiting, solid accent when triggered recording
    if (mode == AudioRecorder::RecordMode::waitingForTrigger)
    {
        if (flashState)
            recordTriggered.setColour (accentColour);
        else
            recordTriggered.clearColour();
    }
    else if (mode == AudioRecorder::RecordMode::triggered)
        recordTriggered.setColour (accentColour);
    else
        recordTriggered.clearColour();
}

void SideBarComponent::showRecordMenu()
{
    juce::PopupMenu menu;
    menu.setLookAndFeel (&getLookAndFeel());

    menu.addItem ("Set Destination Folder...", [this]
    {
        auto fc = std::make_shared<juce::FileChooser> ("Select Destination Folder",
            player.audioRecorder.getDestFolder());

        auto flags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories;
        fc->launchAsync (flags, [this, fc] (const juce::FileChooser&)
        {
            auto folder = fc->getResult();
            if (folder.isDirectory())
            {
                player.audioRecorder.setDestFolder (folder);

                if (auto* props = filterWindow.pluginHolder->settings.get())
                    props->setValue ("recordDestFolder", folder.getFullPathName());
            }
        });
    });

    menu.addSeparator();

    auto destFolder = player.audioRecorder.getDestFolder();
    menu.addItem ("Open Destination Folder", destFolder.isDirectory(), false, [destFolder]
    {
        destFolder.revealToUser();
    });

    menu.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (recordMenu));
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

    // Section heights
    const int midiSectionHeight = 79;      // header (16) + gap (2) + component (35) + gap (2) + buttons (16) + gap (8)
    const int sampleSectionHeight = 79;    // header (16) + gap (2) + component (35) + gap (2) + buttons (16) + gap (8)
    const int recordSectionHeight = 79;    // buttons (16) + gap (2) + component (35) + gap (2) + header (16) + gap (8)
    const int scopeSectionHeight = 126;    // component (100) + gap (2) + header (16) + gap (8)
    const int spectrumSectionHeight = 126; // component (100) + gap (2) + header (16) + gap (8)
    const int xySize = rc.getWidth();
    const int xySectionHeight = xySize + 26; // component (square) + gap (2) + header (16) + gap (8)

    if (midi.isVisible())
    {
        if (rc.getHeight() >= midiSectionHeight)
        {
            midiHeader.setBounds (rc.removeFromTop (16));
            rc.removeFromTop (2);

            midi.setBounds (rc.removeFromTop (35));
            rc.removeFromTop (2);

            auto buttonArea = rc.removeFromTop (16);

            midiPlay.setBounds (buttonArea.removeFromLeft (16));
            buttonArea.removeFromLeft (2);
            midiStop.setBounds (buttonArea.removeFromLeft (16));
            buttonArea.removeFromLeft (2);
            midiMenu.setBounds (buttonArea.removeFromLeft (16));

            rc.removeFromTop (8);
        }
        else
        {
            midiHeader.setBounds ({});
            midi.setBounds ({});
            midiPlay.setBounds ({});
            midiStop.setBounds ({});
            midiMenu.setBounds ({});
        }
    }

    if (sample.isVisible())
    {
        if (rc.getHeight() >= sampleSectionHeight)
        {
            sampleHeader.setBounds (rc.removeFromTop (16));
            rc.removeFromTop (2);

            sample.setBounds (rc.removeFromTop (35));
            rc.removeFromTop (2);

            auto buttonArea = rc.removeFromTop (16);

            samplePlay.setBounds (buttonArea.removeFromLeft (16));
            buttonArea.removeFromLeft (2);
            sampleStop.setBounds (buttonArea.removeFromLeft (16));
            buttonArea.removeFromLeft (2);
            sampleMenu.setBounds (buttonArea.removeFromLeft (16));

            rc.removeFromTop (8);
        }
        else
        {
            sampleHeader.setBounds ({});
            sample.setBounds ({});
            samplePlay.setBounds ({});
            sampleStop.setBounds ({});
            sampleMenu.setBounds ({});
        }
    }

    const int outputSectionHeight = 46;    // slider (20) + gap (2) + header (16) + gap (8)

    if (rc.getHeight() >= outputSectionHeight)
    {
        outputGain.setBounds (rc.removeFromBottom (20));
        rc.removeFromBottom (2);
        outputHeader.setBounds (rc.removeFromBottom (16));
        rc.removeFromBottom (8);
    }
    else
    {
        outputHeader.setBounds ({});
        outputGain.setBounds ({});
    }

    if (rc.getHeight() >= recordSectionHeight)
    {
        auto buttonArea = rc.removeFromBottom (16);

        recordNormal.setBounds (buttonArea.removeFromLeft (16));
        buttonArea.removeFromLeft (2);
        recordTriggered.setBounds (buttonArea.removeFromLeft (16));
        buttonArea.removeFromLeft (2);
        recordRetrospective.setBounds (buttonArea.removeFromLeft (16));
        buttonArea.removeFromLeft (2);
        recordStop.setBounds (buttonArea.removeFromLeft (16));
        buttonArea.removeFromLeft (2);
        recordMenu.setBounds (buttonArea.removeFromLeft (16));

        rc.removeFromBottom (2);
        recordBuffer.setBounds (rc.removeFromBottom (35));
        rc.removeFromBottom (2);
        recordHeader.setBounds (rc.removeFromBottom (16));
        rc.removeFromBottom (8);
    }
    else
    {
        recordHeader.setBounds ({});
        recordBuffer.setBounds ({});
        recordNormal.setBounds ({});
        recordTriggered.setBounds ({});
        recordRetrospective.setBounds ({});
        recordStop.setBounds ({});
        recordMenu.setBounds ({});
    }

    if (rc.getHeight() >= scopeSectionHeight)
    {
        scope.setBounds (rc.removeFromBottom (100));
        rc.removeFromBottom (2);
        scopeHeader.setBounds (rc.removeFromBottom (16));
        rc.removeFromBottom (8);
    }
    else
    {
        scopeHeader.setBounds ({});
        scope.setBounds ({});
    }

    if (rc.getHeight() >= spectrumSectionHeight)
    {
        spectrum.setBounds (rc.removeFromBottom (100));
        rc.removeFromBottom (2);
        spectrumHeader.setBounds (rc.removeFromBottom (16));
        rc.removeFromBottom (8);
    }
    else
    {
        spectrumHeader.setBounds ({});
        spectrum.setBounds ({});
    }

    if (xyScope.isVisible())
    {
        if (rc.getHeight() >= xySectionHeight)
        {
            xyScope.setBounds (rc.removeFromBottom (xySize));
            rc.removeFromBottom (2);
            xyHeader.setBounds (rc.removeFromBottom (16));
            rc.removeFromBottom (8);
        }
        else
        {
            xyHeader.setBounds ({});
            xyScope.setBounds ({});
        }
    }
}

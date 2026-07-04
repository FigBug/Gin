
StandaloneFilterApp::StandaloneFilterApp()
{
	juce::PropertiesFile::Options options;

	options.applicationName     = juce::CharPointer_UTF8 (JucePlugin_Name);
	options.filenameSuffix      = ".settings";
	options.osxLibrarySubFolder = "Application Support";
   #if JUCE_LINUX || JUCE_BSD
	options.folderName          = "~/.config";
   #else
	options.folderName          = "";
   #endif

	appProperties.setStorageParameters (options);
}

StandaloneFilterWindow* StandaloneFilterApp::createWindow()
{
	if (juce::Desktop::getInstance().getDisplays().displays.isEmpty())
	{
		// No displays are available, so no window will be created!
		jassertfalse;
		return nullptr;
	}

	return new StandaloneFilterWindow (getApplicationName(),
									   juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
									   createPluginHolder());
}

std::unique_ptr<StandalonePluginHolder> StandaloneFilterApp::createPluginHolder()
{
	constexpr auto autoOpenMidiDevices =
   #if (JUCE_ANDROID || JUCE_IOS) && ! JUCE_DONT_AUTO_OPEN_MIDI_DEVICES_ON_MOBILE
			true;
   #else
			false;
   #endif


   #ifdef JucePlugin_PreferredChannelConfigurations
	constexpr StandalonePluginHolder::PluginInOuts channels[] { JucePlugin_PreferredChannelConfigurations };
	const juce::Array<StandalonePluginHolder::PluginInOuts> channelConfig (channels, juce::numElementsInArray (channels));
   #else
	const juce::Array<StandalonePluginHolder::PluginInOuts> channelConfig;
   #endif

	return std::make_unique<StandalonePluginHolder> (appProperties.getUserSettings(),
													 false,
                                                     juce::String{},
													 nullptr,
													 channelConfig,
													 autoOpenMidiDevices);
}

void StandaloneFilterApp::initialise (const juce::String& commandLineParameters)
{
	if (commandLineParameters.contains ("--smoketest"))
	{
		auto args = juce::StringArray::fromTokens (commandLineParameters, " ", "\"");
		auto idx = args.indexOf ("--smoketest");
		auto presetDir = (idx + 1 < args.size()) ? args[idx + 1] : juce::String();

		bool ok = runSmokeTest (presetDir);
		setApplicationReturnValue (ok ? 0 : 1);
		quit();
		return;
	}

	mainWindow = rawToUniquePtr (createWindow());

	if (mainWindow != nullptr)
	{
	   #if JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE
		Desktop::getInstance().setKioskModeComponent (mainWindow.get(), false);
	   #endif

		mainWindow->setVisible (true);
	}
	else
	{
		pluginHolder = createPluginHolder();
	}
}

bool StandaloneFilterApp::runSmokeTest (const juce::String& presetDir)
{
	auto dir = juce::File (presetDir);
	if (! dir.isDirectory())
	{
		fprintf (stderr, "Smoketest: preset directory not found: %s\n", presetDir.toRawUTF8());
		return false;
	}

	auto presets = dir.findChildFiles (juce::File::findFiles, false, "*.xml");
	presets.sort();

	if (presets.isEmpty())
	{
		fprintf (stderr, "Smoketest: no .xml presets found in %s\n", presetDir.toRawUTF8());
		return false;
	}

	constexpr double sampleRate = 44100.0;
	constexpr int blockSize = 512;
	constexpr int totalSamples = int (sampleRate * 10);
	constexpr int midiNote = 60;

	int passed = 0;
	int failed = 0;

	for (auto& presetFile : presets)
	{
		auto xml = presetFile.loadFileAsString();
		if (xml.isEmpty())
		{
			fprintf (stderr, "  FAIL (empty file): %s\n", presetFile.getFileName().toRawUTF8());
			failed++;
			continue;
		}

		auto processor = createPluginFilterOfType (juce::AudioProcessor::wrapperType_Standalone);
		processor->setPlayConfigDetails (0, 2, sampleRate, blockSize);
		processor->prepareToPlay (sampleRate, blockSize);

		if (auto* p = dynamic_cast<Processor*> (processor.get()))
			p->setStateXml (xml);

		juce::AudioBuffer<float> buffer (2, blockSize);
		juce::MidiBuffer midi;

		float peakLevel = 0.0f;

		int noteOnSample = int (sampleRate * 0.05);
		int noteOffSample = int (sampleRate * 8.0);

		for (int pos = 0; pos < totalSamples; pos += blockSize)
		{
			buffer.clear();
			midi.clear();

			int samplesThisBlock = std::min (blockSize, totalSamples - pos);

			if (noteOnSample >= pos && noteOnSample < pos + samplesThisBlock)
				midi.addEvent (juce::MidiMessage::noteOn (1, midiNote, 0.8f), noteOnSample - pos);

			if (noteOffSample >= pos && noteOffSample < pos + samplesThisBlock)
				midi.addEvent (juce::MidiMessage::noteOff (1, midiNote, 0.0f), noteOffSample - pos);

			processor->processBlock (buffer, midi);

			peakLevel = std::max (peakLevel, buffer.getMagnitude (0, samplesThisBlock));
		}

		processor->releaseResources();

		bool hasAudio = peakLevel > 0.01f;

		if (hasAudio)
		{
			fprintf (stdout, "  PASS: %s (peak: %.6f)\n", presetFile.getFileName().toRawUTF8(), peakLevel);
			passed++;
		}
		else
		{
			fprintf (stderr, "  FAIL (silent): %s\n", presetFile.getFileName().toRawUTF8());
			failed++;
		}
	}

	fprintf (stdout, "\nSmoketest results: %d passed, %d failed out of %d presets.\n", passed, failed, presets.size());
	return failed == 0;
}

void StandaloneFilterApp::shutdown()
{
	pluginHolder = nullptr;
	mainWindow = nullptr;
	appProperties.saveIfNeeded();
}

void StandaloneFilterApp::systemRequestedQuit() 
{
	if (pluginHolder != nullptr)
		pluginHolder->savePluginState();

	if (mainWindow != nullptr)
		mainWindow->pluginHolder->savePluginState();

	if (juce::ModalComponentManager::getInstance()->cancelAllModalComponents())
	{
        juce::Timer::callAfterDelay (100, []()
		{
			if (auto app = JUCEApplicationBase::getInstance())
				app->systemRequestedQuit();
		});
	}
	else
	{
		quit();
	}
}

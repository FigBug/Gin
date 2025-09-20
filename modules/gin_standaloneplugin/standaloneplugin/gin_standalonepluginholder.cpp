StandalonePluginHolder::StandalonePluginHolder (juce::PropertySet* settingsToUse,
												bool takeOwnershipOfSettings,
												const juce::String& preferredDefaultDeviceName,
												const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions,
												const juce::Array<PluginInOuts>& channels,
											   #if JUCE_ANDROID || JUCE_IOS
												bool shouldAutoOpenMidiDevices
											   #else
												bool shouldAutoOpenMidiDevices
											   #endif
												)

	: settings (settingsToUse, takeOwnershipOfSettings),
	  channelConfiguration (channels),
	  autoOpenMidiDevices (shouldAutoOpenMidiDevices)
{
	// Only one StandalonePluginHolder may be created at a time
	jassert (currentInstance == nullptr);
	currentInstance = this;

	shouldMuteInput.addListener (this);
	shouldMuteInput = ! isInterAppAudioConnected();

	handleCreatePlugin();

	auto inChannels = (channelConfiguration.size() > 0 ? channelConfiguration[0].numIns
													   : processor->getMainBusNumInputChannels());

	if (preferredSetupOptions != nullptr)
		options.reset (new juce::AudioDeviceManager::AudioDeviceSetup (*preferredSetupOptions));

	auto audioInputRequired = (inChannels > 0);

	if (audioInputRequired && juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
		&& ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
		juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
									 [this, preferredDefaultDeviceName] (bool granted) { init (granted, preferredDefaultDeviceName); });
	else
		init (audioInputRequired, preferredDefaultDeviceName);
}

void StandalonePluginHolder::init (bool enableAudioInput, const juce::String& preferredDefaultDeviceName)
{
	setupAudioDevices (enableAudioInput, preferredDefaultDeviceName, options.get());
	reloadPluginState();
	startPlaying();

   if (autoOpenMidiDevices)
	   startTimer (500);
}

StandalonePluginHolder::~StandalonePluginHolder()
{
	stopTimer();

	handleDeletePlugin();
	shutDownAudioDevices();

	currentInstance = nullptr;
}

//==============================================================================
void StandalonePluginHolder::createPlugin()
{
	handleCreatePlugin();
}

void StandalonePluginHolder::deletePlugin()
{
	handleDeletePlugin();
}

int StandalonePluginHolder::getNumInputChannels() const
{
	if (processor == nullptr)
		return 0;

	return (channelConfiguration.size() > 0 ? channelConfiguration[0].numIns
											: processor->getMainBusNumInputChannels());
}

int StandalonePluginHolder::getNumOutputChannels() const
{
	if (processor == nullptr)
		return 0;

	return (channelConfiguration.size() > 0 ? channelConfiguration[0].numOuts
											: processor->getMainBusNumOutputChannels());
}

juce::String StandalonePluginHolder::getFilePatterns (const juce::String& fileSuffix)
{
	if (fileSuffix.isEmpty())
		return {};

	return (fileSuffix.startsWithChar ('.') ? "*" : "*.") + fileSuffix;
}

//==============================================================================
juce::File StandalonePluginHolder::getLastFile() const
{
	juce::File f;

	if (settings != nullptr)
		f = juce::File (settings->getValue ("lastStateFile"));

	if (f == juce::File())
		f = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory);

	return f;
}

void StandalonePluginHolder::setLastFile (const juce::FileChooser& fc)
{
	if (settings != nullptr)
		settings->setValue ("lastStateFile", fc.getResult().getFullPathName());
}

void StandalonePluginHolder::askUserToSaveState (const juce::String& fileSuffix)
{
	stateFileChooser = std::make_unique<juce::FileChooser> (TRANS ("Save current state"),
															getLastFile(),
															getFilePatterns (fileSuffix));
	auto flags = juce::FileBrowserComponent::saveMode
			   | juce::FileBrowserComponent::canSelectFiles
			   | juce::FileBrowserComponent::warnAboutOverwriting;

	stateFileChooser->launchAsync (flags, [this] (const juce::FileChooser& fc)
	{
		if (fc.getResult() == juce::File{})
			return;

		setLastFile (fc);

		juce::MemoryBlock data;
		processor->getStateInformation (data);

		if (! fc.getResult().replaceWithData (data.getData(), data.getSize()))
		{
			auto opts = juce::MessageBoxOptions::makeOptionsOk (juce::AlertWindow::WarningIcon,
																TRANS ("Error whilst saving"),
																TRANS ("Couldn't write to the specified file!"));
			messageBox = juce::AlertWindow::showScopedAsync (opts, nullptr);
		}
	});
}

void StandalonePluginHolder::askUserToLoadState (const juce::String& fileSuffix)
{
	stateFileChooser = std::make_unique<juce::FileChooser> (TRANS ("Load a saved state"),
															getLastFile(),
															getFilePatterns (fileSuffix));
	auto flags = juce::FileBrowserComponent::openMode
			   | juce::FileBrowserComponent::canSelectFiles;

	stateFileChooser->launchAsync (flags, [this] (const juce::FileChooser& fc)
	{
		if (fc.getResult() == juce::File{})
			return;

		setLastFile (fc);

		juce::MemoryBlock data;

		if (fc.getResult().loadFileAsData (data))
		{
			processor->setStateInformation (data.getData(), (int) data.getSize());
		}
		else
		{
			auto opts = juce::MessageBoxOptions::makeOptionsOk (juce::AlertWindow::WarningIcon,
																TRANS ("Error whilst loading"),
																TRANS ("Couldn't read from the specified file!"));
			messageBox = juce::AlertWindow::showScopedAsync (opts, nullptr);
		}
	});
}

//==============================================================================
void StandalonePluginHolder::startPlaying()
{
	player.setProcessor (processor.get());

   #if JucePlugin_Enable_IAA && JUCE_IOS
	if (auto device = dynamic_cast<iOSAudioIODevice*> (deviceManager.getCurrentAudioDevice()))
	{
		processor->setPlayHead (device->getAudioPlayHead());
		device->setMidiMessageCollector (&player.getMidiMessageCollector());
	}
   #endif
}

void StandalonePluginHolder::stopPlaying()
{
	player.setProcessor (nullptr);
}

//==============================================================================
void StandalonePluginHolder::showAudioSettingsDialog()
{
	juce::DialogWindow::LaunchOptions o;

	int maxNumInputs = 0, maxNumOutputs = 0;

	if (channelConfiguration.size() > 0)
	{
		auto& defaultConfig = channelConfiguration.getReference (0);

		maxNumInputs  = juce::jmax (0, (int) defaultConfig.numIns);
		maxNumOutputs = juce::jmax (0, (int) defaultConfig.numOuts);
	}

	if (auto* bus = processor->getBus (true, 0))
		maxNumInputs = juce::jmax (0, bus->getDefaultLayout().size());

	if (auto* bus = processor->getBus (false, 0))
		maxNumOutputs = juce::jmax (0, bus->getDefaultLayout().size());

	auto content = std::make_unique<SettingsComponent> (*this, deviceManager, maxNumInputs, maxNumOutputs);
	content->setSize (500, 550);
	content->setToRecommendedSize();

	o.content.setOwned (content.release());

	o.dialogTitle                   = TRANS ("Audio/MIDI Settings");
	o.dialogBackgroundColour        = o.content->getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId);
	o.escapeKeyTriggersCloseButton  = true;
	o.useNativeTitleBar             = true;
	o.resizable                     = false;

	o.launchAsync();
}

void StandalonePluginHolder::saveAudioDeviceState()
{
	if (settings != nullptr)
	{
		auto xml = deviceManager.createStateXml();

		settings->setValue ("audioSetup", xml.get());

	   #if ! (JUCE_IOS || JUCE_ANDROID)
		settings->setValue ("shouldMuteInput", (bool) shouldMuteInput.getValue());
	   #endif
	}
}

void StandalonePluginHolder::reloadAudioDeviceState (bool enableAudioInput,
													 const juce::String& preferredDefaultDeviceName,
													 const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions)
{
	std::unique_ptr<juce::XmlElement> savedState;

	if (settings != nullptr)
	{
		savedState = settings->getXmlValue ("audioSetup");

	   #if ! (JUCE_IOS || JUCE_ANDROID)
		shouldMuteInput.setValue (settings->getBoolValue ("shouldMuteInput", true));
	   #endif
	}

	auto inputChannels  = getNumInputChannels();
	auto outputChannels = getNumOutputChannels();

	if (inputChannels == 0 && outputChannels == 0 && processor->isMidiEffect())
	{
		// add a dummy output channel for MIDI effect plug-ins so they can receive audio callbacks
		outputChannels = 1;
	}

	deviceManager.initialise (enableAudioInput ? inputChannels : 0,
							  outputChannels,
							  savedState.get(),
							  true,
							  preferredDefaultDeviceName,
							  preferredSetupOptions);
}

//==============================================================================
void StandalonePluginHolder::savePluginState()
{
	if (settings != nullptr && settings->getBoolValue ("autoLoadSave", true) && processor != nullptr)
	{
		juce::MemoryBlock data;
		processor->getStateInformation (data);

		settings->setValue ("filterState", data.toBase64Encoding());
	}
}

void StandalonePluginHolder::reloadPluginState()
{
	if (settings != nullptr && settings->getBoolValue ("autoLoadSave", true))
	{
		juce::MemoryBlock data;

		if (data.fromBase64Encoding (settings->getValue ("filterState")) && data.getSize() > 0)
			processor->setStateInformation (data.getData(), (int) data.getSize());
	}
}

//==============================================================================
void StandalonePluginHolder::switchToHostApplication()
{
   #if JUCE_IOS
	if (auto device = dynamic_cast<iOSAudioIODevice*> (deviceManager.getCurrentAudioDevice()))
		device->switchApplication();
   #endif
}

bool StandalonePluginHolder::isInterAppAudioConnected()
{
   #if JUCE_IOS
	if (auto device = dynamic_cast<iOSAudioIODevice*> (deviceManager.getCurrentAudioDevice()))
		return device->isInterAppAudioConnected();
   #endif

	return false;
}

juce::Image StandalonePluginHolder::getIAAHostIcon ([[maybe_unused]] int size)
{
   #if JUCE_IOS && JucePlugin_Enable_IAA
	if (auto device = dynamic_cast<iOSAudioIODevice*> (deviceManager.getCurrentAudioDevice()))
		return device->getIcon (size);
   #else
   #endif

	return {};
}

StandalonePluginHolder* StandalonePluginHolder::getInstance()
{
	return currentInstance;
}

//==============================================================================
void StandalonePluginHolder::handleCreatePlugin()
{
	processor = createPluginFilterOfType (juce::AudioProcessor::wrapperType_Standalone);
	processor->disableNonMainBuses();
	processor->setRateAndBufferSizeDetails (44100, 512);

	processorHasPotentialFeedbackLoop = (getNumInputChannels() > 0 && getNumOutputChannels() > 0);
}

void StandalonePluginHolder::handleDeletePlugin()
{
	stopPlaying();
	processor = nullptr;
}

//==============================================================================
void StandalonePluginHolder::audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
															   int numInputChannels,
															   float* const* outputChannelData,
															   int numOutputChannels,
															   int numSamples,
															   const juce::AudioIODeviceCallbackContext& context)
{
	if (muteInput)
	{
		emptyBuffer.clear();
		inputChannelData = emptyBuffer.getArrayOfReadPointers();
	}

	player.audioDeviceIOCallbackWithContext (inputChannelData,
											 numInputChannels,
											 outputChannelData,
											 numOutputChannels,
											 numSamples,
											 context);
}

void StandalonePluginHolder::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
	emptyBuffer.setSize (device->getActiveInputChannels().countNumberOfSetBits(), device->getCurrentBufferSizeSamples());
	emptyBuffer.clear();

	player.audioDeviceAboutToStart (device);
	player.setMidiOutput (deviceManager.getDefaultMidiOutput());
}

void StandalonePluginHolder::audioDeviceStopped()
{
	player.setMidiOutput (nullptr);
	player.audioDeviceStopped();
	emptyBuffer.setSize (0, 0);
}

//==============================================================================
void StandalonePluginHolder::setupAudioDevices (bool enableAudioInput,
												const juce::String& preferredDefaultDeviceName,
												const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions)
{
	deviceManager.addAudioCallback (this);
	deviceManager.addMidiInputDeviceCallback ({}, &player);

	reloadAudioDeviceState (enableAudioInput, preferredDefaultDeviceName, preferredSetupOptions);
}

void StandalonePluginHolder::shutDownAudioDevices()
{
	saveAudioDeviceState();

	deviceManager.removeMidiInputDeviceCallback ({}, &player);
	deviceManager.removeAudioCallback (this);
}

void StandalonePluginHolder::timerCallback()
{
	auto newMidiDevices = juce::MidiInput::getAvailableDevices();

	if (newMidiDevices != lastMidiDevices)
	{
		for (auto& oldDevice : lastMidiDevices)
			if (! newMidiDevices.contains (oldDevice))
				deviceManager.setMidiInputDeviceEnabled (oldDevice.identifier, false);

		for (auto& newDevice : newMidiDevices)
			if (! lastMidiDevices.contains (newDevice))
				deviceManager.setMidiInputDeviceEnabled (newDevice.identifier, true);

		lastMidiDevices = newMidiDevices;
	}
}

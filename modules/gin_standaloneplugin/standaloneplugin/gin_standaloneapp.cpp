
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

void StandaloneFilterApp::initialise (const juce::String&)
{
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

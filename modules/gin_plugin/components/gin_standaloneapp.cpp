#ifdef JucePlugin_Name

StandaloneApp::StandaloneApp (std::function<bool (juce::PropertiesFile&)> init)
     : onInitialise (init)
{
    juce::PluginHostType::jucePlugInClientCurrentWrapperType = juce::AudioProcessor::wrapperType_Standalone;

    juce::PropertiesFile::Options options;

    options.applicationName     = getApplicationName();
    options.filenameSuffix      = ".settings";
    options.osxLibrarySubFolder = "Application Support";
   #if JUCE_LINUX
    options.folderName          = "~/.config";
   #else
    options.folderName          = "";
   #endif

    appProperties.setStorageParameters (options);
}

const juce::String StandaloneApp::getApplicationName()
{
   #ifdef JucePlugin_Name
    return JucePlugin_Name;
   #else
    return {};
   #endif
}

const juce::String StandaloneApp::getApplicationVersion()
{
   #ifdef JucePlugin_VersionString
    return JucePlugin_VersionString;
   #else
    return {};
   #endif
}

bool StandaloneApp::moreThanOneInstanceAllowed()
{
    return true;
}

void StandaloneApp::anotherInstanceStarted (const juce::String&)
{
}

void StandaloneApp::initialise (const juce::String& /*commandLineParameters*/ )
{
    if (onInitialise (*appProperties.getUserSettings()))
    {
        mainWindow.reset (createWindow());
        mainWindow->setVisible (true);
        return;
    }
    quit();
}

void StandaloneApp::shutdown ()
{
    mainWindow = nullptr;
    appProperties.saveIfNeeded ();
}

void StandaloneApp::systemRequestedQuit ()
{
    if (juce::ModalComponentManager::getInstance ()->cancelAllModalComponents())
    {
        juce::Timer::callAfterDelay (100, []
        {
            if (auto app = JUCEApplicationBase::getInstance())
                app->systemRequestedQuit ();
        } );
    }
    else
    {
        quit();
    }
}

juce::StandaloneFilterWindow* StandaloneApp::createWindow ()
{
   #ifdef JucePlugin_PreferredChannelConfigurations
    juce::StandalonePluginHolder::PluginInOuts channels[] = { JucePlugin_PreferredChannelConfigurations };
   #endif

    return new juce::StandaloneFilterWindow (getApplicationName(),
                                             juce::LookAndFeel::getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
                                             appProperties.getUserSettings(),
                                             false, {}, nullptr
                                            #ifdef JucePlugin_PreferredChannelConfigurations
                                             , juce::Array<juce::StandalonePluginHolder::PluginInOuts> (channels, juce::numElementsInArray (channels))
                                            #endif
    );
}

#endif

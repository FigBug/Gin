
StandaloneFilterWindow::StandaloneFilterWindow (const juce::String& title,
                                                juce::Colour backgroundColour,
                                                std::unique_ptr<StandalonePluginHolder> pluginHolderIn)
    : DocumentWindow (title, backgroundColour, DocumentWindow::minimiseButton | DocumentWindow::closeButton),
      pluginHolder (std::move (pluginHolderIn)),
      optionsButton ("Options")
{
    setConstrainer (&decoratorConstrainer);

   #if JUCE_IOS || JUCE_ANDROID
    setTitleBarHeight (0);
   #else
    setTitleBarButtonsRequired (DocumentWindow::minimiseButton | DocumentWindow::closeButton, false);

    Component::addAndMakeVisible (optionsButton);
    optionsButton.addListener (this);
    optionsButton.setTriggeredOnMouseDown (true);
   #endif

   #if JUCE_IOS || JUCE_ANDROID
    setFullScreen (true);
    updateContent();
   #else
    updateContent();

    const auto windowScreenBounds = [this]() -> juce::Rectangle<int>
    {
        const auto width = getWidth();
        const auto height = getHeight();

        const auto& displays = juce::Desktop::getInstance().getDisplays();

        if (displays.displays.isEmpty())
            return { width, height };

        if (auto* props = pluginHolder->settings.get())
        {
            constexpr int defaultValue = -100;

            const auto x = props->getIntValue ("windowX", defaultValue);
            const auto y = props->getIntValue ("windowY", defaultValue);

            if (x != defaultValue && y != defaultValue)
            {
                const auto screenLimits = displays.getDisplayForRect ({ x, y, width, height })->userArea;

                return { juce::jlimit (screenLimits.getX(), juce::jmax (screenLimits.getX(), screenLimits.getRight()  - width),  x),
                         juce::jlimit (screenLimits.getY(), juce::jmax (screenLimits.getY(), screenLimits.getBottom() - height), y),
                         width, height };
            }
        }

        const auto displayArea = displays.getPrimaryDisplay()->userArea;

        return { displayArea.getCentreX() - width / 2,
                 displayArea.getCentreY() - height / 2,
                 width, height };
    }();

    setBoundsConstrained (windowScreenBounds);

    if (auto* processor = getAudioProcessor())
        if (auto* editor = processor->getActiveEditor())
            setResizable (editor->isResizable(), false);
   #endif
}

//==============================================================================
/** Creates a window with a given title and colour.
    The settings object can be a PropertySet that the class should use to
    store its settings (it can also be null). If takeOwnershipOfSettings is
    true, then the settings object will be owned and deleted by this object.
*/
StandaloneFilterWindow::StandaloneFilterWindow (const juce::String& title,
                                                juce::Colour backgroundColour,
                                                juce::PropertySet* settingsToUse,
                                                bool takeOwnershipOfSettings,
                                                const juce::String& preferredDefaultDeviceName,
                                                const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions,
                                                const juce::Array<PluginInOuts>& constrainToConfiguration,
                                               #if JUCE_ANDROID || JUCE_IOS
                                                bool autoOpenMidiDevices
                                               #else
                                                bool autoOpenMidiDevices
                                               #endif
                                                )
    : StandaloneFilterWindow (title,
                              backgroundColour,
                              std::make_unique<StandalonePluginHolder> (settingsToUse,
                                                                        takeOwnershipOfSettings,
                                                                        preferredDefaultDeviceName,
                                                                        preferredSetupOptions,
                                                                        constrainToConfiguration,
                                                                        autoOpenMidiDevices))
{
}

StandaloneFilterWindow::~StandaloneFilterWindow()
{
   #if (! JUCE_IOS) && (! JUCE_ANDROID)
    if (auto* props = pluginHolder->settings.get())
    {
        props->setValue ("windowX", getX());
        props->setValue ("windowY", getY());
    }
   #endif

    pluginHolder->stopPlaying();
    clearContentComponent();
    pluginHolder = nullptr;
}

void StandaloneFilterWindow::resetToDefaultState()
{
    pluginHolder->stopPlaying();
    clearContentComponent();
    pluginHolder->deletePlugin();

    if (auto* props = pluginHolder->settings.get())
        props->removeValue ("filterState");

    pluginHolder->createPlugin();
    updateContent();
    pluginHolder->startPlaying();
}

void StandaloneFilterWindow::reopenEditor()
{
    clearContentComponent();
    updateContent();
}

//==============================================================================
void StandaloneFilterWindow::closeButtonPressed()
{
    pluginHolder->savePluginState();

    juce::JUCEApplicationBase::quit();
}

void StandaloneFilterWindow::resized()
{
    DocumentWindow::resized();
    optionsButton.setBounds (8, 6, 60, getTitleBarHeight() - 8);
}

void StandaloneFilterWindow::updateContent()
{
    auto* content = new MainContentComponent (*this);
    decoratorConstrainer.setMainContentComponent (content);

   #if JUCE_IOS || JUCE_ANDROID
    constexpr auto resizeAutomatically = false;
   #else
    constexpr auto resizeAutomatically = true;
   #endif

    setContentOwned (content, resizeAutomatically);
}

void StandaloneFilterWindow::buttonClicked (juce::Button* button)
{
    juce::PopupMenu m;

    m.addItem (TRANS("Audio/MIDI Settings..."), [this] { pluginHolder->showAudioSettingsDialog(); });
    m.addSeparator();
    m.addItem (TRANS("Save current state..."), [this] { pluginHolder->askUserToSaveState(); });
    m.addItem (TRANS("Load a saved state..."), [this] { pluginHolder->askUserToLoadState(); });


    if (auto settings = pluginHolder->settings.get())
    {
        bool autoLoadSave = settings->getBoolValue ("autoLoadSave", true);
        m.addItem (TRANS("Automatically load and save state"), true, autoLoadSave, [settings, autoLoadSave] { settings->setValue ("autoLoadSave", ! autoLoadSave); });
    }

    m.addSeparator();
    m.addItem (TRANS("Close and reopen editor"), [this] { reopenEditor(); });
    m.addItem (TRANS("Show sidebar"), true, bool (pluginHolder->getShowSidebarValue().getValue()), [this]
    {
        auto& v = pluginHolder->getShowSidebarValue();
        v = ! bool (v.getValue());
    });

    m.addSeparator();
    m.addItem (TRANS("Reset to default state"), [this] { resetToDefaultState(); });


    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (button));
}




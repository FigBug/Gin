#pragma once

//==============================================================================
/**
    An object that creates and plays a standalone instance of an AudioProcessor.

    The object will create your processor using the same createPluginFilter()
    function that the other plugin wrappers use, and will run it through the
    computer's audio/MIDI devices using AudioDeviceManager and AudioProcessorPlayer.

    @tags{Audio}
*/
class StandalonePluginHolder : private juce::AudioIODeviceCallback,
                               private juce::Timer,
                               private juce::Value::Listener
{
public:
    //==============================================================================
    /** Structure used for the number of inputs and outputs. */
    struct PluginInOuts   { short numIns, numOuts; };

    //==============================================================================
    /** Creates an instance of the default plugin.

        The settings object can be a PropertySet that the class should use to store its
        settings - the takeOwnershipOfSettings indicates whether this object will delete
        the settings automatically when no longer needed. The settings can also be nullptr.

        A default device name can be passed in.

        Preferably a complete setup options object can be used, which takes precedence over
        the preferredDefaultDeviceName and allows you to select the input & output device names,
        sample rate, buffer size etc.

        In all instances, the settingsToUse will take precedence over the "preferred" options if not null.
    */
    StandalonePluginHolder (juce::PropertySet* settingsToUse,
                            bool takeOwnershipOfSettings = true,
                            const juce::String& preferredDefaultDeviceName = juce::String(),
                            const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr,
                            const juce::Array<PluginInOuts>& channels = juce::Array<PluginInOuts>(),
                           #if JUCE_ANDROID || JUCE_IOS
                            bool shouldAutoOpenMidiDevices = true
                           #else
                            bool shouldAutoOpenMidiDevices = false
                           #endif
	);

	~StandalonePluginHolder() override;

	void init (bool enableAudioInput, const juce::String& preferredDefaultDeviceName);

    //==============================================================================
	void createPlugin();
	void deletePlugin();

	int getNumInputChannels() const;
	int getNumOutputChannels() const;

	static juce::String getFilePatterns (const juce::String& fileSuffix);

    //==============================================================================
    juce::Value& getMuteInputValue()                     { return shouldMuteInput; }
    bool getProcessorHasPotentialFeedbackLoop() const    { return processorHasPotentialFeedbackLoop; }
    void valueChanged (juce::Value& value) override      { muteInput = (bool) value.getValue(); }

    //==============================================================================
	juce::File getLastFile() const;
	void setLastFile (const juce::FileChooser& fc);

    /** Pops up a dialog letting the user save the processor's state to a file. */
	void askUserToSaveState (const juce::String& fileSuffix = juce::String());

    /** Pops up a dialog letting the user re-load the processor's state from a file. */
	void askUserToLoadState (const juce::String& fileSuffix = juce::String());

    //==============================================================================
	void startPlaying();
	void stopPlaying();

    //==============================================================================
    /** Shows an audio properties dialog box modally. */
	void showAudioSettingsDialog();
	void saveAudioDeviceState();

    void reloadAudioDeviceState (bool enableAudioInput,
                                 const juce::String& preferredDefaultDeviceName,
								 const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions);

    //==============================================================================
	void savePluginState();
	void reloadPluginState();

    //==============================================================================
	void switchToHostApplication();
	bool isInterAppAudioConnected();

	juce::Image getIAAHostIcon ([[maybe_unused]] int size);

	static StandalonePluginHolder* getInstance();

    //==============================================================================
    juce::OptionalScopedPointer<juce::PropertySet> settings;
    std::unique_ptr<juce::AudioProcessor> processor;
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer player;
    juce::Array<PluginInOuts> channelConfiguration;

    // avoid feedback loop by default
    bool processorHasPotentialFeedbackLoop = true;
    std::atomic<bool> muteInput { true };
    juce::Value shouldMuteInput;
    juce::AudioBuffer<float> emptyBuffer;
    bool autoOpenMidiDevices;

    std::unique_ptr<juce::AudioDeviceManager::AudioDeviceSetup> options;
    juce::Array<juce::MidiDeviceInfo> lastMidiDevices;

    std::unique_ptr<juce::FileChooser> stateFileChooser;
    juce::ScopedMessageBox messageBox;

private:
    inline static StandalonePluginHolder* currentInstance = nullptr;

    //==============================================================================
	void handleCreatePlugin();
	void handleDeletePlugin();

    //==============================================================================
    class SettingsComponent : public juce::Component
    {
    public:
        SettingsComponent (StandalonePluginHolder& pluginHolder,
                           juce::AudioDeviceManager& deviceManagerToUse,
                           int maxAudioInputChannels,
                           int maxAudioOutputChannels)
            : owner (pluginHolder),
              deviceSelector (deviceManagerToUse,
                              0, maxAudioInputChannels,
                              0, maxAudioOutputChannels,
                              true,
                              (pluginHolder.processor.get() != nullptr && pluginHolder.processor->producesMidi()),
                              true, false),
              shouldMuteLabel  ("Feedback Loop:", "Feedback Loop:"),
              shouldMuteButton ("Mute audio input")
        {
            setOpaque (true);

            shouldMuteButton.setClickingTogglesState (true);
            shouldMuteButton.getToggleStateValue().referTo (owner.shouldMuteInput);

            addAndMakeVisible (deviceSelector);

            if (owner.getProcessorHasPotentialFeedbackLoop())
            {
                addAndMakeVisible (shouldMuteButton);
                addAndMakeVisible (shouldMuteLabel);

                shouldMuteLabel.attachToComponent (&shouldMuteButton, true);
            }
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        }

        void resized() override
        {
            const juce::ScopedValueSetter<bool> scope (isResizing, true);

            auto r = getLocalBounds();

            if (owner.getProcessorHasPotentialFeedbackLoop())
            {
                auto itemHeight = deviceSelector.getItemHeight();
                auto extra = r.removeFromTop (itemHeight);

                auto seperatorHeight = (itemHeight >> 1);
                shouldMuteButton.setBounds (juce::Rectangle<int> (extra.proportionOfWidth (0.35f), seperatorHeight,
                                                                  extra.proportionOfWidth (0.60f), deviceSelector.getItemHeight()));

                r.removeFromTop (seperatorHeight);
            }

            deviceSelector.setBounds (r);
        }

        void childBoundsChanged (juce::Component* childComp) override
        {
            if (! isResizing && childComp == &deviceSelector)
                setToRecommendedSize();
        }

        void setToRecommendedSize()
        {
            const auto extraHeight = [&]
            {
                if (! owner.getProcessorHasPotentialFeedbackLoop())
                    return 0;

                const auto itemHeight = deviceSelector.getItemHeight();
                const auto separatorHeight = (itemHeight >> 1);
                return itemHeight + separatorHeight;
            }();

            setSize (getWidth(), deviceSelector.getHeight() + extraHeight);
        }

    private:
        //==============================================================================
        StandalonePluginHolder& owner;
        juce::AudioDeviceSelectorComponent deviceSelector;
        juce::Label shouldMuteLabel;
        juce::ToggleButton shouldMuteButton;
        bool isResizing = false;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsComponent)
    };

    //==============================================================================
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int numInputChannels,
                                           float* const* outputChannelData,
                                           int numOutputChannels,
                                           int numSamples,
										   const juce::AudioIODeviceCallbackContext& context) override;

	void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
	void audioDeviceStopped() override;

    //==============================================================================
    void setupAudioDevices (bool enableAudioInput,
                            const juce::String& preferredDefaultDeviceName,
							const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions);

	void shutDownAudioDevices();
	void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandalonePluginHolder)
};

#pragma once

//==============================================================================
/**
    A class that can be used to run a simple standalone application containing your filter.

    Just create one of these objects in your JUCEApplicationBase::initialise() method, and
    let it do its work. It will create your filter object using the same createPluginFilter() function
    that the other plugin wrappers use.

    @tags{Audio}
*/
class StandaloneFilterWindow : public juce::DocumentWindow,
                               private juce::Button::Listener
{
public:
    //==============================================================================
    typedef StandalonePluginHolder::PluginInOuts PluginInOuts;

    StandaloneFilterWindow (const juce::String& title,
                            juce::Colour backgroundColour,
							std::unique_ptr<StandalonePluginHolder> pluginHolderIn);

    //==============================================================================
    /** Creates a window with a given title and colour.
        The settings object can be a PropertySet that the class should use to
        store its settings (it can also be null). If takeOwnershipOfSettings is
        true, then the settings object will be owned and deleted by this object.
    */
    StandaloneFilterWindow (const juce::String& title,
                            juce::Colour backgroundColour,
                            juce::PropertySet* settingsToUse,
                            bool takeOwnershipOfSettings,
                            const juce::String& preferredDefaultDeviceName = juce::String(),
                            const juce::AudioDeviceManager::AudioDeviceSetup* preferredSetupOptions = nullptr,
                            const juce::Array<PluginInOuts>& constrainToConfiguration = {},
                           #if JUCE_ANDROID || JUCE_IOS
                            bool autoOpenMidiDevices = true
                           #else
                            bool autoOpenMidiDevices = false
                           #endif
						   );

	~StandaloneFilterWindow() override;

    //==============================================================================
    juce::AudioProcessor* getAudioProcessor() const noexcept    { return pluginHolder->processor.get(); }
    juce::AudioDeviceManager& getDeviceManager() const noexcept { return pluginHolder->deviceManager; }

    /** Deletes and re-creates the plugin, resetting it to its default state. */
	void resetToDefaultState();
	void reopenEditor();

    //==============================================================================
	void closeButtonPressed() override;
	void resized() override;

    virtual StandalonePluginHolder* getPluginHolder()    { return pluginHolder.get(); }

    std::unique_ptr<StandalonePluginHolder> pluginHolder;

private:
	void updateContent();
	void buttonClicked (juce::Button* button) override;

    /*  This custom constrainer checks with the AudioProcessorEditor (which might itself be
        constrained) to ensure that any size we choose for the standalone window will be suitable
        for the editor too.

        Without this constrainer, attempting to resize the standalone window may set bounds on the
        peer that are unsupported by the inner editor. In this scenario, the peer will be set to a
        'bad' size, then the inner editor will be resized. The editor will check the new bounds with
        its own constrainer, and may set itself to a more suitable size. After that, the resizable
        window will see that its content component has changed size, and set the bounds of the peer
        accordingly. The end result is that the peer is resized twice in a row to different sizes,
        which can appear glitchy/flickery to the user.
    */
    class DecoratorConstrainer : public juce::BorderedComponentBoundsConstrainer
    {
    public:
        ComponentBoundsConstrainer* getWrappedConstrainer() const override
        {
            return contentComponent != nullptr ? contentComponent->getEditorConstrainer() : nullptr;
        }

        juce::BorderSize<int> getAdditionalBorder() const override
        {
            return contentComponent != nullptr ? contentComponent->computeBorder() : juce::BorderSize<int>{};
        }

        void setMainContentComponent (MainContentComponent* in) { contentComponent = in; }

    private:
        MainContentComponent* contentComponent = nullptr;
    };

    //==============================================================================
    juce::TextButton optionsButton;
    DecoratorConstrainer decoratorConstrainer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandaloneFilterWindow)
};

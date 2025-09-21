#pragma once

class StandaloneFilterWindow;

//==============================================================================
class MainContentComponent  : public juce::Component,
                              private juce::Value::Listener,
                              private juce::Button::Listener,
                              private juce::ComponentListener
{
public:
    MainContentComponent (StandaloneFilterWindow& filterWindow);
    ~MainContentComponent() override;

    void resized() override;

    juce::ComponentBoundsConstrainer* getEditorConstrainer() const;
    juce::BorderSize<int> computeBorder() const;

private:
    //==============================================================================
    class NotificationArea : public juce::Component
    {
    public:
        enum { height = 30 };

        NotificationArea (juce::Button::Listener* settingsButtonListener)
            : notification ("notification", "Audio input is muted to avoid feedback loop"),
             #if JUCE_IOS || JUCE_ANDROID
              settingsButton ("Unmute Input")
             #else
              settingsButton ("Settings...")
             #endif
        {
            setOpaque (true);

            notification.setColour (juce::Label::textColourId, juce::Colours::black);

            settingsButton.addListener (settingsButtonListener);

            addAndMakeVisible (notification);
            addAndMakeVisible (settingsButton);
        }

        void paint (juce::Graphics& g) override
        {
            auto r = getLocalBounds();

            g.setColour (juce::Colours::darkgoldenrod);
            g.fillRect (r.removeFromBottom (1));

            g.setColour (juce::Colours::lightgoldenrodyellow);
            g.fillRect (r);
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced (5);

            settingsButton.setBounds (r.removeFromRight (70));
            notification.setBounds (r);
        }
    private:
        juce::Label notification;
        juce::TextButton settingsButton;
    };

    //==============================================================================
    void inputMutedChanged (bool newInputMutedValue);

    void valueChanged (juce::Value& value) override;
    void buttonClicked (juce::Button*) override;

    //==============================================================================
    void handleResized();
    void handleMovedOrResized();

    void componentMovedOrResized (juce::Component&, bool, bool) override;
    juce::Rectangle<int> getSizeToContainEditor() const;

    //==============================================================================
    StandaloneFilterWindow& owner;
    NotificationArea notification;
    SideBarComponent sidebar { owner };
    std::unique_ptr<juce::AudioProcessorEditor> editor;
    juce::Value inputMutedValue;
    bool shouldShowNotification = false;
    bool shouldShowSidebar = true;
    bool preventResizingEditor = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

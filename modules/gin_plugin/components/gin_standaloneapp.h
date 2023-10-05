#pragma once

#ifdef JucePlugin_Name
class StandaloneApp : public juce::JUCEApplication
{
public:
    StandaloneApp (std::function<bool (juce::PropertiesFile&)>);

    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;
    void anotherInstanceStarted (const juce::String&) override;
    void initialise (const juce::String& /*commandLineParameters*/) override;
    void shutdown() override;
    void systemRequestedQuit() override;

private:
    juce::StandaloneFilterWindow* createWindow();

    std::function<bool (juce::PropertiesFile&)>     onInitialise;
    juce::ApplicationProperties                     appProperties;
    std::unique_ptr<juce::StandaloneFilterWindow>   mainWindow;
};
//----------------------------------------------------------------------------------
#endif

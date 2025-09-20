#pragma once

class StandaloneFilterWindow;

//==============================================================================
class StandaloneFilterApp : public juce::JUCEApplication
{
public:
	StandaloneFilterApp();

	const juce::String getApplicationName() override            { return juce::CharPointer_UTF8 (JucePlugin_Name); }
	const juce::String getApplicationVersion() override         { return JucePlugin_VersionString; }
	bool moreThanOneInstanceAllowed() override                  { return true; }
	void anotherInstanceStarted (const juce::String&) override  {}

	virtual StandaloneFilterWindow* createWindow();
	virtual std::unique_ptr<StandalonePluginHolder> createPluginHolder();

	//==============================================================================
	void initialise (const juce::String&) override;
	void shutdown() override;

	//==============================================================================
	void systemRequestedQuit() override;

protected:
	juce::ApplicationProperties appProperties;
	std::unique_ptr<StandaloneFilterWindow> mainWindow;

private:
	std::unique_ptr<StandalonePluginHolder> pluginHolder;
};

juce::JUCEApplicationBase* juce_CreateApplication();


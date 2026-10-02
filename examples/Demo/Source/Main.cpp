/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MainComponent.h"


//==============================================================================
class DemoApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    DemoApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    //==============================================================================
    void initialise (const juce::String& cmdLine) override
    {
       #if defined JUCE_MAC || defined JUCE_WINDOWS
        if (gin::ElevatedFileCopy::processCommandLine (cmdLine))
            return;
       #endif

        mainWindow = std::make_unique<MainWindow> (getApplicationName());

        // Remote control for tools/gin_remote: gin_remote.py tree, click, screenshot etc.
        remote = std::make_unique<gin::RemoteServer>();
        remote->addCommand ("demoInfo", "Example of a product specific command", R"({"shout":"bool: upper case the answer"})",
                            [] (const juce::var& args)
                            {
                                juce::String s = "Hello from the Gin demo";
                                if (bool (gin::RemoteServer::getArg (args, "shout", false)))
                                    s = s.toUpperCase();
                                return gin::RemoteServer::CommandResult (juce::var (s));
                            });
        remote->start();
    }

    void shutdown() override
    {
        remote = nullptr;
        mainWindow = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String&) override
    {
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name)  : DocumentWindow (name,
                                                          juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                          juce::DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar (true);
            setContentOwned (new MainContentComponent(), true);

            setResizable (true, false);
            centreWithSize (getWidth(), getHeight());
            setVisible (true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<gin::RemoteServer> remote;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (DemoApplication)

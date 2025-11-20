#include "PluginProcessor.h"
#include "PluginEditor.h"

UnitTestsAudioProcessorEditor::UnitTestsAudioProcessorEditor (UnitTestsAudioProcessor& p)
    : AudioProcessorEditor (&p)
{
    setSize (800, 600);

    logTextEditor.setMultiLine (true);
    logTextEditor.setReadOnly (true);
    logTextEditor.setScrollbarsShown (true);
    logTextEditor.setCaretVisible (false);
    
    juce::Font font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::plain));
    logTextEditor.setFont (font);
    addAndMakeVisible (logTextEditor);

    logTextEditor.setText ("Unit Tests - Standalone Plugin\n\nWaiting to start tests...\n");

    // Start timer to run tests after 1 second
    startTimer (1000);
}

UnitTestsAudioProcessorEditor::~UnitTestsAudioProcessorEditor()
{
    stopTimer();
}

void UnitTestsAudioProcessorEditor::timerCallback()
{
    stopTimer();
    
    logTextEditor.moveCaretToEnd();
    logTextEditor.insertTextAtCaret ("Starting unit tests...\n\n");
    
    juce::Thread::launch ([this] { runUnitTests(); });
}

void UnitTestsAudioProcessorEditor::runUnitTests()
{
    class LoggingUnitTestRunner : public juce::UnitTestRunner
    {
    public:
        LoggingUnitTestRunner (juce::TextEditor& editor) : textEditor (editor) {}

        void logMessage (const juce::String& message) override
        {
            juce::WeakReference<LoggingUnitTestRunner> safeThis = this;
            juce::MessageManager::callAsync ([this, safeThis, message]()
            {
                if (safeThis)
                {
                    textEditor.moveCaretToEnd();
                    textEditor.insertTextAtCaret (message + "\n");
                    
                    juce::Logger::outputDebugString (message);
                }
            });
        }
        
        JUCE_DECLARE_WEAK_REFERENCEABLE(LoggingUnitTestRunner)

    private:
        juce::TextEditor& textEditor;
    };

    auto runner = std::make_unique<LoggingUnitTestRunner> (logTextEditor);

    runner->logMessage ("Running all unit tests\n");
    runner->runAllTests();

    numFailures = 0;
    for (auto i = 0; i < runner->getNumResults(); i++)
    {
        if (auto res = runner->getResult (i))
            numFailures += res->failures;
    }

    juce::String resultMessage = "\n========================================\n";
    resultMessage += "Unit tests complete: " + juce::String (numFailures) + " errors\n";
    resultMessage += "========================================\n";

    runner->logMessage (resultMessage);

    juce::Thread::sleep (2000);
    juce::JUCEApplication::getInstance()->setApplicationReturnValue (numFailures > 0 ? 1 : 0);
    juce::JUCEApplication::quit ();
}

void UnitTestsAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void UnitTestsAudioProcessorEditor::resized()
{
    logTextEditor.setBounds (getLocalBounds().reduced (10));
}

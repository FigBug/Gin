#include "PluginProcessor.h"
#include "PluginEditor.h"

UnitTestsAudioProcessorEditor::UnitTestsAudioProcessorEditor (UnitTestsAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 600);

    logTextEditor.setMultiLine (true);
    logTextEditor.setReadOnly (true);
    logTextEditor.setScrollbarsShown (true);
    logTextEditor.setCaretVisible (false);
    logTextEditor.setFont (juce::Font (juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::plain));
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

    if (!testsStarted)
    {
        testsStarted = true;
        runUnitTests();
    }
}

void UnitTestsAudioProcessorEditor::runUnitTests()
{
    logTextEditor.moveCaretToEnd();
    logTextEditor.insertTextAtCaret ("Starting unit tests...\n\n");

    class LoggingUnitTestRunner : public juce::UnitTestRunner
    {
    public:
        LoggingUnitTestRunner (juce::TextEditor& editor) : textEditor (editor) {}

        void logMessage (const juce::String& message) override
        {
            juce::MessageManager::callAsync ([this, message]()
            {
                textEditor.moveCaretToEnd();
                textEditor.insertTextAtCaret (message + "\n");
            });
        }

    private:
        juce::TextEditor& textEditor;
    };

    auto runner = std::make_unique<LoggingUnitTestRunner> (logTextEditor);

    logTextEditor.insertTextAtCaret ("Running all unit tests\n\n");
    runner->runAllTests();

    numFailures = 0;
    for (auto i = 0; i < runner->getNumResults(); i++)
    {
        if (auto res = runner->getResult (i))
            numFailures += res->failures;
    }

    testsComplete = true;

    juce::String resultMessage = "\n========================================\n";
    resultMessage += "Unit tests complete: " + juce::String (numFailures) + " errors\n";
    resultMessage += "========================================\n";

    logTextEditor.moveCaretToEnd();
    logTextEditor.insertTextAtCaret (resultMessage);

    repaint();
}

void UnitTestsAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void UnitTestsAudioProcessorEditor::resized()
{
    logTextEditor.setBounds (getLocalBounds().reduced (10));
}

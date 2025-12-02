#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

// Demo includes
#include "../../Demo/Source/Demos.h"

UnitTestsAudioProcessorEditor::UnitTestsAudioProcessorEditor (UnitTestsAudioProcessor& p)
    : AudioProcessorEditor (&p)
{
    setSize (800, 600);

    // Setup log text editor
    logTextEditor.setMultiLine (true);
    logTextEditor.setReadOnly (true);
    logTextEditor.setScrollbarsShown (true);
    logTextEditor.setCaretVisible (false);

    juce::Font font (juce::FontOptions (juce::Font::getDefaultMonospacedFontName(), 14.0f, juce::Font::plain));
    logTextEditor.setFont (font);
    addAndMakeVisible (logTextEditor);

    logTextEditor.setText ("Unit Tests - Standalone Plugin\n\nWaiting to start tests...\n");

    // Add all demos
    demoComponents.add (new LayoutDemo());
    demoComponents.add (new EquationParserDemo());
    demoComponents.add (new ComponentGridDemo());
    demoComponents.add (new EasingDemo());
    demoComponents.add (new BLLTDemo());
    demoComponents.add (new WavetableDemo());
    demoComponents.add (new CatenaryDemo());
    demoComponents.add (new EllipseDemo());
    demoComponents.add (new PerlinNoiseDemo());
    demoComponents.add (new TextRenderDemo());
    demoComponents.add (new AsyncUpdateDemo());
    demoComponents.add (new ValueTreeJsonDemo());
    demoComponents.add (new MessagePackDemo());
    demoComponents.add (new SVGDemo());
    demoComponents.add (new WebsocketDemo());
    demoComponents.add (new SolidBlendingDemo());
    demoComponents.add (new BlendingDemo());
    demoComponents.add (new GradientMapDemo());
    demoComponents.add (new ImageResizeDemo());
   #if defined JUCE_MAC || defined JUCE_WINDOWS
    demoComponents.add (new ElevatedFileCopyDemo());
   #endif
    demoComponents.add (new DownloadManagerDemo());
    demoComponents.add (new DownloadManagerToSaveToFileDemo());
    demoComponents.add (new ImageEffectsDemo());
    demoComponents.add (new BoxBlurDemo());
   #if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
    demoComponents.add (new FileSystemWatcherDemo());
   #endif
    demoComponents.add (new MetadataDemo());
    demoComponents.add (new BmpImageDemo());
    demoComponents.add (new WebpImageDemo());
    demoComponents.add (new MapDemo());
    demoComponents.add (new SharedMemoryDemo());
    demoComponents.add (new LeastSquaresDemo());
    demoComponents.add (new LinearDemo());
    demoComponents.add (new SplineDemo());
    demoComponents.add (new LagrangeDemo());

    // Add all demos as child components
    for (auto* demo : demoComponents)
        addChildComponent (demo);

    // Show first demo
    if (demoComponents.size() > 0)
        demoComponents[currentDemoIndex]->setVisible (true);

    resized();

    // Start timer at 100ms - will switch demos and check for unit test start
    startTimer (100);
}

UnitTestsAudioProcessorEditor::~UnitTestsAudioProcessorEditor()
{
    stopTimer();
}

void UnitTestsAudioProcessorEditor::timerCallback()
{
    timerCount++;

    // Start unit tests after 1 second (10 ticks at 100ms)
    if (!unitTestsStarted && timerCount >= 10)
    {
        unitTestsStarted = true;

        logTextEditor.moveCaretToEnd();
        logTextEditor.insertTextAtCaret ("Starting unit tests...\n\n");

        juce::Thread::launch ([this] { runUnitTests(); });
    }

    // Switch demos every 100ms
    if (demoComponents.size() > 0)
    {
        // Hide current demo
        demoComponents[currentDemoIndex]->setVisible (false);

        // Move to next demo
        currentDemoIndex = (currentDemoIndex + 1) % demoComponents.size();

        // Show next demo
        demoComponents[currentDemoIndex]->setVisible (true);
        resized();
    }
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
    auto bounds = getLocalBounds();

    // Bottom 1/4 for unit test log
    auto logBounds = bounds.removeFromBottom (getHeight() / 4);
    logTextEditor.setBounds (logBounds.reduced (10));

    // Top 3/4 for demos
    for (auto* demo : demoComponents)
        demo->setBounds (bounds);
}

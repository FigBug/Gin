//==============================================================================
#if GIN_UNIT_TESTS

// Minimal mock Processor for testing
class MockProcessorForSmoothedParameterTests : public gin::Processor
{
public:
    MockProcessorForSmoothedParameterTests() : gin::Processor (false) {}

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
};

class SmoothedParameterTests : public juce::UnitTest
{
public:
    SmoothedParameterTests() : juce::UnitTest ("SmoothedParameter", "gin_plugin") {}

    void runTest() override
    {
        juce::MessageManagerLock mmLock;
        
        testBasicSmoothedParameter();
        testSmoothingBehavior();
        testSmoothingTime();
        testSmoothingActive();
        testSmoothingReset();
        testValueChangeDuringSmoothing();
        testPrepareToPlay();
    }

private:
    void testBasicSmoothedParameter()
    {
        beginTest ("Basic Smoothed Parameter");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 50.0f);

        expectWithinAbsoluteError (param->getUserValue(), 50.0f, 0.001f, "Should start at default value");
        expect (!param->isSmoothingActive(), "Should not be smoothing initially");

        delete param;
    }

    void testSmoothingBehavior()
    {
        beginTest ("Smoothing Behavior");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 0.0f);

        param->prepareToPlay (44100.0, 512);
        param->setSmoothingTime (0.1f);
        param->reset();

        // Change value - should trigger smoothing
        param->setUserValue (100.0f);
        expectWithinAbsoluteError (param->getUserValue(), 100.0f, 0.001f, "User value should be target immediately");

        // During smoothing, proc value should lag behind
        float procValue1 = param->getProcValue (128);
        expect (procValue1 < 100.0f, "Proc value should be smoothing toward target");
        expect (param->isSmoothingActive(), "Should be smoothing");

        // Keep calling getProcValue to advance smoothing
        for (int i = 0; i < 100; ++i)
        {
            param->getProcValue (128);
            if (!param->isSmoothingActive())
                break;
        }

        // Eventually should reach target
        float procValueFinal = param->getProcValue (128);
        expectWithinAbsoluteError (procValueFinal, 100.0f, 0.1f, "Should eventually reach target");
        expect (!param->isSmoothingActive(), "Should stop smoothing when reached");

        delete param;
    }

    void testSmoothingTime()
    {
        beginTest ("Smoothing Time");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 0.0f);

        param->prepareToPlay (44100.0, 512);

        // Faster smoothing time
        param->setSmoothingTime (0.01f);
        param->reset();
        param->setUserValue (100.0f);

        int steps1 = 0;
        while (param->isSmoothingActive() && steps1 < 1000)
        {
            param->getProcValue (128);
            steps1++;
        }

        // Slower smoothing time
        param->setUserValue (0.0f);
        param->setSmoothingTime (0.2f);
        param->reset();
        param->setUserValue (100.0f);

        int steps2 = 0;
        while (param->isSmoothingActive() && steps2 < 1000)
        {
            param->getProcValue (128);
            steps2++;
        }

        expect (steps2 > steps1, "Longer smoothing time should take more steps");

        delete param;
    }

    void testSmoothingActive()
    {
        beginTest ("Smoothing Active Check");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 50.0f);

        param->prepareToPlay (44100.0, 512);
        param->setSmoothingTime (0.05f);
        param->reset();

        expect (!param->isSmoothingActive(), "Should not be smoothing initially");

        param->setUserValue (75.0f);
        expect (param->isSmoothingActive(), "Should be smoothing after value change");

        // Advance until done
        for (int i = 0; i < 1000 && param->isSmoothingActive(); ++i)
            param->getProcValue (128);

        expect (!param->isSmoothingActive(), "Should stop smoothing when done");

        delete param;
    }

    void testSmoothingReset()
    {
        beginTest ("Smoothing Reset");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 0.0f);

        param->prepareToPlay (44100.0, 512);
        param->setSmoothingTime (0.1f);

        param->setUserValue (100.0f);
        expect (param->isSmoothingActive(), "Should be smoothing");

        // Reset should snap to current value
        param->reset();
        expect (!param->isSmoothingActive(), "Should not be smoothing after reset");

        float procValue = param->getProcValue (128);
        expectWithinAbsoluteError (procValue, 100.0f, 0.1f, "Should snap to target value after reset");

        delete param;
    }

    void testValueChangeDuringSmoothing()
    {
        beginTest ("Value Change During Smoothing");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 0.0f);

        param->prepareToPlay (44100.0, 512);
        param->setSmoothingTime (0.1f);
        param->reset();

        // Start smoothing to 100
        param->setUserValue (100.0f);
        param->getProcValue (128);
        param->getProcValue (128);

        float intermediate = param->getProcValue (128);
        expect (intermediate > 0.0f && intermediate < 100.0f, "Should be in between");

        // Change target while smoothing
        param->setUserValue (50.0f);

        // Should start smoothing toward new target
        expect (param->isSmoothingActive(), "Should still be smoothing");

        // Advance smoothing
        for (int i = 0; i < 1000 && param->isSmoothingActive(); ++i)
            param->getProcValue (128);

        float final = param->getProcValue (128);
        expectWithinAbsoluteError (final, 50.0f, 0.5f, "Should reach new target");

        delete param;
    }

    void testPrepareToPlay()
    {
        beginTest ("Prepare To Play");

        MockProcessorForSmoothedParameterTests mockProc;
        auto* param = new gin::SmoothedParameter<gin::ValueSmoother<float>>
                        (mockProc, "test", "Test", "Test", "",
                         0.0f, 100.0f, 1.0f, 50.0f);

        // Test different sample rates
        param->prepareToPlay (44100.0, 512);
        param->setSmoothingTime (0.1f);
        param->reset();
        param->setUserValue (100.0f);

        int steps44k = 0;
        while (param->isSmoothingActive() && steps44k < 1000)
        {
            param->getProcValue (128);
            steps44k++;
        }

        param->setUserValue (0.0f);
        param->prepareToPlay (96000.0, 512);
        param->setSmoothingTime (0.1f);
        param->reset();
        param->setUserValue (100.0f);

        int steps96k = 0;
        while (param->isSmoothingActive() && steps96k < 1000)
        {
            param->getProcValue (128);
            steps96k++;
        }

        expect (steps96k > steps44k, "Higher sample rate should take more steps for same time");

        delete param;
    }
};

static SmoothedParameterTests smoothedParameterTests;

#endif

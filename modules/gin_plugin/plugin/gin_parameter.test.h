//==============================================================================
#if GIN_UNIT_TESTS

// Minimal mock Processor for testing
class MockProcessorForParameterTests : public gin::Processor
{
public:
    MockProcessorForParameterTests() : gin::Processor (false) {}

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
};

class ParameterTests : public juce::UnitTest
{
public:
    ParameterTests() : juce::UnitTest ("Parameter", "gin_plugin") {}

    void runTest() override
    {
        const juce::MessageManagerLock mmLock;
        
        testBasicParameterCreation();
        testValueConversions();
        testUserValueRange();
        testNormalizedValue();
        testConversionFunction();
        testOnOffParameter();
        testParameterState();
        testSkewedParameter();
        testTextFunction();
        testInternalParameter();
    }

private:
    void testBasicParameterCreation()
    {
        beginTest ("Basic Parameter Creation");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "test", "Test Param", "Test", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        expect (param->getUid() == "test", "UID should match");
        expect (param->getName(100) == "Test Param", "Name should match");
        expect (param->getShortName() == "Test", "Short name should match");
        expectWithinAbsoluteError (param->getUserDefaultValue(), 50.0f, 0.001f, "Default value should be 50");
        expectWithinAbsoluteError (param->getUserValue(), 50.0f, 0.001f, "Initial value should be default");

        delete param;
    }

    void testValueConversions()
    {
        beginTest ("Value Conversions");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "test", "Test", "Test", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        // Test that user value and proc value are the same by default
        param->setUserValue (75.0f);
        expectWithinAbsoluteError (param->getUserValue(), 75.0f, 0.001f, "User value should be 75");
        expectWithinAbsoluteError (param->getProcValue(), 75.0f, 0.001f, "Proc value should match user value");

        // Test normalized value (0..1)
        expectWithinAbsoluteError (param->getValue(), 0.75f, 0.001f, "Normalized value should be 0.75");

        delete param;
    }

    void testUserValueRange()
    {
        beginTest ("User Value Range");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "test", "Test", "Test", "",
                                          -10.0f, 10.0f, 0.1f, 0.0f);

        // Test clamping
        param->setUserValue (15.0f);
        expectWithinAbsoluteError (param->getUserValue(), 10.0f, 0.001f, "Value should be clamped to max");

        param->setUserValue (-20.0f);
        expectWithinAbsoluteError (param->getUserValue(), -10.0f, 0.001f, "Value should be clamped to min");

        param->setUserValue (5.0f);
        expectWithinAbsoluteError (param->getUserValue(), 5.0f, 0.001f, "Value within range should be preserved");

        delete param;
    }

    void testNormalizedValue()
    {
        beginTest ("Normalized Value");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "test", "Test", "Test", "",
                                          0.0f, 100.0f, 1.0f, 0.0f);

        // Test setValue (normalized 0..1)
        param->setValue (0.0f);
        expectWithinAbsoluteError (param->getUserValue(), 0.0f, 0.001f, "0.0 normalized = 0.0 user");

        param->setValue (1.0f);
        expectWithinAbsoluteError (param->getUserValue(), 100.0f, 0.001f, "1.0 normalized = 100.0 user");

        param->setValue (0.5f);
        expectWithinAbsoluteError (param->getUserValue(), 50.0f, 0.001f, "0.5 normalized = 50.0 user");

        // Test getValue returns normalized
        param->setUserValue (25.0f);
        expectWithinAbsoluteError (param->getValue(), 0.25f, 0.001f, "25.0 user = 0.25 normalized");

        delete param;
    }

    void testConversionFunction()
    {
        beginTest ("Conversion Function");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "gain", "Gain", "Gain", "dB",
                                          -100.0f, 10.0f, 0.1f, 0.0f);

        // Add dB to linear conversion
        param->conversionFunction = [] (float db) {
            return juce::Decibels::decibelsToGain (db);
        };

        // Test that proc value uses conversion but user value doesn't
        param->setUserValue (0.0f);
        expectWithinAbsoluteError (param->getUserValue(), 0.0f, 0.001f, "User value should be 0 dB");
        expectWithinAbsoluteError (param->getProcValue(), 1.0f, 0.001f, "Proc value should be 1.0 (linear)");

        param->setUserValue (-6.0f);
        expectWithinAbsoluteError (param->getUserValue(), -6.0f, 0.001f, "User value should be -6 dB");
        expectWithinAbsoluteError (param->getProcValue(), 0.5012f, 0.001f, "Proc value should be ~0.5 (linear)");

        delete param;
    }

    void testOnOffParameter()
    {
        beginTest ("On/Off Parameter");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "switch", "Switch", "Sw", "",
                                          0.0f, 1.0f, 1.0f, 0.0f);

        expect (param->isOnOff(), "Should be recognized as on/off parameter");

        param->setUserValue (0.0f);
        expect (!param->isOn(), "Should be off when value is min");
        expect (!param->getUserValueBool(), "Bool value should be false");

        param->setUserValue (1.0f);
        expect (param->isOn(), "Should be on when value is max");
        expect (param->getUserValueBool(), "Bool value should be true");

        delete param;
    }

    void testParameterState()
    {
        beginTest ("Parameter State");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "test", "Test", "Test", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        param->setUserValue (75.0f);

        // Get state
        auto state = param->getState();
        expect (state.uid == "test", "State should preserve UID");
        expectWithinAbsoluteError (state.value, 75.0f, 0.001f, "State should preserve value");

        // Change value
        param->setUserValue (25.0f);
        expectWithinAbsoluteError (param->getUserValue(), 25.0f, 0.001f, "Value should be changed");

        // Restore state
        param->setState (state);
        expectWithinAbsoluteError (param->getUserValue(), 75.0f, 0.001f, "Value should be restored from state");

        delete param;
    }

    void testSkewedParameter()
    {
        beginTest ("Skewed Parameter");

        MockProcessorForParameterTests mockProc;

        // Frequency parameter with logarithmic skew
        auto* param = new gin::Parameter (mockProc, "freq", "Frequency", "Freq", "Hz",
                                          20.0f, 20000.0f, 1.0f, 1000.0f, 0.3f);

        expect (param->getSkew() < 1.0f, "Should have logarithmic skew");

        // With skew, normalized 0.5 should not map to exactly middle of range
        param->setValue (0.5f);
        float userVal = param->getUserValue();
        expect (userVal > 20.0f && userVal < 20000.0f, "Value should be in range");
        expect (userVal < 10010.0f, "With log skew, 0.5 normalized should be < linear midpoint");

        delete param;
    }

    void testTextFunction()
    {
        beginTest ("Text Function");

        MockProcessorForParameterTests mockProc;

        // Parameter with custom text function
        auto textFunc = [] (const gin::Parameter&, float value) -> juce::String {
            if (value < 0.33f) return "Low";
            if (value < 0.67f) return "Mid";
            return "High";
        };

        auto* param = new gin::Parameter (mockProc, "quality", "Quality", "Qual", "",
                                          0.0f, 1.0f, 0.01f, 0.5f, 1.0f, textFunc);

        param->setValue (0.2f);
        expect (param->getText (0.2f, 100).contains ("Low"), "Should show Low for 0.2");

        param->setValue (0.5f);
        expect (param->getText (0.5f, 100).contains ("Mid"), "Should show Mid for 0.5");

        param->setValue (0.8f);
        expect (param->getText (0.8f, 100).contains ("High"), "Should show High for 0.8");

        delete param;
    }

    void testInternalParameter()
    {
        beginTest ("Internal Parameter");

        MockProcessorForParameterTests mockProc;
        auto* param = new gin::Parameter (mockProc, "internal", "Internal", "Int", "",
                                          0.0f, 1.0f, 0.01f, 0.0f);

        expect (!param->isInternal(), "Should not be internal by default");

        param->setInternal (true);
        expect (param->isInternal(), "Should be internal after setting");

        delete param;
    }
};

static ParameterTests parameterTests;

#endif

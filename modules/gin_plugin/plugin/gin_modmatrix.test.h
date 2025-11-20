//==============================================================================
#if GIN_UNIT_TESTS

// Mock voice for testing ModMatrix
class MockModVoice : public gin::ModVoice
{
public:
    MockModVoice() = default;

    bool isVoiceActive() override { return active; }
    void setActive (bool a) { active = a; }

private:
    bool active = true;
};

// Minimal mock Processor for testing
class MockProcessorForModMatrixTests : public gin::Processor
{
public:
    MockProcessorForModMatrixTests() : gin::Processor (false) {}

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}

    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
};

class ModMatrixTests : public juce::UnitTest
{
public:
    ModMatrixTests() : juce::UnitTest ("ModMatrix", "gin_plugin") {}

    void runTest() override
    {
        testModSrcIdAndDstId();
        testModSourceCreation();
        testParameterAddition();
        testMonophonicModulation();
        testPolyphonicModulation();
        testModulationDepth();
        testModulationShaping();
        testModulationEnable();
        testBipolarModulation();
        testMultipleSourcesOneDestination();
        testValueClamping();
    }

private:
    void testModSrcIdAndDstId()
    {
        beginTest ("ModSrcId and ModDstId");

        gin::ModSrcId src1;
        expect (!src1.isValid(), "Default ModSrcId should be invalid");

        gin::ModSrcId src2 (5);
        expect (src2.isValid(), "ModSrcId with positive ID should be valid");
        expect (src2.id == 5, "ID should be 5");

        gin::ModSrcId src3 (src2);
        expect (src3 == src2, "Copy should be equal");

        gin::ModDstId dst1;
        expect (!dst1.isValid(), "Default ModDstId should be invalid");

        gin::ModDstId dst2 (10);
        expect (dst2.isValid(), "ModDstId with positive ID should be valid");
        expect (dst2.id == 10, "ID should be 10");
    }

    void testModSourceCreation()
    {
        beginTest ("Mod Source Creation");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;

        auto lfo1 = modMatrix.addMonoModSource ("lfo1", "LFO 1", true);
        expect (lfo1.isValid(), "Mono source should be valid");
        expect (modMatrix.getModSrcName (lfo1) == "LFO 1", "Source name should match");
        expect (!modMatrix.getModSrcPoly (lfo1), "Mono source should not be poly");
        expect (modMatrix.getModSrcBipolar (lfo1), "Source should be bipolar");

        auto env1 = modMatrix.addPolyModSource ("env1", "Envelope 1", false);
        expect (env1.isValid(), "Poly source should be valid");
        expect (modMatrix.getModSrcPoly (env1), "Poly source should be poly");
        expect (!modMatrix.getModSrcBipolar (env1), "Source should be unipolar");

        expect (modMatrix.getNumModSources() == 2, "Should have 2 sources");
    }

    void testParameterAddition()
    {
        beginTest ("Parameter Addition");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;

        auto* param = new gin::Parameter (mockProc, "cutoff", "Cutoff", "Cut", "Hz",
                                          20.0f, 20000.0f, 1.0f, 1000.0f, 0.3f);

        modMatrix.addParameter (param, false);
        expect (param->getModIndex() >= 0, "Parameter should have mod index");

        delete param;
    }

    void testMonophonicModulation()
    {
        beginTest ("Monophonic Modulation");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "cutoff", "Cutoff", "Cut", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f); // No smoothing for test

        auto lfo = modMatrix.addMonoModSource ("lfo", "LFO", false);

        modMatrix.build();

        // Set base parameter value to 0.5 (normalized)
        param->setValue (0.5f);

        // No modulation yet
        float value1 = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (value1, 50.0f, 0.1f, "Should be base value with no modulation");

        // Add modulation: depth of 0.2 (normalized)
        gin::ModDstId dst (param->getModIndex());
        modMatrix.setModDepth (lfo, dst, 0.2f);
        modMatrix.setModEnable (lfo, dst, true);

        // Set LFO output to 1.0 (max)
        modMatrix.setMonoValue (lfo, 1.0f);

        // Value should be base (0.5) + depth * lfo (0.2 * 1.0) = 0.7 normalized = 70.0 user
        float value2 = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (value2, 70.0f, 0.1f, "Should be modulated value");

        delete param;
    }

    void testPolyphonicModulation()
    {
        beginTest ("Polyphonic Modulation");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        MockModVoice voice;
        modMatrix.addVoice (&voice);

        auto* param = new gin::Parameter (mockProc, "cutoff", "Cutoff", "Cut", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f);

        auto env = modMatrix.addPolyModSource ("env", "Envelope", false);

        modMatrix.build();

        param->setValue (0.5f);

        gin::ModDstId dst (param->getModIndex());
        modMatrix.setModDepth (env, dst, 0.3f);
        modMatrix.setModEnable (env, dst, true);

        // Set per-voice envelope value
        voice.startVoice();
        modMatrix.setPolyValue (voice, env, 1.0f);

        // Value should be base (0.5) + depth * env (0.3 * 1.0) = 0.8 normalized = 80.0 user
        float value = modMatrix.getValue (voice, param, false);
        expectWithinAbsoluteError (value, 80.0f, 0.1f, "Should be poly modulated value");

        delete param;
    }

    void testModulationDepth()
    {
        beginTest ("Modulation Depth");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "param", "Param", "P", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f);
       
        auto lfo = modMatrix.addMonoModSource ("lfo", "LFO", false);
        
        modMatrix.build();
        
        gin::ModDstId dst (param->getModIndex());

        // Test getting/setting depth
        modMatrix.setModDepth (lfo, dst, 0.5f);
        expectWithinAbsoluteError (modMatrix.getModDepth (lfo, dst), 0.5f, 0.001f, "Depth should be 0.5");

        // Test clearing depth
        modMatrix.clearModDepth (lfo, dst);
        expectWithinAbsoluteError (modMatrix.getModDepth (lfo, dst), 0.0f, 0.001f, "Depth should be cleared");

        delete param;
    }

    void testModulationShaping()
    {
        beginTest ("Modulation Shaping");

        // Test linear shaping
        float linear = gin::ModMatrix::shape (0.5f, gin::ModMatrix::linear, false, false);
        expectWithinAbsoluteError (linear, 0.5f, 0.001f, "Linear should be unchanged");

        // Test inverted linear
        float invLinear = gin::ModMatrix::shape (0.5f, gin::ModMatrix::invLinear, false, false);
        expectWithinAbsoluteError (invLinear, 0.5f, 0.001f, "Inverted linear of 0.5 should be 0.5");

        float invLinearMin = gin::ModMatrix::shape (0.0f, gin::ModMatrix::invLinear, false, false);
        expectWithinAbsoluteError (invLinearMin, 1.0f, 0.001f, "Inverted linear of 0.0 should be 1.0");

        // Test quadratic shaping
        float quadIn = gin::ModMatrix::shape (0.5f, gin::ModMatrix::quadraticIn, false, false);
        expect (quadIn > 0.0f && quadIn < 1.0f, "Quad in should be in range");
        expect (quadIn < 0.5f, "Quad in should accelerate (< 0.5)");

        float quadOut = gin::ModMatrix::shape (0.5f, gin::ModMatrix::quadraticOut, false, false);
        expect (quadOut > 0.5f, "Quad out should decelerate (> 0.5)");

        // Test exponential shaping
        float expIn = gin::ModMatrix::shape (0.5f, gin::ModMatrix::exponentialIn, false, false);
        expect (expIn > 0.0f && expIn < 1.0f, "Exp in should be in range");

        // Test sine shaping
        float sineIn = gin::ModMatrix::shape (0.5f, gin::ModMatrix::sineIn, false, false);
        expect (sineIn > 0.0f && sineIn < 1.0f, "Sine in should be in range");
    }

    void testModulationEnable()
    {
        beginTest ("Modulation Enable/Disable");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "param", "Param", "P", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f);

        auto lfo = modMatrix.addMonoModSource ("lfo", "LFO", false);
        
        modMatrix.build();
        
        gin::ModDstId dst (param->getModIndex());

        param->setValue (0.5f);
        modMatrix.setModDepth (lfo, dst, 0.3f);
        modMatrix.setMonoValue (lfo, 1.0f);

        // Disable modulation
        modMatrix.setModEnable (lfo, dst, false);
        expect (!modMatrix.getModEnable (lfo, dst), "Should be disabled");

        float valueDisabled = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (valueDisabled, 50.0f, 0.1f, "Should be base value when disabled");

        // Enable modulation
        modMatrix.setModEnable (lfo, dst, true);
        expect (modMatrix.getModEnable (lfo, dst), "Should be enabled");

        float valueEnabled = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (valueEnabled, 80.0f, 0.1f, "Should be modulated when enabled");

        delete param;
    }

    void testBipolarModulation()
    {
        beginTest ("Bipolar Modulation");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "param", "Param", "P", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f);

        // Bipolar source (-1 to +1)
        auto lfo = modMatrix.addMonoModSource ("lfo", "LFO", true);
        
        modMatrix.build();
        
        gin::ModDstId dst (param->getModIndex());

        param->setValue (0.5f);
        modMatrix.setModDepth (lfo, dst, 0.2f);
        modMatrix.setModEnable (lfo, dst, true);
        modMatrix.setModBipolarMapping (lfo, dst, true);

        // Negative modulation
        modMatrix.setMonoValue (lfo, -1.0f);
        float valueNeg = modMatrix.getValue (param, false);
        expect (valueNeg < 50.0f, "Negative bipolar mod should decrease value");

        // Positive modulation
        modMatrix.setMonoValue (lfo, 1.0f);
        float valuePos = modMatrix.getValue (param, false);
        expect (valuePos > 50.0f, "Positive bipolar mod should increase value");

        // Zero modulation
        modMatrix.setMonoValue (lfo, 0.0f);
        float valueZero = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (valueZero, 50.0f, 0.1f, "Zero bipolar mod should not change value");

        delete param;
    }

    void testMultipleSourcesOneDestination()
    {
        beginTest ("Multiple Sources to One Destination");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "param", "Param", "P", "",
                                          0.0f, 100.0f, 1.0f, 50.0f);

        modMatrix.addParameter (param, false, 0.0f);

        auto lfo1 = modMatrix.addMonoModSource ("lfo1", "LFO 1", false);
        auto lfo2 = modMatrix.addMonoModSource ("lfo2", "LFO 2", false);
        
        modMatrix.build();

        gin::ModDstId dst (param->getModIndex());

        param->setValue (0.5f); // Base = 50.0

        // LFO1: depth 0.2, value 1.0 -> adds 0.2 normalized (20.0 user)
        modMatrix.setModDepth (lfo1, dst, 0.2f);
        modMatrix.setModEnable (lfo1, dst, true);
        modMatrix.setMonoValue (lfo1, 1.0f);

        // LFO2: depth 0.1, value 1.0 -> adds 0.1 normalized (10.0 user)
        modMatrix.setModDepth (lfo2, dst, 0.1f);
        modMatrix.setModEnable (lfo2, dst, true);
        modMatrix.setMonoValue (lfo2, 1.0f);

        // Total: 50.0 + 20.0 + 10.0 = 80.0
        float value = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (value, 80.0f, 0.1f, "Should sum both modulations");

        delete param;
    }

    void testValueClamping()
    {
        beginTest ("Value Clamping");

        MockProcessorForModMatrixTests mockProc;
        gin::ModMatrix modMatrix;
        modMatrix.setSampleRate (44100.0);

        auto* param = new gin::Parameter (mockProc, "param", "Param", "P", "",
                                          0.0f, 100.0f, 1.0f, 90.0f);

        modMatrix.addParameter (param, false, 0.0f);

        auto lfo = modMatrix.addMonoModSource ("lfo", "LFO", false);
        
        modMatrix.build();
        
        gin::ModDstId dst (param->getModIndex());

        param->setValue (0.9f); // 90.0
        modMatrix.setModDepth (lfo, dst, 0.5f);
        modMatrix.setModEnable (lfo, dst, true);
        modMatrix.setMonoValue (lfo, 1.0f);

        // Would be 90.0 + 50.0 = 140.0, but should clamp to 100.0
        float value = modMatrix.getValue (param, false);
        expectWithinAbsoluteError (value, 100.0f, 0.1f, "Should clamp to maximum");

        delete param;
    }
};

static ModMatrixTests modMatrixTests;

#endif

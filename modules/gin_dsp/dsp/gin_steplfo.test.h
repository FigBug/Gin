/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class StepLFOTests : public juce::UnitTest
{
public:
    StepLFOTests() : juce::UnitTest ("StepLFO", "gin_dsp") {}

    void runTest() override
    {
        testSteppedValues();
        testWrapAround();
        testStoppedWhenFreqZero();
        testOutputClamped();
    }

private:
    // Configure the LFO so that each call to process (1) advances exactly
    // one step: step = (freq / sr) / numPoints, so freq == sr gives 1/numPoints.
    StepLFO makeLFO (const juce::Array<float>& values)
    {
        StepLFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setNumPoints (values.size());
        for (int i = 0; i < values.size(); i++)
            lfo.setPoint (i, values[i]);
        lfo.setFreq (1000.0f);
        return lfo;
    }

    void testSteppedValues()
    {
        beginTest ("Stepped Values");

        juce::Array<float> values { 1.0f, 0.5f, -0.5f, -1.0f };
        auto lfo = makeLFO (values);

        lfo.noteOn();

        // process (0) leaves the phase at the start, reading the first point
        expectWithinAbsoluteError (lfo.process (0), 1.0f, 1.0e-6f, "First step");
        expectWithinAbsoluteError (lfo.process (1), 0.5f, 1.0e-6f, "Second step");
        expectWithinAbsoluteError (lfo.process (1), -0.5f, 1.0e-6f, "Third step");
        expectWithinAbsoluteError (lfo.process (1), -1.0f, 1.0e-6f, "Fourth step");
    }

    void testWrapAround()
    {
        beginTest ("Wrap Around");

        juce::Array<float> values { 1.0f, 0.5f, -0.5f, -1.0f };
        auto lfo = makeLFO (values);

        lfo.noteOn();

        // Advance a full cycle (4 steps) and confirm we are back to the start
        lfo.process (1); // step 1
        lfo.process (1); // step 2
        lfo.process (1); // step 3
        expectWithinAbsoluteError (lfo.process (1), 1.0f, 1.0e-6f, "Wrapped back to first step");
    }

    void testStoppedWhenFreqZero()
    {
        beginTest ("Zero Frequency Holds Step");

        juce::Array<float> values { 0.25f, 0.75f };
        StepLFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setNumPoints (values.size());
        lfo.setPoint (0, values[0]);
        lfo.setPoint (1, values[1]);
        lfo.setFreq (0.0f);

        lfo.noteOn();

        // With no frequency the phase never advances
        expectWithinAbsoluteError (lfo.process (128), 0.25f, 1.0e-6f, "Held on first step");
        expectWithinAbsoluteError (lfo.process (128), 0.25f, 1.0e-6f, "Still held on first step");
    }

    void testOutputClamped()
    {
        beginTest ("Output Clamped");

        juce::Array<float> values { 5.0f, -5.0f };
        auto lfo = makeLFO (values);
        lfo.noteOn();

        lfo.process (0);
        expect (lfo.getOutput() <= 1.0f && lfo.getOutput() >= -1.0f, "Output should be clamped to [-1, 1]");
    }
};

static StepLFOTests stepLFOTests;

#endif

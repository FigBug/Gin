/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ValueSmootherTests : public juce::UnitTest
{
public:
    ValueSmootherTests() : juce::UnitTest ("Value Smoother", "gin_dsp") {}

    void runTest() override
    {
        testBasicSmoothing();
        testSmoothingTime();
        testGetNextValue();
        testProcessMethod();
        testIsSmoothing();
        testSnapToValue();
        testSetValueUnsmoothed();
        testSampleRateChange();
        testBidirectionalSmoothing();
        testAlreadyAtTarget();
        testZeroInitialization();
    }

private:
    void testBasicSmoothing()
    {
        beginTest ("Basic Smoothing");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0); // 1000 samples per second
        smoother.setTime (0.1); // 100ms smoothing time

        // Start at 0, target 1
        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        expect (smoother.isSmoothing(), "Should be smoothing");

        // The delta should be 1.0 / (1000 * 0.1) = 0.01 per sample
        float firstValue = smoother.getNextValue();
        expectWithinAbsoluteError (firstValue, 0.01f, 0.0001f, "First step should be delta");

        float secondValue = smoother.getNextValue();
        expectWithinAbsoluteError (secondValue, 0.02f, 0.0001f, "Second step should be 2*delta");
    }

    void testSmoothingTime()
    {
        beginTest ("Smoothing Time Accuracy");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (44100.0);
        smoother.setTime (0.1); // 100ms = 4410 samples at 44100 Hz

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // After approx 4410 samples, should reach 1.0
        for (int i = 0; i < 4411; i++)
            smoother.getNextValue();

        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target after specified time");
        expect (!smoother.isSmoothing(), "Should not be smoothing anymore");
    }

    void testGetNextValue()
    {
        beginTest ("Get Next Value");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples to go from 0 to 1

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // Should take 10 steps of 0.1 each
        for (int i = 1; i <= 10; i++)
        {
            float expected = i * 0.1f;
            float actual = smoother.getNextValue();
            expectWithinAbsoluteError (actual, expected, 0.001f, "Step " + juce::String(i) + " should be correct");
        }

        // Should now be at target
        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should be at target");
        expect (!smoother.isSmoothing(), "Should not be smoothing");

        // Further calls should stay at target
        expectWithinAbsoluteError (smoother.getNextValue(), 1.0f, 0.001f, "Should stay at target");
    }

    void testProcessMethod()
    {
        beginTest ("Process Method");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // Process 5 samples
        smoother.process (5);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.5f, 0.001f, "Should be halfway after 5 samples");

        // Process 5 more samples
        smoother.process (5);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target after 10 total samples");
        expect (!smoother.isSmoothing(), "Should not be smoothing");
    }

    void testIsSmoothing()
    {
        beginTest ("Is Smoothing State");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1);

        smoother.setValueUnsmoothed (0.0f);
        expect (!smoother.isSmoothing(), "Should not be smoothing when at target");

        smoother.setValue (1.0f);
        expect (smoother.isSmoothing(), "Should be smoothing after setting new target");

        // Process all the way to target
        smoother.process (100);
        expect (!smoother.isSmoothing(), "Should not be smoothing after reaching target");
    }

    void testSnapToValue()
    {
        beginTest ("Snap To Value");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1);

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        expect (smoother.isSmoothing(), "Should be smoothing");

        // Snap to target
        smoother.snapToValue();

        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.0001f, "Should be at target after snap");
        expect (!smoother.isSmoothing(), "Should not be smoothing after snap");
    }

    void testSetValueUnsmoothed()
    {
        beginTest ("Set Value Unsmoothed");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1);

        // Start smoothing
        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);
        smoother.getNextValue(); // Advance one step

        expect (smoother.isSmoothing(), "Should be smoothing");

        // Jump to new value without smoothing
        smoother.setValueUnsmoothed (0.5f);

        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.5f, 0.0001f, "Should be at new value");
        expect (!smoother.isSmoothing(), "Should not be smoothing");

        // getNextValue should stay at 0.5
        expectWithinAbsoluteError (smoother.getNextValue(), 0.5f, 0.0001f, "Should stay at value");
    }

    void testSampleRateChange()
    {
        beginTest ("Sample Rate Change");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1); // delta = 0.01

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // First step at 1000 Hz
        float firstValue = smoother.getNextValue();
        expectWithinAbsoluteError (firstValue, 0.01f, 0.0001f, "Step size at 1000 Hz");

        // Change sample rate
        smoother.setSampleRate (2000.0); // delta should now be 0.005

        float secondValue = smoother.getNextValue();
        // Current is 0.01, so next should be 0.01 + 0.005 = 0.015
        expectWithinAbsoluteError (secondValue, 0.015f, 0.0001f, "Step size should change with sample rate");
    }

    void testBidirectionalSmoothing()
    {
        beginTest ("Bidirectional Smoothing");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples, delta = 0.1

        // Start at 0, go to 1
        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);
        smoother.process (5);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.5f, 0.001f, "Should be at 0.5");

        // Change direction: go back to 0
        smoother.setValue (0.0f);
        expect (smoother.isSmoothing(), "Should be smoothing downward");

        smoother.process (5);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.0f, 0.001f, "Should reach 0");

        // Go back up to 0.3
        smoother.setValue (0.3f);
        smoother.process (3);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.3f, 0.001f, "Should reach 0.3");
    }

    void testAlreadyAtTarget()
    {
        beginTest ("Already At Target");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1);

        smoother.setValueUnsmoothed (0.5f);
        smoother.setValue (0.5f);

        expect (!smoother.isSmoothing(), "Should not be smoothing when already at target");

        float value = smoother.getNextValue();
        expectWithinAbsoluteError (value, 0.5f, 0.0001f, "Should stay at target");

        smoother.process (10);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.5f, 0.0001f, "Should still be at target after process");
    }

    void testZeroInitialization()
    {
        beginTest ("Zero Initialization");

        ValueSmoother<float> smoother;
        smoother.setSampleRate (44100.0);
        smoother.setTime (0.05);

        // Default initialization should be 0
        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.0f, 0.0001f, "Should start at 0");
        expect (!smoother.isSmoothing(), "Should not be smoothing initially");

        // Set to non-zero target
        smoother.setValue (0.8f);
        expect (smoother.isSmoothing(), "Should start smoothing");

        // Process some samples
        for (int i = 0; i < 100; i++)
            smoother.getNextValue();

        expect (smoother.getCurrentValue() > 0.0f, "Should have moved from zero");
        expect (smoother.getCurrentValue() < 0.8f || !smoother.isSmoothing(), "Should be moving toward or at target");
    }
};

static ValueSmootherTests valueSmootherTests;

#endif

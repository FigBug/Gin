/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class EasedValueSmootherTests : public juce::UnitTest
{
public:
    EasedValueSmootherTests() : juce::UnitTest ("Eased Value Smoother", "gin_dsp") {}

    void runTest() override
    {
        testBasicSmoothing();
        testDifferentEasingFunctions();
        testSmoothingTime();
        testGetNextValue();
        testProcessMethod();
        testIsSmoothing();
        testSnapToValue();
        testSetValueUnsmoothed();
        testEasingCurveDifference();
        testZeroInitialization();
        testBidirectionalSmoothing();
    }

private:
    void testBasicSmoothing()
    {
        beginTest ("Basic Smoothing with Default Easing");

        EasedValueSmoother<float> smoother; // Uses QuadraticInOutEasing by default
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        expect (smoother.isSmoothing(), "Should be smoothing");

        // Process through smoothing - may take up to 11 steps due to floating point
        int steps = 0;
        while (smoother.isSmoothing() && steps < 12)
        {
            smoother.getNextValue();
            steps++;
        }

        expect (steps >= 10 && steps <= 11, "Should complete in 10-11 steps");
        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target");
        expect (!smoother.isSmoothing(), "Should not be smoothing anymore");
    }

    void testDifferentEasingFunctions()
    {
        beginTest ("Different Easing Functions");

        // Test Linear easing
        EasedValueSmoother<float, LinearEasing> linearSmoother;
        linearSmoother.setSampleRate (100.0);
        linearSmoother.setTime (0.1);
        linearSmoother.setValueUnsmoothed (0.0f);
        linearSmoother.setValue (1.0f);

        // Test Elastic easing
        EasedValueSmoother<float, ElasticOutEasing> elasticSmoother;
        elasticSmoother.setSampleRate (100.0);
        elasticSmoother.setTime (0.1);
        elasticSmoother.setValueUnsmoothed (0.0f);
        elasticSmoother.setValue (1.0f);

        // Test Exponential easing
        EasedValueSmoother<float, ExponentialInOutEasing> expSmoother;
        expSmoother.setSampleRate (100.0);
        expSmoother.setTime (0.1);
        expSmoother.setValueUnsmoothed (0.0f);
        expSmoother.setValue (1.0f);

        // Get first step from each
        float linearFirst = linearSmoother.getNextValue();
        float elasticFirst = elasticSmoother.getNextValue();
        float expFirst = expSmoother.getNextValue();

        // Linear should be around 0.1 (simple linear interpolation)
        expectWithinAbsoluteError (linearFirst, 0.1f, 0.02f, "Linear should be close to 0.1");

        // Exponential should be much smaller initially
        expect (expFirst < linearFirst, "Exponential should start slower than linear");

        // All should eventually reach target
        while (linearSmoother.isSmoothing()) linearSmoother.getNextValue();
        while (elasticSmoother.isSmoothing()) elasticSmoother.getNextValue();
        while (expSmoother.isSmoothing()) expSmoother.getNextValue();

        expectWithinAbsoluteError (linearSmoother.getCurrentValue(), 1.0f, 0.001f, "Linear should reach target");
        expectWithinAbsoluteError (elasticSmoother.getCurrentValue(), 1.0f, 0.001f, "Elastic should reach target");
        expectWithinAbsoluteError (expSmoother.getCurrentValue(), 1.0f, 0.001f, "Exponential should reach target");
    }

    void testSmoothingTime()
    {
        beginTest ("Smoothing Time Accuracy");

        EasedValueSmoother<float> smoother;
        smoother.setSampleRate (44100.0);
        smoother.setTime (0.1); // 100ms = 4410 samples

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // Process until done - allow one extra step for floating point
        int steps = 0;
        while (smoother.isSmoothing() && steps < 4412)
        {
            smoother.getNextValue();
            steps++;
        }

        expect (steps >= 4410 && steps <= 4411, "Should complete in approximately 4410-4411 steps");
        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target");
    }

    void testGetNextValue()
    {
        beginTest ("Get Next Value");

        EasedValueSmoother<float, QuadraticInOutEasing> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.2); // 20 samples

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        float lastValue = 0.0f;
        int steps = 0;

        // Values should be monotonically increasing
        while (smoother.isSmoothing() && steps < 22)
        {
            float newValue = smoother.getNextValue();
            expect (newValue >= lastValue, "Values should increase or stay same");
            lastValue = newValue;
            steps++;
        }

        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target");
    }

    void testProcessMethod()
    {
        beginTest ("Process Method");

        EasedValueSmoother<float> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples

        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);

        // Process half the samples
        smoother.process (5);
        float halfwayValue = smoother.getCurrentValue();

        expect (halfwayValue > 0.0f && halfwayValue < 1.0f, "Should be between start and target");

        // Process remaining samples (allow extra for floating point)
        smoother.process (6);
        expectWithinAbsoluteError (smoother.getCurrentValue(), 1.0f, 0.001f, "Should reach target");
        expect (!smoother.isSmoothing(), "Should not be smoothing");
    }

    void testIsSmoothing()
    {
        beginTest ("Is Smoothing State");

        EasedValueSmoother<float> smoother;
        smoother.setSampleRate (1000.0);
        smoother.setTime (0.1);

        smoother.setValueUnsmoothed (0.0f);
        expect (!smoother.isSmoothing(), "Should not be smoothing when at target");

        smoother.setValue (1.0f);
        expect (smoother.isSmoothing(), "Should be smoothing after setting new target");

        // Process to completion
        int steps = 0;
        while (smoother.isSmoothing() && steps < 102)
        {
            smoother.getNextValue();
            steps++;
        }

        expect (!smoother.isSmoothing(), "Should not be smoothing after reaching target");
    }

    void testSnapToValue()
    {
        beginTest ("Snap To Value");

        EasedValueSmoother<float> smoother;
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

        EasedValueSmoother<float> smoother;
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

    void testEasingCurveDifference()
    {
        beginTest ("Easing Curve Produces Different Results");

        // Compare linear vs eased at the halfway point
        EasedValueSmoother<float, LinearEasing> linearSmoother;
        EasedValueSmoother<float, QuadraticInOutEasing> easedSmoother;

        linearSmoother.setSampleRate (100.0);
        easedSmoother.setSampleRate (100.0);

        linearSmoother.setTime (0.2); // 20 samples
        easedSmoother.setTime (0.2);

        linearSmoother.setValueUnsmoothed (0.0f);
        easedSmoother.setValueUnsmoothed (0.0f);

        linearSmoother.setValue (1.0f);
        easedSmoother.setValue (1.0f);

        // Process 10 samples (halfway)
        linearSmoother.process (10);
        easedSmoother.process (10);

        float linearHalf = linearSmoother.getCurrentValue();
        float easedHalf = easedSmoother.getCurrentValue();

        // Linear should be close to 0.5 at halfway
        expectWithinAbsoluteError (linearHalf, 0.5f, 0.05f, "Linear should be near 0.5 at halfway");

        // QuadraticInOut should also be near 0.5 at halfway (symmetric), but not exactly the same
        expectWithinAbsoluteError (easedHalf, 0.5f, 0.15f, "Eased should be near 0.5 at halfway");

        // Test asymmetric easing (QuadraticIn)
        EasedValueSmoother<float, QuadraticInEasing> quadInSmoother;
        quadInSmoother.setSampleRate (100.0);
        quadInSmoother.setTime (0.2);
        quadInSmoother.setValueUnsmoothed (0.0f);
        quadInSmoother.setValue (1.0f);
        quadInSmoother.process (10);

        float quadInHalf = quadInSmoother.getCurrentValue();

        // QuadraticIn should be less than 0.5 at halfway (slow start)
        expect (quadInHalf < 0.5f, "QuadraticIn should be less than 0.5 at halfway");
    }

    void testZeroInitialization()
    {
        beginTest ("Zero Initialization");

        EasedValueSmoother<float> smoother;
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
    }

    void testBidirectionalSmoothing()
    {
        beginTest ("Bidirectional Smoothing");

        EasedValueSmoother<float, CubicInOutEasing> smoother;
        smoother.setSampleRate (100.0);
        smoother.setTime (0.1); // 10 samples

        // Start at 0, go to 1
        smoother.setValueUnsmoothed (0.0f);
        smoother.setValue (1.0f);
        smoother.process (5);

        float midValue = smoother.getCurrentValue();
        expect (midValue > 0.0f && midValue < 1.0f, "Should be between 0 and 1");

        // Change direction: go back to 0
        smoother.setValue (0.0f);
        expect (smoother.isSmoothing(), "Should be smoothing downward");

        int steps = 0;
        while (smoother.isSmoothing() && steps < 12)
        {
            smoother.getNextValue();
            steps++;
        }

        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.0f, 0.001f, "Should reach 0");

        // Go to 0.7
        smoother.setValue (0.7f);
        steps = 0;
        while (smoother.isSmoothing() && steps < 12)
        {
            smoother.getNextValue();
            steps++;
        }

        expectWithinAbsoluteError (smoother.getCurrentValue(), 0.7f, 0.001f, "Should reach 0.7");
    }
};

static EasedValueSmootherTests easedValueSmootherTests;

#endif

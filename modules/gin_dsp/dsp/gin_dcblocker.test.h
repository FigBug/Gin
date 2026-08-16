/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DCBlockerTests : public juce::UnitTest
{
public:
    DCBlockerTests() : juce::UnitTest ("DCBlocker", "gin_dsp") {}

    void runTest() override
    {
        testDCEstimateConverges();
        testBlocksDCOffset();
        testZeroInput();
        testReset();
    }

private:
    // process() returns a low-pass estimate of the DC content; callers block DC
    // by computing (x - process(x)), as gin's oscillators do.

    void testDCEstimateConverges()
    {
        beginTest ("DC Estimate Converges To Input");

        DCBlocker blocker;
        blocker.setSampleRate (44100.0f);
        blocker.setCutoff (10.0f);

        const float dc = 0.5f;
        float estimate = 0.0f;
        for (int i = 0; i < 40000; i++)
            estimate = blocker.process (dc);

        expectWithinAbsoluteError (estimate, dc, 0.01f, "Estimate should track the DC level");
    }

    void testBlocksDCOffset()
    {
        beginTest ("Blocks DC Offset");

        DCBlocker blocker;
        blocker.setSampleRate (44100.0f);
        blocker.setCutoff (10.0f);

        const float dc = 0.75f;
        float blocked = dc;
        for (int i = 0; i < 40000; i++)
            blocked = dc - blocker.process (dc); // high-pass = input - low-pass

        expect (std::abs (blocked) < 0.01f, "DC offset should be removed");
    }

    void testZeroInput()
    {
        beginTest ("Zero Input Stays Zero");

        DCBlocker blocker;
        blocker.setSampleRate (44100.0f);
        blocker.setCutoff (10.0f);

        for (int i = 0; i < 100; i++)
            expectWithinAbsoluteError (blocker.process (0.0f), 0.0f, 1.0e-9f, "Zero in, zero out");
    }

    void testReset()
    {
        beginTest ("Reset Clears State");

        DCBlocker blocker;
        blocker.setSampleRate (44100.0f);
        blocker.setCutoff (10.0f);

        for (int i = 0; i < 1000; i++)
            blocker.process (1.0f);

        blocker.reset();

        // After reset the internal state is cleared, so the first output is 0
        expectWithinAbsoluteError (blocker.process (0.0f), 0.0f, 1.0e-9f, "State cleared after reset");
    }
};

static DCBlockerTests dcBlockerTests;

#endif

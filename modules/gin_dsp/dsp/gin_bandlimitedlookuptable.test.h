/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class BandLimitedOscillatorTests : public juce::UnitTest
{
public:
    BandLimitedOscillatorTests() : juce::UnitTest ("Band Limited Oscillators", "gin_dsp") {}

    void runTest() override
    {
        testSine();
        testSawUpDownAreOpposite();
        testTriangleShape();
        testPulseMatchesSquare();
        testOutputBounded();
    }

private:
    void testSine()
    {
        beginTest ("Sine");

        expectWithinAbsoluteError (sine (0.0f),  0.0f, 1.0e-6f, "sine(0)");
        expectWithinAbsoluteError (sine (0.25f), 1.0f, 1.0e-6f, "sine(0.25)");
        expectWithinAbsoluteError (sine (0.5f),  0.0f, 1.0e-6f, "sine(0.5)");
        expectWithinAbsoluteError (sine (0.75f), -1.0f, 1.0e-6f, "sine(0.75)");
    }

    void testSawUpDownAreOpposite()
    {
        beginTest ("Saw Up Is Negated Saw Down");

        const float freq = 100.0f, sr = 44100.0f;
        for (float p = 0.0f; p < 1.0f; p += 0.1f)
            expectWithinAbsoluteError (sawUp (p, freq, sr), -sawDown (p, freq, sr), 1.0e-5f, "sawUp == -sawDown");
    }

    void testTriangleShape()
    {
        beginTest ("Triangle Shape");

        const float freq = 100.0f, sr = 44100.0f;
        expectWithinAbsoluteError (triangle (0.0f, freq, sr), 0.0f, 1.0e-4f, "triangle(0) is zero crossing");
        expectWithinAbsoluteError (triangle (0.25f, freq, sr), 1.0f, 0.05f, "triangle(0.25) near peak");
        expectWithinAbsoluteError (triangle (0.75f, freq, sr), -1.0f, 0.05f, "triangle(0.75) near trough");
    }

    void testPulseMatchesSquare()
    {
        beginTest ("50% Pulse Matches Square");

        const float freq = 100.0f, sr = 44100.0f;
        for (float p = 0.0f; p < 1.0f; p += 0.1f)
            expectWithinAbsoluteError (pulse (p, 0.5f, freq, sr), squareWave (p, freq, sr), 1.0e-5f, "pulse(0.5) == square");
    }

    void testOutputBounded()
    {
        beginTest ("Output Bounded");

        const float freq = 100.0f, sr = 44100.0f;
        // Band-limited waves ring slightly past +/-1 (Gibbs), so allow a small margin
        for (float p = 0.0f; p < 1.0f; p += 0.01f)
        {
            expect (std::abs (sawUp (p, freq, sr)) < 1.3f, "sawUp bounded");
            expect (std::abs (triangle (p, freq, sr)) < 1.3f, "triangle bounded");
            expect (std::abs (squareWave (p, freq, sr)) < 1.3f, "square bounded");
        }
    }
};

static BandLimitedOscillatorTests bandLimitedOscillatorTests;

#endif

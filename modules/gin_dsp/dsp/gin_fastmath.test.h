/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class FastMathTests : public juce::UnitTest
{
public:
    FastMathTests() : juce::UnitTest ("Fast Math", "gin_dsp") {}

    void runTest() override
    {
        testFastSinBoundaries();
        testFastSinAccuracy();
        testFastTanhBoundaries();
        testFastTanhAccuracy();
        testFastTanhSymmetry();
    }

private:
    void testFastSinBoundaries()
    {
        beginTest ("Fast Sin Boundaries");

        // Test at key points
        float atZero = FastMath<float>::fastSin (0.0f);
        expectWithinAbsoluteError (atZero, 0.0f, 0.01f, "sin(0) should be ~0");

        float atPi = FastMath<float>::fastSin (juce::MathConstants<float>::pi);
        expectWithinAbsoluteError (atPi, 0.0f, 0.01f, "sin(pi) should be ~0");

        float atNegPi = FastMath<float>::fastSin (-juce::MathConstants<float>::pi);
        expectWithinAbsoluteError (atNegPi, 0.0f, 0.01f, "sin(-pi) should be ~0");

        float atHalfPi = FastMath<float>::fastSin (juce::MathConstants<float>::halfPi);
        expectWithinAbsoluteError (atHalfPi, 1.0f, 0.01f, "sin(pi/2) should be ~1");

        float atNegHalfPi = FastMath<float>::fastSin (-juce::MathConstants<float>::halfPi);
        expectWithinAbsoluteError (atNegHalfPi, -1.0f, 0.01f, "sin(-pi/2) should be ~-1");
    }

    void testFastSinAccuracy()
    {
        beginTest ("Fast Sin Accuracy");

        // Test against std::sin at various points
        const int numTests = 20;
        float maxError = 0.0f;

        for (int i = 0; i < numTests; i++)
        {
            float x = -juce::MathConstants<float>::pi + (2.0f * juce::MathConstants<float>::pi * i / (numTests - 1));
            float fast = FastMath<float>::fastSin (x);
            float accurate = std::sin (x);
            float error = std::abs (fast - accurate);
            maxError = std::max (maxError, error);
        }

        expect (maxError < 0.01f, "Maximum error should be less than 1%");
    }

    void testFastTanhBoundaries()
    {
        beginTest ("Fast Tanh Boundaries");

        float atZero = FastMath<float>::fastTanh (0.0f);
        expectWithinAbsoluteError (atZero, 0.0f, 0.01f, "tanh(0) should be ~0");

        // Large positive should approach 1
        float atLarge = FastMath<float>::fastTanh (10.0f);
        expect (atLarge > 0.99f && atLarge <= 1.0f, "tanh(10) should be close to 1");

        // Large negative should approach -1
        float atNegLarge = FastMath<float>::fastTanh (-10.0f);
        expect (atNegLarge < -0.99f && atNegLarge >= -1.0f, "tanh(-10) should be close to -1");
    }

    void testFastTanhAccuracy()
    {
        beginTest ("Fast Tanh Accuracy");

        // Test against std::tanh at various points
        float maxError = 0.0f;

        for (float x = -5.0f; x <= 5.0f; x += 0.5f)
        {
            float fast = FastMath<float>::fastTanh (x);
            float accurate = std::tanh (x);
            float error = std::abs (fast - accurate);
            maxError = std::max (maxError, error);
        }

        expect (maxError < 0.05f, "Maximum error should be less than 5%");
    }

    void testFastTanhSymmetry()
    {
        beginTest ("Fast Tanh Symmetry");

        // tanh is an odd function: tanh(-x) = -tanh(x)
        for (float x = 0.1f; x < 5.0f; x += 0.5f)
        {
            float pos = FastMath<float>::fastTanh (x);
            float neg = FastMath<float>::fastTanh (-x);
            expectWithinAbsoluteError (neg, -pos, 0.0001f, "tanh should be symmetric");
        }
    }
};

static FastMathTests fastMathTests;

#endif

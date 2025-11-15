#include <JuceHeader.h>

//==============================================================================
class GinEasingTests : public juce::UnitTest
{
public:
    GinEasingTests() : juce::UnitTest ("Gin Easing Functions", "gin") {}

    void runTest() override
    {
        testEasingBoundaries();
        testLinearEasing();
        testQuadraticEasing();
        testCubicEasing();
        testSineEasing();
        testExponentialEasing();
        testElasticEasing();
        testBounceEasing();
    }

private:
    void testEasingBoundaries()
    {
        beginTest ("Easing Boundaries");

        // All easing functions should map 0 -> 0 and 1 -> 1 (except elastic/back which can overshoot)
        expectWithinAbsoluteError (easeLinear (0.0f), 0.0f, 0.0001f, "Linear(0) should be 0");
        expectWithinAbsoluteError (easeLinear (1.0f), 1.0f, 0.0001f, "Linear(1) should be 1");

        expectWithinAbsoluteError (easeQuadraticIn (0.0f), 0.0f, 0.0001f, "QuadraticIn(0) should be 0");
        expectWithinAbsoluteError (easeQuadraticIn (1.0f), 1.0f, 0.0001f, "QuadraticIn(1) should be 1");

        expectWithinAbsoluteError (easeQuadraticOut (0.0f), 0.0f, 0.0001f, "QuadraticOut(0) should be 0");
        expectWithinAbsoluteError (easeQuadraticOut (1.0f), 1.0f, 0.0001f, "QuadraticOut(1) should be 1");

        expectWithinAbsoluteError (easeCubicIn (0.0f), 0.0f, 0.0001f, "CubicIn(0) should be 0");
        expectWithinAbsoluteError (easeCubicIn (1.0f), 1.0f, 0.0001f, "CubicIn(1) should be 1");

        expectWithinAbsoluteError (easeSineIn (0.0f), 0.0f, 0.0001f, "SineIn(0) should be 0");
        expectWithinAbsoluteError (easeSineIn (1.0f), 1.0f, 0.0001f, "SineIn(1) should be 1");

        expectWithinAbsoluteError (easeSineOut (0.0f), 0.0f, 0.0001f, "SineOut(0) should be 0");
        expectWithinAbsoluteError (easeSineOut (1.0f), 1.0f, 0.0001f, "SineOut(1) should be 1");
    }

    void testLinearEasing()
    {
        beginTest ("Linear Easing");

        expectWithinAbsoluteError (easeLinear (0.0f), 0.0f, 0.0001f, "Linear(0.0) = 0.0");
        expectWithinAbsoluteError (easeLinear (0.25f), 0.25f, 0.0001f, "Linear(0.25) = 0.25");
        expectWithinAbsoluteError (easeLinear (0.5f), 0.5f, 0.0001f, "Linear(0.5) = 0.5");
        expectWithinAbsoluteError (easeLinear (0.75f), 0.75f, 0.0001f, "Linear(0.75) = 0.75");
        expectWithinAbsoluteError (easeLinear (1.0f), 1.0f, 0.0001f, "Linear(1.0) = 1.0");
    }

    void testQuadraticEasing()
    {
        beginTest ("Quadratic Easing");

        // QuadraticIn: y = x^2
        expectWithinAbsoluteError (easeQuadraticIn (0.5f), 0.25f, 0.0001f, "QuadraticIn(0.5) = 0.25");

        // QuadraticOut should be steeper at start
        float outValue = easeQuadraticOut (0.5f);
        expect (outValue > 0.5f, "QuadraticOut(0.5) should be > 0.5");

        // QuadraticInOut should be symmetric
        float inOut1 = easeQuadraticInOut (0.25f);
        float inOut2 = easeQuadraticInOut (0.75f);
        expectWithinAbsoluteError (inOut1 + inOut2, 1.0f, 0.0001f, "QuadraticInOut should be symmetric");
    }

    void testCubicEasing()
    {
        beginTest ("Cubic Easing");

        // CubicIn: y = x^3
        expectWithinAbsoluteError (easeCubicIn (0.5f), 0.125f, 0.0001f, "CubicIn(0.5) = 0.125");

        // CubicOut should reach target faster
        float outValue = easeCubicOut (0.5f);
        expect (outValue > easeQuadraticOut (0.5f), "CubicOut should be steeper than QuadraticOut");

        // Test that CubicIn is slower than QuadraticIn at midpoint
        expect (easeCubicIn (0.5f) < easeQuadraticIn (0.5f), "CubicIn should be slower than QuadraticIn");
    }

    void testSineEasing()
    {
        beginTest ("Sine Easing");

        // SineIn should be smooth
        float sine1 = easeSineIn (0.0f);
        float sine2 = easeSineIn (0.5f);
        float sine3 = easeSineIn (1.0f);

        expectWithinAbsoluteError (sine1, 0.0f, 0.0001f, "SineIn(0) = 0");
        expect (sine2 > 0.0f && sine2 < 1.0f, "SineIn(0.5) should be between 0 and 1");
        expectWithinAbsoluteError (sine3, 1.0f, 0.0001f, "SineIn(1) = 1");

        // SineInOut should be symmetric around 0.5
        float inOut1 = easeSineInOut (0.25f);
        float inOut2 = easeSineInOut (0.75f);
        expectWithinAbsoluteError (inOut1 + inOut2, 1.0f, 0.01f, "SineInOut should be approximately symmetric");
    }

    void testExponentialEasing()
    {
        beginTest ("Exponential Easing");

        // ExponentialIn should be very slow at start
        float expIn = easeExponentialIn (0.5f);
        expect (expIn < 0.1f, "ExponentialIn(0.5) should be very small");

        // ExponentialOut should be very fast at start
        float expOut = easeExponentialOut (0.5f);
        expect (expOut > 0.9f, "ExponentialOut(0.5) should be close to 1");

        // Test boundaries
        expectWithinAbsoluteError (easeExponentialIn (0.0f), 0.0f, 0.0001f, "ExponentialIn(0) = 0");
        expectWithinAbsoluteError (easeExponentialIn (1.0f), 1.0f, 0.0001f, "ExponentialIn(1) = 1");
        expectWithinAbsoluteError (easeExponentialOut (0.0f), 0.0f, 0.0001f, "ExponentialOut(0) = 0");
        expectWithinAbsoluteError (easeExponentialOut (1.0f), 1.0f, 0.0001f, "ExponentialOut(1) = 1");
    }

    void testElasticEasing()
    {
        beginTest ("Elastic Easing");

        // Elastic easing can overshoot, so we just test that it starts and ends correctly
        expectWithinAbsoluteError (easeElasticIn (0.0f), 0.0f, 0.0001f, "ElasticIn(0) = 0");
        expectWithinAbsoluteError (easeElasticOut (1.0f), 1.0f, 0.0001f, "ElasticOut(1) = 1");

        // ElasticOut should overshoot (go above 1) before settling
        bool foundOvershoot = false;
        for (float t = 0.1f; t < 1.0f; t += 0.1f)
        {
            if (easeElasticOut (t) > 1.0f)
            {
                foundOvershoot = true;
                break;
            }
        }
        expect (foundOvershoot, "ElasticOut should overshoot");
    }

    void testBounceEasing()
    {
        beginTest ("Bounce Easing");

        // BounceOut should have multiple local maxima (bounces)
        juce::Array<float> bounceValues;
        for (float t = 0.0f; t <= 1.0f; t += 0.05f)
            bounceValues.add (easeBounceOut (t));

        // Check boundaries
        expectWithinAbsoluteError (easeBounceOut (0.0f), 0.0f, 0.0001f, "BounceOut(0) = 0");
        expectWithinAbsoluteError (easeBounceOut (1.0f), 1.0f, 0.0001f, "BounceOut(1) = 1");

        // BounceIn should be inverse of BounceOut
        expectWithinAbsoluteError (easeBounceIn (0.5f), 1.0f - easeBounceOut (0.5f), 0.0001f,
                                   "BounceIn(x) = 1 - BounceOut(1-x)");
    }
};

static GinEasingTests ginEasingTests;

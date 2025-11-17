/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LagrangeTests : public juce::UnitTest
{
public:
    LagrangeTests() : juce::UnitTest ("Lagrange Interpolation", "gin") {}

    void runTest() override
    {
        testLinearInterpolation();
        testQuadraticInterpolation();
        testExactPoints();
        testArrayOverload();
    }

private:
    void testLinearInterpolation()
    {
        beginTest ("Linear Interpolation");

        // Two points define a line
        juce::Array<Point<float>> points;
        points.add ({0.0f, 0.0f});
        points.add ({10.0f, 10.0f});

        // Midpoint should be 5
        float mid = Lagrange::interpolate (points, 5.0f);
        expectWithinAbsoluteError (mid, 5.0f, 0.001f, "Midpoint should be 5");

        // Quarter point
        float quarter = Lagrange::interpolate (points, 2.5f);
        expectWithinAbsoluteError (quarter, 2.5f, 0.001f, "Quarter point should be 2.5");
    }

    void testQuadraticInterpolation()
    {
        beginTest ("Quadratic Interpolation");

        // Three points defining y = x^2
        juce::Array<Point<float>> points;
        points.add ({0.0f, 0.0f});
        points.add ({1.0f, 1.0f});
        points.add ({2.0f, 4.0f});

        // Test at known points
        float at0_5 = Lagrange::interpolate (points, 0.5f);
        expectWithinAbsoluteError (at0_5, 0.25f, 0.001f, "0.5^2 should be 0.25");

        float at1_5 = Lagrange::interpolate (points, 1.5f);
        expectWithinAbsoluteError (at1_5, 2.25f, 0.001f, "1.5^2 should be 2.25");
    }

    void testExactPoints()
    {
        beginTest ("Exact Points");

        // Interpolation should pass through the given points exactly
        juce::Array<Point<float>> points;
        points.add ({1.0f, 3.0f});
        points.add ({2.0f, 7.0f});
        points.add ({4.0f, 2.0f});

        float at1 = Lagrange::interpolate (points, 1.0f);
        expectWithinAbsoluteError (at1, 3.0f, 0.001f, "Should pass through first point");

        float at2 = Lagrange::interpolate (points, 2.0f);
        expectWithinAbsoluteError (at2, 7.0f, 0.001f, "Should pass through second point");

        float at4 = Lagrange::interpolate (points, 4.0f);
        expectWithinAbsoluteError (at4, 2.0f, 0.001f, "Should pass through third point");
    }

    void testArrayOverload()
    {
        beginTest ("Array Overload");

        float xArr[] = {0.0f, 1.0f, 2.0f};
        float yArr[] = {0.0f, 1.0f, 4.0f}; // y = x^2

        float at0_5 = Lagrange::interpolate (xArr, yArr, 3, 0.5f);
        expectWithinAbsoluteError (at0_5, 0.25f, 0.001f, "Array overload should work");

        float at1_5 = Lagrange::interpolate (xArr, yArr, 3, 1.5f);
        expectWithinAbsoluteError (at1_5, 2.25f, 0.001f, "Array overload should work");
    }
};

static LagrangeTests lagrangeTests;

#endif

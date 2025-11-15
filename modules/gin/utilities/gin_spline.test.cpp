/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class SplineTests : public juce::UnitTest
{
public:
    SplineTests() : juce::UnitTest ("Spline Interpolation", "gin") {}

    void runTest() override
    {
        testLinearPoints();
        testCurvedData();
        testPassesThroughPoints();
        testMonotonic();
    }

private:
    void testLinearPoints()
    {
        beginTest ("Linear Points");

        // Points on a line y = 2x
        juce::Array<Point<double>> points;
        points.add ({0.0, 0.0});
        points.add ({1.0, 2.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 6.0});

        Spline spline (points);

        // Spline should interpolate linearly
        expectWithinAbsoluteError (spline.interpolate (1.5), 3.0, 0.1, "Mid-point should be 3.0");
        expectWithinAbsoluteError (spline.interpolate (2.5), 5.0, 0.1, "Mid-point should be 5.0");
    }

    void testCurvedData()
    {
        beginTest ("Curved Data");

        // Points on a curve
        juce::Array<Point<double>> points;
        points.add ({0.0, 0.0});
        points.add ({1.0, 1.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 9.0});

        Spline spline (points);

        // Should produce smooth interpolation
        double y1 = spline.interpolate (0.5);
        double y2 = spline.interpolate (1.5);

        expect (y1 > 0.0 && y1 < 1.0, "Interpolated value should be between endpoints");
        expect (y2 > 1.0 && y2 < 4.0, "Interpolated value should be between endpoints");
    }

    void testPassesThroughPoints()
    {
        beginTest ("Passes Through Points");

        juce::Array<Point<double>> points;
        points.add ({0.0, 5.0});
        points.add ({1.0, 3.0});
        points.add ({2.0, 8.0});
        points.add ({3.0, 2.0});

        Spline spline (points);

        // Spline should pass exactly through control points
        expectWithinAbsoluteError (spline.interpolate (0.0), 5.0, 0.01, "Should pass through (0, 5)");
        expectWithinAbsoluteError (spline.interpolate (1.0), 3.0, 0.01, "Should pass through (1, 3)");
        expectWithinAbsoluteError (spline.interpolate (2.0), 8.0, 0.01, "Should pass through (2, 8)");
        expectWithinAbsoluteError (spline.interpolate (3.0), 2.0, 0.01, "Should pass through (3, 2)");
    }

    void testMonotonic()
    {
        beginTest ("Monotonic Increasing");

        // Monotonically increasing points
        juce::Array<Point<double>> points;
        points.add ({0.0, 0.0});
        points.add ({1.0, 1.0});
        points.add ({2.0, 2.0});
        points.add ({3.0, 3.0});

        Spline spline (points);

        // Sample multiple points to ensure smoothness
        double prev = spline.interpolate (0.0);
        for (double x = 0.1; x <= 3.0; x += 0.1)
        {
            double curr = spline.interpolate (x);
            expect (curr >= prev - 0.01, "Should maintain monotonic trend");
            prev = curr;
        }
    }
};

static SplineTests splineTests;

#endif

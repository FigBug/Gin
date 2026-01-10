/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LinearRegressionTests : public juce::UnitTest
{
public:
    LinearRegressionTests() : juce::UnitTest ("Linear Regression", "gin") {}

    void runTest() override
    {
        testPerfectLine();
        testHorizontalLine();
        testNegativeSlope();
        testAddPoints();
        testClearData();
    }

private:
    void testPerfectLine()
    {
        beginTest ("Perfect Line");

        // y = 2x + 1
        juce::Array<Point<double>> points;
        points.add ({0.0, 1.0});
        points.add ({1.0, 3.0});
        points.add ({2.0, 5.0});
        points.add ({3.0, 7.0});
        points.add ({4.0, 9.0});

        LinearRegression lr (points);

        // Should fit y = 2x + 1 perfectly
        expectWithinAbsoluteError (lr.getA(), 1.0, 0.001, "Intercept should be 1.0");
        expectWithinAbsoluteError (lr.getB(), 2.0, 0.001, "Slope should be 2.0");
        expectWithinAbsoluteError (lr.getCoefCorrel(), 1.0, 0.001, "Correlation should be perfect");
        expectWithinAbsoluteError (lr.getCoefDeterm(), 1.0, 0.001, "R^2 should be 1.0");

        // Test estimation
        expectWithinAbsoluteError (lr.estimateY (5.0), 11.0, 0.001, "Y(5) should be 11");
    }

    void testHorizontalLine()
    {
        beginTest ("Horizontal Line");

        // y = 5 (horizontal line)
        juce::Array<Point<double>> points;
        points.add ({0.0, 5.0});
        points.add ({1.0, 5.0});
        points.add ({2.0, 5.0});
        points.add ({3.0, 5.0});

        LinearRegression lr (points);

        expectWithinAbsoluteError (lr.getA(), 5.0, 0.001, "Intercept should be 5.0");
        expectWithinAbsoluteError (lr.getB(), 0.0, 0.001, "Slope should be 0.0");
    }

    void testNegativeSlope()
    {
        beginTest ("Negative Slope");

        // y = -3x + 10
        juce::Array<Point<double>> points;
        points.add ({0.0, 10.0});
        points.add ({1.0, 7.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 1.0});

        LinearRegression lr (points);

        expectWithinAbsoluteError (lr.getA(), 10.0, 0.001, "Intercept should be 10.0");
        expectWithinAbsoluteError (lr.getB(), -3.0, 0.001, "Slope should be -3.0");
    }

    void testAddPoints()
    {
        beginTest ("Add Points");

        LinearRegression lr;

        expect (!lr.haveData(), "Should have no data initially");

        lr.addPoint ({1.0, 2.0});
        lr.addPoint ({2.0, 4.0});
        lr.addPoint ({3.0, 6.0});
        lr.calculate();

        expect (lr.haveData(), "Should have data after adding points");
        expectWithinAbsoluteError (lr.getB(), 2.0, 0.001, "Slope should be 2.0");
    }

    void testClearData()
    {
        beginTest ("Clear Data");

        juce::Array<Point<double>> points;
        points.add ({1.0, 2.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 6.0});
        points.add ({4.0, 8.0});

        LinearRegression lr (points);
        expect (lr.haveData(), "Should have data");

        lr.clear();
        expect (!lr.haveData(), "Should have no data after clear");
        expectWithinAbsoluteError (lr.getA(), 0.0, 0.001, "a should be 0 after clear");
        expectWithinAbsoluteError (lr.getB(), 0.0, 0.001, "b should be 0 after clear");
    }
};

static LinearRegressionTests linearRegressionTests;

#endif

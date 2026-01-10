/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LeastSquaresRegressionTests : public juce::UnitTest
{
public:
    LeastSquaresRegressionTests() : juce::UnitTest ("Least Squares Regression", "gin") {}

    void runTest() override
    {
        testPerfectParabola();
        testLinearCase();
        testAddPoints();
        testRSquare();
        testClear();
    }

private:
    void testPerfectParabola()
    {
        beginTest ("Perfect Parabola");

        // y = 2x^2 + 3x + 1
        LeastSquaresRegression lsr;
        lsr.addPoint (0.0, 1.0);   // 2*0 + 3*0 + 1 = 1
        lsr.addPoint (1.0, 6.0);   // 2*1 + 3*1 + 1 = 6
        lsr.addPoint (2.0, 15.0);  // 2*4 + 3*2 + 1 = 15
        lsr.addPoint (3.0, 28.0);  // 2*9 + 3*3 + 1 = 28
        lsr.addPoint (4.0, 45.0);  // 2*16 + 3*4 + 1 = 45

        expect (lsr.enoughPoints(), "Should have enough points");

        expectWithinAbsoluteError (lsr.aTerm(), 2.0, 0.001, "a term should be 2.0");
        expectWithinAbsoluteError (lsr.bTerm(), 3.0, 0.001, "b term should be 3.0");
        expectWithinAbsoluteError (lsr.cTerm(), 1.0, 0.001, "c term should be 1.0");

        // R^2 should be 1.0 for perfect fit
        expectWithinAbsoluteError (lsr.rSquare(), 1.0, 0.001, "R-squared should be 1.0");
    }

    void testLinearCase()
    {
        beginTest ("Linear Case");

        // y = 5x + 2 (no quadratic term)
        LeastSquaresRegression lsr;
        lsr.addPoint (0.0, 2.0);
        lsr.addPoint (1.0, 7.0);
        lsr.addPoint (2.0, 12.0);
        lsr.addPoint (3.0, 17.0);

        // a should be ~0 for linear data
        expectWithinAbsoluteError (lsr.aTerm(), 0.0, 0.001, "a term should be ~0 for linear");
        expectWithinAbsoluteError (lsr.bTerm(), 5.0, 0.001, "b term should be 5.0");
        expectWithinAbsoluteError (lsr.cTerm(), 2.0, 0.001, "c term should be 2.0");
    }

    void testAddPoints()
    {
        beginTest ("Add Points");

        LeastSquaresRegression lsr;

        expect (!lsr.enoughPoints(), "Should not have enough points initially");

        lsr.addPoint ({0.0, 0.0});
        lsr.addPoint ({1.0, 1.0});

        expect (!lsr.enoughPoints(), "Should need at least 3 points");

        lsr.addPoint ({2.0, 4.0});

        expect (lsr.enoughPoints(), "Should have enough points now");

        // Test adding array of points
        juce::Array<Point<double>> points;
        points.add ({3.0, 9.0});
        points.add ({4.0, 16.0});

        lsr.addPoints (points);

        // y = x^2
        expectWithinAbsoluteError (lsr.aTerm(), 1.0, 0.001, "Should fit y = x^2");
        expectWithinAbsoluteError (lsr.bTerm(), 0.0, 0.001, "b should be 0");
        expectWithinAbsoluteError (lsr.cTerm(), 0.0, 0.001, "c should be 0");
    }

    void testRSquare()
    {
        beginTest ("R-Square");

        // Perfect fit data
        LeastSquaresRegression lsr;
        lsr.addPoint (0.0, 0.0);
        lsr.addPoint (1.0, 1.0);
        lsr.addPoint (2.0, 4.0);
        lsr.addPoint (3.0, 9.0);

        // R^2 should be 1.0 for perfect quadratic fit
        double r2 = lsr.rSquare();
        expect (r2 >= 0.99, "R-squared should be very close to 1.0 for perfect fit");
    }

    void testClear()
    {
        beginTest ("Clear");

        LeastSquaresRegression lsr;
        lsr.addPoint (1.0, 2.0);
        lsr.addPoint (2.0, 4.0);
        lsr.addPoint (3.0, 6.0);

        expect (lsr.enoughPoints(), "Should have points");

        lsr.clear();

        expect (!lsr.enoughPoints(), "Should have no points after clear");
    }
};

static LeastSquaresRegressionTests leastSquaresRegressionTests;

#endif

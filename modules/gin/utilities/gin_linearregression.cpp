/*==============================================================================

 Copyright 2018 by Roland Rabien, 1998 David C. Swaim
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

LinearRegression::LinearRegression()
{
}

LinearRegression::LinearRegression (juce::Array<Point<double>> points)
{
    for (auto p : points)
        addPoint (p);

    calculate();
}

void LinearRegression::clear()
{
    n = 0;
    sumX = 0;
    sumY = 0;
    sumXsquared = 0;
    sumYsquared = 0;
    sumXY = 0;

    a = 0;
    b = 0;
    coefD = 0;
    coefC = 0;
    stdError = 0;
}

void LinearRegression::addPoint (Point<double> pnt)
{
    n++;
    sumX += pnt.getX();
    sumY += pnt.getY();
    sumXsquared += pnt.getX() * pnt.getX();
    sumYsquared += pnt.getY() * pnt.getY();
    sumXY += pnt.getX() * pnt.getY();
}

void LinearRegression::calculate()
{
    if (haveData())
    {
        if (std::fabs (n * sumXsquared - sumX * sumX) > DBL_EPSILON)
        {
            b = (n * sumXY - sumY * sumX) / (n * sumXsquared - sumX * sumX);
            a = (sumY - b * sumX) / n;

            double sx = b * (sumXY - sumX * sumY / n);
            double sy2 = sumYsquared - sumY * sumY / n;
            double sy = sy2 - sx;

            coefD = sx / sy2;
            coefC = std::sqrt (coefD);
            stdError = std::sqrt(sy / (n - 2));
        }
        else
        {
            a = b = coefD = coefC = stdError = 0.0;
        }
    }
}

//==============================================================================
#if JUCE_UNIT_TESTS

class LinearRegressionTests : public juce::UnitTest
{
public:
    LinearRegressionTests() : juce::UnitTest ("Linear Regression", "gin") {}

    void runTest() override
    {
        testPerfectLine();
        testHorizontalLine();
        testVerticalTrend();
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
        expectWithinAbsoluteError (lr.a(), 1.0, 0.001, "Intercept should be 1.0");
        expectWithinAbsoluteError (lr.b(), 2.0, 0.001, "Slope should be 2.0");
        expectWithinAbsoluteError (lr.coefC(), 1.0, 0.001, "Correlation should be perfect");

        // Test estimation
        expectWithinAbsoluteError (lr.estimateY (5.0), 11.0, 0.001, "Y(5) should be 11");
        expectWithinAbsoluteError (lr.estimateX (15.0), 7.0, 0.001, "X(15) should be 7");
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

        expectWithinAbsoluteError (lr.a(), 5.0, 0.001, "Intercept should be 5.0");
        expectWithinAbsoluteError (lr.b(), 0.0, 0.001, "Slope should be 0.0");
    }

    void testVerticalTrend()
    {
        beginTest ("Vertical Trend");

        // y = -3x + 10
        juce::Array<Point<double>> points;
        points.add ({0.0, 10.0});
        points.add ({1.0, 7.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 1.0});

        LinearRegression lr (points);

        expectWithinAbsoluteError (lr.a(), 10.0, 0.001, "Intercept should be 10.0");
        expectWithinAbsoluteError (lr.b(), -3.0, 0.001, "Slope should be -3.0");
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
        expectWithinAbsoluteError (lr.b(), 2.0, 0.001, "Slope should be 2.0");
    }

    void testClearData()
    {
        beginTest ("Clear Data");

        juce::Array<Point<double>> points;
        points.add ({1.0, 2.0});
        points.add ({2.0, 4.0});

        LinearRegression lr (points);
        expect (lr.haveData(), "Should have data");

        lr.clear();
        expect (!lr.haveData(), "Should have no data after clear");
        expectWithinAbsoluteError (lr.a(), 0.0, 0.001, "a should be 0 after clear");
        expectWithinAbsoluteError (lr.b(), 0.0, 0.001, "b should be 0 after clear");
    }
};

static LinearRegressionTests linearRegressionTests;

#endif

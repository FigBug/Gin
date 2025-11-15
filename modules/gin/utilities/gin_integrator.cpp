/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

void Integrator::clear()
{
    first = true;
    curSum = 0;
    oldX   = 0;
    oldY   = 0;
}

double Integrator::getIntegral()
{
    return curSum;
}

void Integrator::addPoint (double x, double y)
{
    if (first)
    {
        first = false;
    }
    else
    {
        double curY = (oldY + y) / 2.0;
        curSum += curY * (x - oldX);
    }
    oldX = x;
    oldY = y;
}

void Integrator::addPoint (Point<double> point)
{
    addPoint (point.getX(), point.getY());
}

void Integrator::addPoints (juce::Array<Point<double>> points)
{
    for (auto point : points)
        addPoint (point.getX(), point.getY());
}

//==============================================================================
#if JUCE_UNIT_TESTS

class IntegratorTests : public juce::UnitTest
{
public:
    IntegratorTests() : juce::UnitTest ("Integrator", "gin") {}

    void runTest() override
    {
        testConstantFunction();
        testLinearFunction();
        testTriangleArea();
        testClear();
        testAddPoints();
    }

private:
    void testConstantFunction()
    {
        beginTest ("Constant Function");

        // Integral of y=2 from x=0 to x=5 should be 10
        Integrator integrator;
        integrator.addPoint (0.0, 2.0);
        integrator.addPoint (5.0, 2.0);

        expectWithinAbsoluteError (integrator.getIntegral(), 10.0, 0.001,
                                   "Integral of constant function should be height * width");
    }

    void testLinearFunction()
    {
        beginTest ("Linear Function");

        // Integral of y=x from x=0 to x=4 should be 8 (area of triangle)
        Integrator integrator;
        integrator.addPoint (0.0, 0.0);
        integrator.addPoint (1.0, 1.0);
        integrator.addPoint (2.0, 2.0);
        integrator.addPoint (3.0, 3.0);
        integrator.addPoint (4.0, 4.0);

        expectWithinAbsoluteError (integrator.getIntegral(), 8.0, 0.001,
                                   "Integral of y=x from 0 to 4 should be 8");
    }

    void testTriangleArea()
    {
        beginTest ("Triangle Area");

        // Triangle: (0,0), (2,4), (4,0) - area should be 8
        Integrator integrator;
        integrator.addPoint (0.0, 0.0);
        integrator.addPoint (2.0, 4.0);
        integrator.addPoint (4.0, 0.0);

        expectWithinAbsoluteError (integrator.getIntegral(), 8.0, 0.001,
                                   "Triangle area should be 8");
    }

    void testClear()
    {
        beginTest ("Clear");

        Integrator integrator;
        integrator.addPoint (0.0, 0.0);
        integrator.addPoint (10.0, 10.0);

        expect (integrator.getIntegral() > 0.0, "Should have non-zero integral");

        integrator.clear();
        expectWithinAbsoluteError (integrator.getIntegral(), 0.0, 0.001,
                                   "Integral should be 0 after clear");

        // Should be able to reuse after clear
        integrator.addPoint (0.0, 5.0);
        integrator.addPoint (2.0, 5.0);

        expectWithinAbsoluteError (integrator.getIntegral(), 10.0, 0.001,
                                   "Should work correctly after clear");
    }

    void testAddPoints()
    {
        beginTest ("Add Points Array");

        juce::Array<Point<double>> points;
        points.add ({0.0, 0.0});
        points.add ({1.0, 2.0});
        points.add ({2.0, 4.0});
        points.add ({3.0, 6.0});

        Integrator integrator;
        integrator.addPoints (points);

        // Integral of y=2x from 0 to 3 should be 9
        expectWithinAbsoluteError (integrator.getIntegral(), 9.0, 0.001,
                                   "Integral using addPoints should work");
    }
};

static IntegratorTests integratorTests;

#endif

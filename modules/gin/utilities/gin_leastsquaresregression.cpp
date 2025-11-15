/*==============================================================================

 Copyright 2018 by Roland Rabien, 2010 by 2010 by Alex Etchells
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

void LeastSquaresRegression::addPoint (double x, double y)
{
    pointArray.add ({x, y});
}

void LeastSquaresRegression::addPoint (Point<double> point)
{
    pointArray.add (point);
}

void LeastSquaresRegression::addPoints (juce::Array<Point<double>> points)
{
    pointArray.addArray (points);
}

void LeastSquaresRegression::clear()
{
    pointArray.clear();
}

bool LeastSquaresRegression::enoughPoints()
{
    return pointArray.size() >= 3;
}

juce::Array<double> LeastSquaresRegression::getTerms()
{
    //notation sjk to mean the sum of x_i^j*y_i^k.
    double s40 = getSx4(); //sum of x^4
    double s30 = getSx3(); //sum of x^3
    double s20 = getSx2(); //sum of x^2
    double s10 = getSx();  //sum of x
    double s00 = pointArray.size();
    //sum of x^0 * y^0  ie 1 * number of entries

    double s21 = getSx2y(); //sum of x^2*y
    double s11 = getSxy();  //sum of x*y
    double s01 = getSy();   //sum of y

    double a = (s21*(s20 * s00 - s10 * s10) -
            s11*(s30 * s00 - s10 * s20) +
            s01*(s30 * s10 - s20 * s20))
            /
            (s40*(s20 * s00 - s10 * s10) -
             s30*(s30 * s00 - s10 * s20) +
             s20*(s30 * s10 - s20 * s20));

    double b = (s40*(s11 * s00 - s01 * s10) -
            s30*(s21 * s00 - s01 * s20) +
            s20*(s21 * s10 - s11 * s20))
            /
            (s40 * (s20 * s00 - s10 * s10) -
             s30 * (s30 * s00 - s10 * s20) +
             s20 * (s30 * s10 - s20 * s20));

    double c = (s40*(s20 * s01 - s10 * s11) -
            s30*(s30 * s01 - s10 * s21) +
            s20*(s30 * s11 - s20 * s21))
            /
            (s40 * (s20 * s00 - s10 * s10) -
             s30 * (s30 * s00 - s10 * s20) +
             s20 * (s30 * s10 - s20 * s20));

    juce::Array<double> terms;
    terms.add (a);
    terms.add (b);
    terms.add (c);
    return terms;
}

double LeastSquaresRegression::aTerm()
{
    //notation sjk to mean the sum of x_i^j*y_i^k.
    double s40 = getSx4(); //sum of x^4
    double s30 = getSx3(); //sum of x^3
    double s20 = getSx2(); //sum of x^2
    double s10 = getSx();  //sum of x
    double s00 = pointArray.size();
    //sum of x^0 * y^0  ie 1 * number of entries

    double s21 = getSx2y(); //sum of x^2*y
    double s11 = getSxy();  //sum of x*y
    double s01 = getSy();   //sum of y

    //a = Da/D
    return (s21*(s20 * s00 - s10 * s10) -
            s11*(s30 * s00 - s10 * s20) +
            s01*(s30 * s10 - s20 * s20))
            /
            (s40*(s20 * s00 - s10 * s10) -
             s30*(s30 * s00 - s10 * s20) +
             s20*(s30 * s10 - s20 * s20));
}

double LeastSquaresRegression::bTerm()
{
    //notation sjk to mean the sum of x_i^j*y_i^k.
    double s40 = getSx4(); //sum of x^4
    double s30 = getSx3(); //sum of x^3
    double s20 = getSx2(); //sum of x^2
    double s10 = getSx();  //sum of x
    double s00 = pointArray.size();
    //sum of x^0 * y^0  ie 1 * number of entries

    double s21 = getSx2y(); //sum of x^2*y
    double s11 = getSxy();  //sum of x*y
    double s01 = getSy();   //sum of y

    //b = Db/D
    return (s40*(s11 * s00 - s01 * s10) -
            s30*(s21 * s00 - s01 * s20) +
            s20*(s21 * s10 - s11 * s20))
            /
            (s40 * (s20 * s00 - s10 * s10) -
             s30 * (s30 * s00 - s10 * s20) +
             s20 * (s30 * s10 - s20 * s20));
}

double LeastSquaresRegression::cTerm()
{
    //notation sjk to mean the sum of x_i^j*y_i^k.
    double s40 = getSx4(); //sum of x^4
    double s30 = getSx3(); //sum of x^3
    double s20 = getSx2(); //sum of x^2
    double s10 = getSx();  //sum of x
    double s00 = pointArray.size();
    //sum of x^0 * y^0  ie 1 * number of entries

    double s21 = getSx2y(); //sum of x^2*y
    double s11 = getSxy();  //sum of x*y
    double s01 = getSy();   //sum of y

    //c = Dc/D
    return (s40*(s20 * s01 - s10 * s11) -
            s30*(s30 * s01 - s10 * s21) +
            s20*(s30 * s11 - s20 * s21))
            /
            (s40 * (s20 * s00 - s10 * s10) -
             s30 * (s30 * s00 - s10 * s20) +
             s20 * (s30 * s10 - s20 * s20));
}

double LeastSquaresRegression::rSquare() // get r-squared
{
    // 1 - (residual sum of squares / total sum of squares)
    return 1 - getSSerr() / getSStot();
}


/*helper methods*/
double LeastSquaresRegression::getSx() // get sum of x
{
    double Sx = 0;
    for (auto it : pointArray)
    {
        Sx += it.getX();
    }
    return Sx;
}

double LeastSquaresRegression::getSy() // get sum of y
{
    double Sy = 0;
    for (auto it : pointArray)
    {
        Sy += it.getY();
    }
    return Sy;
}

double LeastSquaresRegression::getSx2() // get sum of x^2
{
    double Sx2 = 0;
    for (auto it : pointArray)
    {
        Sx2 += std::pow (it.getX(), 2); // sum of x^2
    }
    return Sx2;
}

double LeastSquaresRegression::getSx3() // get sum of x^3
{
    double Sx3 = 0;
    for (auto it : pointArray)
    {
        Sx3 += std::pow (it.getX(), 3); // sum of x^3
    }
    return Sx3;
}

double LeastSquaresRegression::getSx4() // get sum of x^4
{
    double Sx4 = 0;
    for (auto it : pointArray)
    {
        Sx4 += std::pow (it.getX(), 4); // sum of x^4
    }
    return Sx4;
}

double LeastSquaresRegression::getSxy() // get sum of x*y
{
    double Sxy = 0;
    for (auto it : pointArray)
    {
        Sxy += it.getX() * it.getY(); // sum of x*y
    }
    return Sxy;
}

double LeastSquaresRegression::getSx2y() // get sum of x^2*y
{
    double Sx2y = 0;
    for (auto it : pointArray)
    {
        Sx2y += pow(it.getX(), 2) * it.getY(); // sum of x^2*y
    }
    return Sx2y;
}

double LeastSquaresRegression::getYMean() // mean value of y
{
    double y_tot = 0;
    for (auto it : pointArray)
    {
        y_tot += it.getY();
    }
    return y_tot / pointArray.size();
}

double LeastSquaresRegression::getSStot() // total sum of squares
{
    //the sum of the squares of the differences between
    //the measured y values and the mean y value
    double ss_tot = 0;
    for (auto it : pointArray)
    {
        ss_tot += std::pow (it.getY() - getYMean(), 2);
    }
    return ss_tot;
}

double LeastSquaresRegression::getSSerr() // residual sum of squares
{
    //the sum of the squares of te difference between
    //the measured y values and the values of y predicted by the equation
    double ss_err = 0;
    for (auto it : pointArray)
    {
        ss_err += std::pow (it.getY() - getPredictedY (it.getX()), 2);
    }
    return ss_err;
}

double LeastSquaresRegression::getPredictedY (double x)
{
    //returns value of y predicted by the equation for a given value of x
    return aTerm() * std::pow(x, 2) + bTerm() * x + cTerm();
}

//==============================================================================
#if JUCE_UNIT_TESTS

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

        // Test prediction
        expectWithinAbsoluteError (lsr.getPredictedY (5.0), 66.0, 0.001, "Y(5) should be 66");

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

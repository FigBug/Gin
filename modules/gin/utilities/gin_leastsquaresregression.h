/*==============================================================================

 Copyright 2018 by Roland Rabien, 2010 by Alex Etchells
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
   See https://www.codeproject.com/Articles/63170/Least-Squares-Regression-for-Quadratic-Curve-Fitti
   for original code

   Derive the equation of a quadratic curve from a series of data points. That is to say, to
   determine a, b, and c, where y = ax2 + bx + c. Having determined a, b, and c, I would also
   need a value for R-squared (the coefficient of determination).
  */
class LeastSquaresRegression
{
public:
    LeastSquaresRegression() = default;

    void addPoint (double x, double y);
    void addPoint (Point<double> point);
    void addPoints (juce::Array<Point<double>> points);

    void clear();

    bool enoughPoints();
    juce::Array<double> getTerms();

    double aTerm();
    double bTerm();
    double cTerm();
    double rSquare();

private:
    double getSx();
    double getSy();
    double getSx2();
    double getSx3();
    double getSx4();
    double getSxy();
    double getSx2y();
    double getYMean();
    double getSStot();
    double getSSerr();
    double getPredictedY (double x);

    juce::Array<Point<double>> pointArray;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LeastSquaresRegression)
};

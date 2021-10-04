/*==============================================================================

 Copyright 2018 by Roland Rabien, 1998 David C. Swaim
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Calculates linear regression from a set of points */
class LinearRegression
{
public:
    LinearRegression();
    LinearRegression (juce::Array<Point<double>> points);

    void addPoint (Point<double> pnt);
    void calculate();

    void clear();

    bool haveData() const               { return n > 2; }
    int items() const                   { return n; }

    double getA() const                 { return a; }
    double getB() const                 { return b; }

    double getCoefDeterm() const        { return coefD; }
    double getCoefCorrel() const        { return coefC; }
    double getStdErrorEst() const       { return stdError; }
    double estimateY (double x) const   { return (a + b * x); }

protected:

    int n = 0;              // number of data points input so far
    double sumX = 0;
    double sumY = 0;        // sums of x and y
    double sumXsquared = 0; // sum of x squares
    double sumYsquared = 0; // sum y squares
    double sumXY = 0;       // sum of x*y

    double a = 0;
    double b = 0;           // coefficients of f(x) = a + b*x
    double coefD = 0;       // coefficient of determination
    double coefC = 0;       // coefficient of correlation
    double stdError = 0;    // standard error of estimate

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LinearRegression)
};

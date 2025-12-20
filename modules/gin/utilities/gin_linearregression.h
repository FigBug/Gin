/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien, 1998 David C. Swaim
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Performs linear regression to fit a straight line to data points.

    This class fits a linear equation y = a + bx to a set of data points using
    the least squares method. It calculates the coefficients (a, b) that minimize
    the sum of squared differences between the data points and the line.

    The class also computes:
    - Coefficient of determination (R²): Measures goodness of fit (0 to 1)
    - Coefficient of correlation: Measures strength of linear relationship (-1 to 1)
    - Standard error of estimate: Measures typical prediction error

    Example usage:
    @code
    LinearRegression regression;
    regression.addPoint (Point<double> (1.0, 2.3));
    regression.addPoint (Point<double> (2.0, 4.1));
    regression.addPoint (Point<double> (3.0, 5.9));
    regression.calculate();

    double a = regression.getA();  // Intercept
    double b = regression.getB();  // Slope
    double predicted = regression.estimateY (4.0);  // Predict y for x=4
    @endcode

    @see getA, getB, estimateY, getCoefDeterm
*/
class LinearRegression
{
public:
    /** Creates an empty linear regression object. */
    LinearRegression();

    /** Creates a linear regression object and adds the given points.

        @param points  Initial set of data points
    */
    LinearRegression (juce::Array<Point<double>> points);

    /** Adds a data point to the regression.

        @param pnt  The point to add
    */
    void addPoint (Point<double> pnt);

    /** Calculates the regression coefficients from the added points.
        Must be called after adding points and before retrieving results.
    */
    void calculate();

    /** Removes all data points and resets the regression. */
    void clear();

    /** Checks if enough data points have been added.

        @returns  true if more than 2 points have been added
    */
    bool haveData() const               { return n > 2; }

    /** Returns the number of data points.

        @returns  The number of points added
    */
    int items() const                   { return n; }

    /** Returns the intercept of the fitted line.

        @returns  The 'a' coefficient in y = a + bx
    */
    double getA() const                 { return a; }

    /** Returns the slope of the fitted line.

        @returns  The 'b' coefficient in y = a + bx
    */
    double getB() const                 { return b; }

    /** Returns the coefficient of determination (R²).

        @returns  R² value (0 to 1), where 1 indicates perfect fit
    */
    double getCoefDeterm() const        { return coefD; }

    /** Returns the coefficient of correlation.

        @returns  Correlation coefficient (-1 to 1)
    */
    double getCoefCorrel() const        { return coefC; }

    /** Returns the standard error of estimate.

        @returns  The typical prediction error
    */
    double getStdErrorEst() const       { return stdError; }

    /** Estimates the y value for a given x using the fitted line.

        @param x  The x coordinate
        @returns  The estimated y value
    */
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

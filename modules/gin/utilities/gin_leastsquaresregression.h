/*==============================================================================

 Copyright 2018 by Roland Rabien, 2010 by Alex Etchells
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Performs least squares regression to fit a quadratic curve to data points.

    This class fits a quadratic equation y = ax² + bx + c to a set of data points
    using the least squares method. It calculates the coefficients (a, b, c) that
    minimize the sum of squared differences between the data points and the curve.

    The class also computes R² (the coefficient of determination), which measures
    how well the curve fits the data (values closer to 1.0 indicate better fit).

    Example usage:
    @code
    LeastSquaresRegression regression;
    regression.addPoint (1.0, 2.5);
    regression.addPoint (2.0, 5.2);
    regression.addPoint (3.0, 8.8);
    // ... add more points

    if (regression.enoughPoints())
    {
        double a = regression.aTerm();  // Coefficient of x²
        double b = regression.bTerm();  // Coefficient of x
        double c = regression.cTerm();  // Constant term
        double r2 = regression.rSquare();  // Goodness of fit
    }
    @endcode

    Based on code from:
    https://www.codeproject.com/Articles/63170/Least-Squares-Regression-for-Quadratic-Curve-Fitti

    @see aTerm, bTerm, cTerm, rSquare
*/
class LeastSquaresRegression
{
public:
    /** Creates an empty regression object. */
    LeastSquaresRegression() = default;

    /** Adds a data point to the regression.

        @param x  The x coordinate
        @param y  The y coordinate
    */
    void addPoint (double x, double y);

    /** Adds a data point to the regression.

        @param point  The point to add
    */
    void addPoint (Point<double> point);

    /** Adds multiple data points to the regression.

        @param points  Array of points to add
    */
    void addPoints (juce::Array<Point<double>> points);

    /** Removes all data points. */
    void clear();

    /** Checks if enough points have been added for valid regression.

        @returns  true if there are enough points to calculate the curve
    */
    bool enoughPoints();

    /** Returns all three coefficients as an array.

        @returns  Array containing [a, b, c] where y = ax² + bx + c
    */
    juce::Array<double> getTerms();

    /** Returns the coefficient of the x² term.

        @returns  The 'a' coefficient in y = ax² + bx + c
    */
    double aTerm();

    /** Returns the coefficient of the x term.

        @returns  The 'b' coefficient in y = ax² + bx + c
    */
    double bTerm();

    /** Returns the constant term.

        @returns  The 'c' coefficient in y = ax² + bx + c
    */
    double cTerm();

    /** Returns the coefficient of determination (R²).

        R² measures how well the curve fits the data, ranging from 0 to 1.
        Values closer to 1 indicate a better fit.

        @returns  The R² value
    */
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

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Numerical integration using the trapezoidal rule for discrete data points.

    Integrator calculates the definite integral of a function represented by a
    series of (x, y) data points using the trapezoidal rule. This is useful for
    computing areas under curves, cumulative sums, or any integration task where
    you have discrete data points rather than a continuous function.

    The trapezoidal rule approximates the area between consecutive points as
    trapezoids, providing good accuracy for reasonably smooth data.

    Key Features:
    - Simple and efficient numerical integration
    - Works with discrete data points
    - Accumulates integral as points are added
    - No memory overhead (streaming calculation)

    Important: Points must be added in increasing x order for correct results.

    Usage:
    @code
    Integrator integrator;

    // Add data points in increasing x order
    integrator.addPoint(0.0, 0.0);
    integrator.addPoint(1.0, 2.5);
    integrator.addPoint(2.0, 4.0);
    integrator.addPoint(3.0, 3.5);
    integrator.addPoint(4.0, 1.0);

    // Get the accumulated integral
    double area = integrator.getIntegral();

    // Clear and start over
    integrator.clear();
    @endcode

    @see Spline, LeastSquaresRegression
*/
class Integrator
{
public:
    Integrator() = default;
    void clear();

    double getIntegral();

    void addPoint (double x, double y);
    void addPoint (Point<double> point);
    void addPoints (juce::Array<Point<double>> points);

private:

    bool first = true;
    double oldX = 0;
    double oldY = 0;
    double curSum = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Integrator)
};

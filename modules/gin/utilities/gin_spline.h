/*==============================================================================

 Copyright 2018 by Roland Rabien, Devin Lane
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Cubic spline interpolation for smooth curves through discrete data points.

    Spline provides cubic spline interpolation, creating a smooth curve that passes
    through all given data points. Unlike linear interpolation, cubic splines have
    continuous first and second derivatives (C1 and C2 continuity), resulting in
    very smooth, natural-looking curves.

    Cubic spline interpolation is ideal for:
    - Smooth animation curves
    - Data visualization and smoothing
    - Upsampling discrete data
    - Creating smooth parameter transitions
    - Generating lookup tables

    Key Features:
    - C1 and C2 continuity (smooth first and second derivatives)
    - Passes exactly through all input points
    - Efficient evaluation using piecewise cubic polynomials
    - Natural boundary conditions
    - Operator[] syntax for convenient evaluation

    Important: Points must be added in increasing x order.

    Usage:
    @code
    juce::Array<Point<double>> points;
    points.add({0.0, 0.0});
    points.add({1.0, 2.5});
    points.add({2.0, 1.8});
    points.add({3.0, 4.0});
    points.add({4.0, 3.2});

    Spline spline(points);

    // Interpolate at any x position
    double y1 = spline[1.5];      // Using operator[]
    double y2 = spline.interpolate(2.7);  // Using interpolate()

    // Evaluate many points for smooth curve
    for (double x = 0.0; x <= 4.0; x += 0.1)
    {
        double y = spline[x];
        // ... use interpolated value
    }
    @endcode

    Based on implementation by Devin Lane:
    https://shiftedbits.org/2011/01/30/cubic-spline-interpolation/

    @see Integrator, LeastSquaresRegression
*/
class Spline
{
public:
    Spline (const juce::Array<Point<double>>& points);

    double operator[] (double x) const;
    double interpolate (double x) const;

    class Element
    {
    public:
        Element (double x_ = 0) : x (x_) {}

        Element (double x_, double a_, double b_, double c_, double d_)
          : x (x_), a (a_), b (b_), c (c_), d (d_) {}

        double eval (double xx) const
        {
            double xix (xx - x);
            return a + b * xix + c * (xix * xix) + d * (xix * xix * xix);
        }

        bool operator< (const Element& e) const { return x < e.x;   }
        bool operator< (const double xx) const  { return x < xx;    }

        double x = 0, a = 0, b = 0, c = 0, d = 0;
    };

private:
    juce::Array<Element> elements;

    JUCE_LEAK_DETECTOR (Spline)
};

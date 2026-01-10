/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Lagrange interpolation functions for obtaining smooth curves from discrete points.

    Lagrange interpolation creates a polynomial that passes through all given points.
    Unlike splines, it uses a single polynomial rather than piecewise functions.
    Note that for large numbers of points, the resulting polynomial can oscillate
    significantly (Runge's phenomenon).

    @see interpolate
*/

namespace Lagrange
{

/** Interpolates a value at coordinate x using Lagrange polynomial interpolation.

    Creates a polynomial that passes through all the given points and evaluates it
    at the specified x coordinate. The resulting polynomial has degree n-1 where n
    is the number of points.

    @param points  An array of Point<T> defining the interpolation curve
    @param x       The x coordinate at which to evaluate the interpolated value
    @returns       The interpolated y value at coordinate x
*/
template <typename T>
T interpolate (const juce::Array<Point<T>>& points, T x)
{
    T res = 0;
  
    const int num = points.size();
    for (int i = 0; i < num; i++)
    {
        T term = points[i].y;
        for (int j = 0; j < num; j++)
        {
            if (i != j)
            {
                auto d = points[i].x - points[j].x;
                if (! juce::approximatelyEqual (d, T(0)))
                    term = term * (x - points[j].x ) / (d);
                else
                    term = 0;
            }
        }
  
        res += term;
    }
  
    return res;
}

/** Interpolates a value at coordinate x using Lagrange polynomial interpolation.

    This overload accepts separate arrays for x and y coordinates rather than
    an array of points. Creates a polynomial that passes through all the given
    points and evaluates it at the specified x coordinate.

    @param xArr  Array of x coordinates
    @param yArr  Array of y coordinates (must be same length as xArr)
    @param num   Number of points in the arrays
    @param x     The x coordinate at which to evaluate the interpolated value
    @returns     The interpolated y value at coordinate x
*/
template <typename T>
T interpolate (T xArr[], T yArr[], int num, T x)
{
    T res = 0;
  
    for (int i = 0; i < num; i++)
    {
        T term = yArr[i];
        for (int j = 0; j < num; j++)
        {
            if (i != j)
            {
                auto d = xArr[i] - xArr[j];
                if (! juce::approximatelyEqual (d, 0.0f))
                    term = term * (x - xArr[j] ) / d;
                else
                    term = 0;
            }
        }
  
        res += term;
    }
  
    return res;
}

}

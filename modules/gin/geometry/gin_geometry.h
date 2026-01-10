/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Returns the square of a value.

    @param in  The value to square
    @returns   in * in
*/
template <typename T>
inline T square (T in)
{
    return in * in;
}

//==============================================================================
/**
    Equations to calculate the shape of an ellipse and to calculate points
    on the edge of the ellipse and to see if points are inside or outside
*/
template <typename T>
class Ellipse
{
public:
    /** Creates a ellipse with a given centre and width / height. */
    Ellipse  (T x_, T y_, T a_, T b_) : x (x_), y (y_), a (a_), b (b_)
    {
    }

    /** Creates an ellipse centred at (0,0) with the specified width and height.

        @param a_  Half the width of the ellipse (semi-major axis if a > b)
        @param b_  Half the height of the ellipse (semi-minor axis if b < a)
    */
    Ellipse  (T a_, T b_) : a (a_), b (b_)
    {
    }

    /** Checks if a point lies on the ellipse boundary.

        @param pt        The point to test
        @param accuracy  Tolerance for the test (default: 0.00001)
        @returns         true if the point is on the ellipse within the given accuracy
    */
    bool isPointOn (Point<T> pt, T accuracy = 0.00001)
    {
        pt.x -= x;
        pt.y -= y;
        return std::abs (1.0 - (square (pt.getX()) / square (a) + square (pt.getY()) / square (b))) < accuracy;
    }

    /** Checks if a point lies outside the ellipse.

        @param pt  The point to test
        @returns   true if the point is outside the ellipse
    */
    bool isPointOutside (Point<T> pt)
    {
        pt.x -= x;
        pt.y -= y;
        return (square (pt.getX()) / square (a) + square (pt.getY()) / square (b)) > 1.0;
    }

    /** Checks if a point lies inside the ellipse.

        @param pt  The point to test
        @returns   true if the point is inside the ellipse
    */
    bool isPointInside (Point<T> pt)
    {
        pt.x -= x;
        pt.y -= y;
        return (square (pt.getX()) / square (a) + square (pt.getY()) / square (b)) < 1.0;
    }

    /** Calculates the point on the ellipse at the specified angle.

        The angle is measured from the positive x-axis, going counter-clockwise.

        @param angle  The angle in radians
        @returns      The point on the ellipse at that angle
    */
    Point<T> pointAtAngle (T angle)
    {
        T px = (a * b) / ::std::sqrt (square (b) + square (a) * square (::std::tan (angle)));
        T py = (a * b) / ::std::sqrt (square (a) + square (b) / square (::std::tan (angle)));

        while (angle < 0) angle += static_cast<T> (juce::MathConstants<double>::pi * 2);
        angle = static_cast<T> (::std::fmod (angle, juce::MathConstants<double>::pi * 2));

        if (angle >= static_cast<T> (juce::MathConstants<double>::pi / 2 * 3))
        {
            py = -py;
        }
        else if (angle >= static_cast<T> (juce::MathConstants<double>::pi))
        {
            py = -py;
            px = -px;
        }
        else if (angle >= static_cast<T> (juce::MathConstants<double>::pi / 2))
        {
            px = -px;
        }

        return {px + x, py + y};
    }

   #if JUCE_MODULE_AVAILABLE_juce_graphics
    /** Finds the intersection points between the ellipse and a line segment.

        Only available when juce_graphics module is present.

        @param p1  First endpoint of the line segment
        @param p2  Second endpoint of the line segment
        @returns   Array of intersection points (0, 1, or 2 points)
    */
    juce::Array<Point<T>> findIntersections (Point<T> p1, Point<T> p2)
    {
        juce::Array<Point<T>> res;

        p1.x -= x;
        p1.y -= y;

        p2.x -= x;
        p2.y -= y;

        Point<T> p3;
        Point<T> p4;

        juce::Rectangle<T> rect;

        rect.setX (std::min (p1.x, p2.x));
        rect.setWidth (std::max (p1.x, p2.x) - rect.getX());
        rect.setY (std::min (p1.y, p2.y));
        rect.setHeight (std::max (p1.y, p2.y) - rect.getY());

        float rx = a / 2.0f;
        float ry = b / 2.0f;

        float s = (p2.y - p1.y) / (p2.x - p1.x);
        float si = p2.y - (s * p2.x);
        float A = (ry * ry) + (rx * rx * s * s);
        float B = 2.0f * rx * rx * si * s;
        float C = rx * rx * si * si - rx * rx * ry * ry;

        float radicand_sqrt = std::sqrt ((B * B) - (4.0f * A * C));
        p3.x = (-B - radicand_sqrt) / (2.0f * A);
        p4.x = (-B + radicand_sqrt) / (2.0f * A);
        p3.y = s * p3.x + si;
        p4.y = s * p4.x + si;

        if (juce::approximatelyEqual (rect.getWidth(), 0.0f))
        {
            if (p3.y >= rect.getY() && p3.y <= rect.getY() + rect.getHeight())
                res.add (p3);

            if (p4.y >= rect.getY() && p4.y <= rect.getY() + rect.getHeight())
                res.add (p4);
        }
        else if (juce::approximatelyEqual (rect.getHeight(), 0.0f))
        {
            if (p3.x >= rect.getX() && p3.x <= rect.getX() + rect.getWidth())
                res.add (p3);

            if (p4.x >= rect.getX() && p4.x <= rect.getX() + rect.getWidth())
                res.add (p4);
        }
        else
        {
            if (rect.contains (p3))
                res.add (p3);

            if (rect.contains (p4))
                res.add (p4);
        }

        for (auto& p : res)
        {
            p.x += x;
            p.y += y;
        }

        return res;
    }
   #endif

    T x = 0, y = 0;  /**< Centre coordinates of the ellipse */
    T a = 0, b = 0;  /**< Semi-major and semi-minor axes (half width and height) */
};

//==============================================================================
/** Solves for the slope and intercept of a line given two points.

    Calculates m and b in the equation y = mx + b from two points on the line.

    @param x1  X coordinate of the first point
    @param y1  Y coordinate of the first point
    @param x2  X coordinate of the second point
    @param y2  Y coordinate of the second point
    @param m   Output: the slope of the line
    @param b   Output: the y-intercept of the line
    @returns   true if the line could be solved (returns false for vertical lines)
*/
template <typename T>
bool solveLine (T x1, T y1, T x2, T y2, T& m, T& b)
{
    if (! juce::exactlyEqual (x2, x1))
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - m * x2;

        return true;
    }
    else
    {
        m = 0;
        b = 0;

        return false;
    }
}

#if JUCE_MODULE_AVAILABLE_juce_graphics
/** Solves for the slope and intercept of a juce::Line.

    Only available when juce_graphics module is present.
    Calculates m and b in the equation y = mx + b from the line.

    @param l  The line to solve
    @param m  Output: the slope of the line
    @param b  Output: the y-intercept of the line
    @returns  true if the line could be solved (returns false for vertical lines)
*/
template <typename T>
bool solveLine (const juce::Line<T>& l, T& m, T& b)
{
    T x1 = l.getStartX();
    T x2 = l.getEndX();
    T y1 = l.getStartY();
    T y2 = l.getEndY();

    if (! juce::exactlyEqual (x2, x1))
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - m * x2;

        return true;
    }
    else
    {
        m = 0;
        b = 0;

        return false;
    }
}

/** Calculates the x coordinate for a given y value on a line.

    Only available when juce_graphics module is present.

    @param line  The line to evaluate
    @param y     The y coordinate
    @returns     The corresponding x coordinate
*/
template <typename T>
T getXForY (const juce::Line<T> line, T y)
{
    float m = 0.0f;
    float b = 0.0f;

    if (solveLine (line, m, b))
    {
        if (! juce::exactlyEqual (m, 0.0f))
            return (y - b) / m;
        
        return line.getStartX();
    }
    else
    {
        return line.getStartX();
    }
}

#endif

/** A simple polygon defined by a series of points.

    This class represents a polygon as an ordered collection of points.
    The points define the vertices of the polygon in sequence.

    @see points
*/
template <typename T>
class Polygon
{
public:
    /** The array of points defining the polygon's vertices. */
    juce::Array<Point<T>>   points;
};

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

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
    Ellipse  (T a_, T b_) : a (a_), b (b_)
    {
    }

    bool isPointOn (juce::Point<T> pt, T accuracy = 0.00001)
    {
        return std::abs (1.0 - (square (pt.getX()) / square (a) + square (pt.getY()) / square (b))) < accuracy;
    }

    bool isPointOutside (juce::Point<T> pt)
    {
        return (square (pt.getX()) / square (a) + square (pt.getY()) / square (b)) > 1.0;
    }

    bool isPointInside (juce::Point<T> pt)
    {
        return (square (pt.getX()) / square (a) + square (pt.getY()) / square (b)) < 1.0;
    }

    juce::Point<T> pointAtAngle (T angle)
    {
        T x = (a * b) / std::sqrt (square (b) + square (a) * square (std::tan (angle)));
        T y = (a * b) / std::sqrt (square (a) + square (b) / square (std::tan (angle)));

        while (angle < 0) angle += juce::double_Pi * 2;
        angle = std::fmod (angle, juce::double_Pi * 2);

        if (angle >= juce::double_Pi / 2 * 3)
        {
            y = -y;
        }
        else if (angle >= juce::double_Pi)
        {
            y = -y;
            x = -x;
        }
        else if (angle >= juce::double_Pi / 2)
        {
            x = -x;
        }

        return {x, y};
    }

    T a = 0, b = 0;
};

//==============================================================================
/** Solves for the slope and intercept of a line.
*/
template <typename T>
bool solveLine (T x1, T y1, T x2, T y2, T& m, T& b)
{
    if (x2 != x1)
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

/** Solves for the slope and intercept of a line
*/
template <typename T>
bool solveLine (juce::Line<T> l, T& m, T& b)
{
    T x1 = l.getStartX();
    T x2 = l.getEndX();
    T y1 = l.getStartY();
    T y2 = l.getEndY();

    if (x2 != x1)
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

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Calculates the integral of a function defined by a series of points. Points
    must be added in increasing x order */
class Integrator
{
public:
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

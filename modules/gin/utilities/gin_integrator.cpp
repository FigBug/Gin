/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

void Integrator::clear()
{
    first = true;
    curSum = 0;
    oldX   = 0;
    oldY   = 0;
}

double Integrator::getIntegral()
{
    return curSum;
}

void Integrator::addPoint (double x, double y)
{
    if (first)
    {
        first = false;
    }
    else
    {
        double curY = (oldY + y) / 2.0;
        curSum += curY * (x - oldX);
    }
    oldX = x;
    oldY = y;
}

void Integrator::addPoint (Point<double> point)
{
    addPoint (point.getX(), point.getY());
}

void Integrator::addPoints (juce::Array<Point<double>> points)
{
    for (auto point : points)
        addPoint (point.getX(), point.getY());
}

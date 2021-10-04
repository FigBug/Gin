/*==============================================================================

 Copyright 2018 by Roland Rabien, 1998 David C. Swaim
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

LinearRegression::LinearRegression()
{
}

LinearRegression::LinearRegression (juce::Array<Point<double>> points)
{
    for (auto p : points)
        addPoint (p);

    calculate();
}

void LinearRegression::clear()
{
    n = 0;
    sumX = 0;
    sumY = 0;
    sumXsquared = 0;
    sumYsquared = 0;
    sumXY = 0;

    a = 0;
    b = 0;
    coefD = 0;
    coefC = 0;
    stdError = 0;
}

void LinearRegression::addPoint (Point<double> pnt)
{
    n++;
    sumX += pnt.getX();
    sumY += pnt.getY();
    sumXsquared += pnt.getX() * pnt.getX();
    sumYsquared += pnt.getY() * pnt.getY();
    sumXY += pnt.getX() * pnt.getY();
}

void LinearRegression::calculate()
{
    if (haveData())
    {
        if (std::fabs (n * sumXsquared - sumX * sumX) > DBL_EPSILON)
        {
            b = (n * sumXY - sumY * sumX) / (n * sumXsquared - sumX * sumX);
            a = (sumY - b * sumX) / n;

            double sx = b * (sumXY - sumX * sumY / n);
            double sy2 = sumYsquared - sumY * sumY / n;
            double sy = sy2 - sx;

            coefD = sx / sy2;
            coefC = std::sqrt (coefD);
            stdError = std::sqrt(sy / (n - 2));
        }
        else
        {
            a = b = coefD = coefC = stdError = 0.0;
        }
    }
}

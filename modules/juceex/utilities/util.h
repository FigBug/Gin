/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

bool solveLine (double x1, double y1, double x2, double y2, double* m, double* b);

class RollingAverage
{
public:
    RollingAverage (int numVals_)
      : numVals(numVals_),
        currAvg(0)
    {
    }

    double average (double nextValue)
    {
        return (nextValue + numVals * currAvg) / (double)(numVals + 1); 
    }

    double getAverage()
    {
        return currAvg;
    }

private:
    int numVals;
    double currAvg;
};

Colour goldenRatioColor(int idx);



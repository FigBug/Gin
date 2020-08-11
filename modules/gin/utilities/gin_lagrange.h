/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Lagrange interpolation is a simple way to obtain a smooth curve from a set of
    discrete points.
*/

namespace Lagrange
{

template <class T>
T interpolate (const juce::Array<juce::Point<T>>& points, T x)
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
                if (d != 0)
                    term = term * (x - points[j].x ) / (d);
                else
                    term = 0;
            }
        }
  
        res += term;
    }
  
    return res;
}

template <class T>
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
                if (d != 0)
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

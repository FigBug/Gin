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
T interpolate (const Array<Point<T>>& points, T x)
{
    T res = 0;
  
    const int num = points.size();
    for (int i = 0; i < num; i++)
    {
        T term = points[i].y;
        for (int j = 0; j < num; j++)
            if (i != j)
                term = term * (x - points[j].x ) / (points[i].x - points[j].x);
  
        res += term;
    }
  
    return res;
}

}

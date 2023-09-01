/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Like the juce::Point, useful for project that don't use juce_graphics
 */
template <typename T>
class Point
{
public:
    Point() = default;
    Point (T x_, T y_) { x = x_; y = y_; }
   #if JUCE_MODULE_AVAILABLE_juce_graphics
    Point (juce::Point<T> p)
    {
        x = p.x;
        y = p.y;
    }
   #endif

    T x = T (0);
    T y = T (0);

    T getX() { return x; }
    T getY() { return y; }

   #if JUCE_MODULE_AVAILABLE_juce_graphics
    juce::Point<T> toJuce()
    {
        return { x, y };
    }

    operator juce::Point<T>()
    {
        return juce::Point (x, y);
    }

   #endif
};

/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** A lightweight 2D point class for projects that don't use juce_graphics.

    This class provides a simple coordinate pair with optional conversion to/from
    juce::Point when juce_graphics is available. It's useful for modules that need
    basic point functionality without depending on the full graphics module.

    @see getX, getY, toJuce
*/
template <typename T>
class Point
{
public:
    /** Creates a point at the origin (0, 0). */
    Point() = default;

    /** Creates a point with the specified coordinates.

        @param x_  The x coordinate
        @param y_  The y coordinate
    */
    Point (T x_, T y_) { x = x_; y = y_; }
   #if JUCE_MODULE_AVAILABLE_juce_graphics
    /** Creates a point from a juce::Point.
        Only available when juce_graphics module is present.

        @param p  The juce::Point to copy from
    */
    Point (juce::Point<T> p)
    {
        x = p.x;
        y = p.y;
    }
   #endif

    /** The x coordinate. */
    T x = T (0);

    /** The y coordinate. */
    T y = T (0);

    /** Returns the x coordinate.
        @returns  The x coordinate
    */
    T getX() { return x; }

    /** Returns the y coordinate.
        @returns  The y coordinate
    */
    T getY() { return y; }

   #if JUCE_MODULE_AVAILABLE_juce_graphics
    /** Converts this point to a juce::Point.
        Only available when juce_graphics module is present.

        @returns  A juce::Point with the same coordinates
    */
    juce::Point<T> toJuce()
    {
        return { x, y };
    }

    /** Automatic conversion operator to juce::Point.
        Only available when juce_graphics module is present.

        @returns  A juce::Point with the same coordinates
    */
    operator juce::Point<T>()
    {
        return juce::Point (x, y);
    }

   #endif
};

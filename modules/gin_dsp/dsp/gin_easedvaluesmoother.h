/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
// Easing function wrappers for template parameters
//==============================================================================

/** Linear easing function object (no easing). */
struct LinearEasing
{
    template<class T>
    static T ease (T p) { return easeLinear (p); }
};

/** Quadratic In easing function object. */
struct QuadraticInEasing
{
    template<class T>
    static T ease (T p) { return easeQuadraticIn (p); }
};

/** Quadratic Out easing function object. */
struct QuadraticOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuadraticOut (p); }
};

/** Quadratic InOut easing function object. */
struct QuadraticInOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuadraticInOut (p); }
};

/** Cubic In easing function object. */
struct CubicInEasing
{
    template<class T>
    static T ease (T p) { return easeCubicIn (p); }
};

/** Cubic Out easing function object. */
struct CubicOutEasing
{
    template<class T>
    static T ease (T p) { return easeCubicOut (p); }
};

/** Cubic InOut easing function object. */
struct CubicInOutEasing
{
    template<class T>
    static T ease (T p) { return easeCubicInOut (p); }
};

/** Quartic In easing function object. */
struct QuarticInEasing
{
    template<class T>
    static T ease (T p) { return easeQuarticIn (p); }
};

/** Quartic Out easing function object. */
struct QuarticOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuarticOut (p); }
};

/** Quartic InOut easing function object. */
struct QuarticInOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuarticInOut (p); }
};

/** Quintic In easing function object. */
struct QuinticInEasing
{
    template<class T>
    static T ease (T p) { return easeQuinticIn (p); }
};

/** Quintic Out easing function object. */
struct QuinticOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuinticOut (p); }
};

/** Quintic InOut easing function object. */
struct QuinticInOutEasing
{
    template<class T>
    static T ease (T p) { return easeQuinticInOut (p); }
};

/** Sine In easing function object. */
struct SineInEasing
{
    template<class T>
    static T ease (T p) { return easeSineIn (p); }
};

/** Sine Out easing function object. */
struct SineOutEasing
{
    template<class T>
    static T ease (T p) { return easeSineOut (p); }
};

/** Sine InOut easing function object. */
struct SineInOutEasing
{
    template<class T>
    static T ease (T p) { return easeSineInOut (p); }
};

/** Circular In easing function object. */
struct CircularInEasing
{
    template<class T>
    static T ease (T p) { return easeCircularIn (p); }
};

/** Circular Out easing function object. */
struct CircularOutEasing
{
    template<class T>
    static T ease (T p) { return easeCircularOut (p); }
};

/** Circular InOut easing function object. */
struct CircularInOutEasing
{
    template<class T>
    static T ease (T p) { return easeCircularInOut (p); }
};

/** Exponential In easing function object. */
struct ExponentialInEasing
{
    template<class T>
    static T ease (T p) { return easeExponentialIn (p); }
};

/** Exponential Out easing function object. */
struct ExponentialOutEasing
{
    template<class T>
    static T ease (T p) { return easeExponentialOut (p); }
};

/** Exponential InOut easing function object. */
struct ExponentialInOutEasing
{
    template<class T>
    static T ease (T p) { return easeExponentialInOut (p); }
};

/** Elastic In easing function object. */
struct ElasticInEasing
{
    template<class T>
    static T ease (T p) { return easeElasticIn (p); }
};

/** Elastic Out easing function object. */
struct ElasticOutEasing
{
    template<class T>
    static T ease (T p) { return easeElasticOut (p); }
};

/** Elastic InOut easing function object. */
struct ElasticInOutEasing
{
    template<class T>
    static T ease (T p) { return easeElasticInOut (p); }
};

/** Back In easing function object. */
struct BackInEasing
{
    template<class T>
    static T ease (T p) { return easeBackIn (p); }
};

/** Back Out easing function object. */
struct BackOutEasing
{
    template<class T>
    static T ease (T p) { return easeBackOut (p); }
};

/** Back InOut easing function object. */
struct BackInOutEasing
{
    template<class T>
    static T ease (T p) { return easeBackInOut (p); }
};

/** Bounce In easing function object. */
struct BounceInEasing
{
    template<class T>
    static T ease (T p) { return easeBounceIn (p); }
};

/** Bounce Out easing function object. */
struct BounceOutEasing
{
    template<class T>
    static T ease (T p) { return easeBounceOut (p); }
};

/** Bounce InOut easing function object. */
struct BounceInOutEasing
{
    template<class T>
    static T ease (T p) { return easeBounceInOut (p); }
};

//==============================================================================
/** Smooths a value using an easing function for natural-feeling transitions.

    EasedValueSmoother provides smooth parameter transitions using easing curves
    instead of linear interpolation. This creates more natural, aesthetically
    pleasing animations and parameter changes.

    The easing function is specified as a template parameter, allowing for
    compile-time selection of the curve shape. Any of the easing function objects
    defined in this file can be used.

    @tparam T The numeric type for values (typically float or double)
    @tparam EasingFunction Function object providing the easing curve (defaults to QuadraticInOutEasing)

    Example:
    @code
    // Default quadratic in/out easing
    EasedValueSmoother<float> smoother1;

    // Elastic easing for bouncy effect
    EasedValueSmoother<float, ElasticOutEasing> smoother2;

    // Exponential easing for sharp transitions
    EasedValueSmoother<float, ExponentialInOutEasing> smoother3;
    @endcode
*/
template <class T, class EasingFunction = QuadraticInOutEasing>
class EasedValueSmoother
{
public:
    void setSampleRate (double sr)  { sampleRate = sr; reset();     }
    void setTime (double t)         { time = t; reset();            }

    bool isSmoothing()              { return position < 1.0;        }
    void reset()                    {                               }

    T getCurrentValue()             { return currentValue;          }
    T* getValuePtr()                { return &currentValue;         }

    void snapToValue()
    {
        currentValue = targetValue;
        position = 1.0;
    }

    void process (int n)
    {
        if (! juce::approximatelyEqual (targetValue, currentValue))
            for (int i = 0; i < n; i++)
                updateValue();
    }

    void setValue (T v)
    {
        origValue = currentValue;
        targetValue = v;
        position = 0;
    }

    void updateValue()
    {
        if (position < 1.0)
        {
            double delta = 1.0 / double (sampleRate * time);
            position = std::min (position + delta, 1.0);

            auto f = EasingFunction::ease (position);
            currentValue = T ((targetValue * f) + (origValue * (1.0 - f)));
        }
    }

    T getNextValue()
    {
        updateValue();
        return (position < 1.0) ? currentValue : targetValue;
    }

    void setValueUnsmoothed (T v)
    {
        targetValue  = v;
        currentValue = v;
        origValue    = v;
        position     = 1.0;
    }

private:
    double time = 0.1, sampleRate = 44100.0;
    double position = 1.0;

    T targetValue = 0;
    T origValue = 0;
    T currentValue = 0;
};

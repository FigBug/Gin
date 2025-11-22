//  Easing functions based on AHEasing
//  Converted to template functions for Gin
//
//  Copyright (c) 2011, Auerhaus Development, LLC
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What The Fuck You Want
//  To Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/wtfpl/COPYING for more details.

//==============================================================================
/** @file gin_easing.h
    Collection of easing functions for animation and interpolation.

    Easing functions provide smooth transitions between values, commonly used
    in animations, UI transitions, and parameter smoothing. All functions take
    a normalized input value [0, 1] and return a normalized output [0, 1].

    Function naming convention:
    - ease[Type]In: Acceleration from zero velocity
    - ease[Type]Out: Deceleration to zero velocity
    - ease[Type]InOut: Acceleration until halfway, then deceleration

    Available easing types:
    - Linear: Constant velocity
    - Quadratic, Cubic, Quartic, Quintic: Polynomial curves
    - Sine: Smooth sinusoidal motion
    - Circular: Quarter-circle motion
    - Exponential: Sharp exponential curves
    - Elastic: Damped sine wave with overshoot
    - Back: Overshooting motion
    - Bounce: Bouncing motion with decreasing amplitude

    Example:
    @code
    float t = 0.5f;  // Halfway through animation
    float eased = easeCubicInOut (t);  // Smooth acceleration/deceleration
    float value = startValue + eased * (endValue - startValue);
    @endcode

    Based on AHEasing by Auerhaus Development, LLC
    @see http://easings.net for visual demonstrations
*/

/** Linear interpolation with no easing.

    Modeled after the line y = x. Provides constant velocity with no
    acceleration or deceleration.

    @param p  Normalized time value [0, 1]
    @returns  Linearly interpolated value [0, 1]
*/
template<class T>
T easeLinear (T p)
{
    return p;
}

/** Quadratic easing in - accelerating from zero velocity.

    Modeled after the parabola y = x^2. Starts slowly and accelerates.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuadraticIn (T p)
{
    return p * p;
}

/** Quadratic easing out - decelerating to zero velocity.

    Modeled after the parabola y = -x^2 + 2x. Starts quickly and decelerates.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuadraticOut (T p)
{
    return -(p * (p - 2));
}

/** Quadratic easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise quadratic:
    - y = (1/2)((2x)^2) for [0, 0.5)
    - y = -(1/2)((2x-1)*(2x-3) - 1) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuadraticInOut (T p)
{
    if (p < 0.5)
        return 2 * p * p;
    else
        return (-2 * p * p) + (4 * p) - 1;
}

/** Cubic easing in - accelerating from zero velocity.

    Modeled after the cubic y = x^3. Provides smoother acceleration than quadratic.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCubicIn (T p)
{
    return p * p * p;
}

/** Cubic easing out - decelerating to zero velocity.

    Modeled after the cubic y = (x - 1)^3 + 1. Provides smoother deceleration than quadratic.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCubicOut (T p)
{
    T f = (p - 1);
    return f * f * f + 1;
}

/** Cubic easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise cubic:
    - y = (1/2)((2x)^3) for [0, 0.5)
    - y = (1/2)((2x-2)^3 + 2) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCubicInOut (T p)
{
    if (p < 0.5)
        return 4 * p * p * p;

    T f = ((2 * p) - 2);
    return T (0.5 * f * f * f + 1);
}

/** Quartic easing in - accelerating from zero velocity.

    Modeled after the quartic x^4. More pronounced acceleration than cubic.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuarticIn (T p)
{
    return p * p * p * p;
}

/** Quartic easing out - decelerating to zero velocity.

    Modeled after the quartic y = 1 - (x - 1)^4. More pronounced deceleration than cubic.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuarticOut (T p)
{
    T f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

/** Quartic easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise quartic:
    - y = (1/2)((2x)^4) for [0, 0.5)
    - y = -(1/2)((2x-2)^4 - 2) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuarticInOut (T p)
{
    if (p < 0.5)
        return 8 * p * p * p * p;

    T f = (p - 1);
    return -8 * f * f * f * f + 1;
}

/** Quintic easing in - accelerating from zero velocity.

    Modeled after the quintic y = x^5. Very pronounced acceleration.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuinticIn (T p)
{
    return p * p * p * p * p;
}

/** Quintic easing out - decelerating to zero velocity.

    Modeled after the quintic y = (x - 1)^5 + 1. Very pronounced deceleration.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuinticOut (T p)
{
    T f = (p - 1);
    return f * f * f * f * f + 1;
}

/** Quintic easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise quintic:
    - y = (1/2)((2x)^5) for [0, 0.5)
    - y = (1/2)((2x-2)^5 + 2) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeQuinticInOut (T p)
{
    if (p < 0.5)
        return 16 * p * p * p * p * p;

    T f = ((2 * p) - 2);
    return  T (0.5 * f * f * f * f * f + 1);
}

/** Sine easing in - accelerating from zero velocity.

    Modeled after a quarter-cycle of a sine wave. Provides smooth, gradual acceleration.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeSineIn (T p)
{
    return std::sin ((p - 1) * (juce::MathConstants<T>::pi / 2)) + 1;
}

/** Sine easing out - decelerating to zero velocity.

    Modeled after a quarter-cycle of a sine wave (different phase). Provides smooth, gradual deceleration.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeSineOut (T p)
{
    return std::sin (p * juce::MathConstants<T>::pi / 2);
}

/** Sine easing in/out - acceleration until halfway, then deceleration.

    Modeled after a half sine wave. Provides smooth, natural motion.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeSineInOut (T p)
{
    return T (0.5) * (1 - std::cos (p * juce::MathConstants<T>::pi));
}

/** Circular easing in - accelerating from zero velocity.

    Modeled after a shifted quadrant IV of a unit circle. Creates a gentle curve
    that accelerates more smoothly than polynomial easings.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCircularIn (T p)
{
    return 1 - std::sqrt (1 - (p * p));
}

/** Circular easing out - decelerating to zero velocity.

    Modeled after a shifted quadrant II of a unit circle. Creates a gentle curve
    that decelerates more smoothly than polynomial easings.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCircularOut (T p)
{
    return std::sqrt ((2 - p) * p);
}

/** Circular easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise circular function:
    - y = (1/2)(1 - sqrt(1 - 4x^2)) for [0, 0.5)
    - y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeCircularInOut (T p)
{
    if (p < 0.5)
        return T (0.5 * (1 - std::sqrt (1 - 4 * (p * p))));
    else
        return T (0.5 * (std::sqrt (-((2 * p) - 3) * ((2 * p) - 1)) + 1));
}

/** Exponential easing in - accelerating from zero velocity.

    Modeled after the exponential function y = 2^(10(x - 1)). Creates a very sharp,
    dramatic acceleration curve.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeExponentialIn (T p)
{
    return T ((p == 0.0) ? p : std::pow (2, 10 * (p - 1)));
}

/** Exponential easing out - decelerating to zero velocity.

    Modeled after the exponential function y = -2^(-10x) + 1. Creates a very sharp,
    dramatic deceleration curve.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeExponentialOut (T p)
{
    return T ((p == 1.0) ? p : 1 - std::pow (2, -10 * p));
}

/** Exponential easing in/out - acceleration until halfway, then deceleration.

    Modeled after the piecewise exponential:
    - y = (1/2)2^(10(2x - 1)) for [0, 0.5)
    - y = -(1/2)*2^(-10(2x - 1))) + 1 for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1]
*/
template<class T>
T easeExponentialInOut (T p)
{
    if (p == 0.0 || p == 1.0) return p;

    if (p < 0.5)
        return T (0.5 * std::pow (2, (20 * p) - 10));
    else
        return T (-0.5 * std::pow (2, (-20 * p) + 10) + 1);
}

/** Elastic easing in - accelerating with oscillation.

    Modeled after a damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1)).
    Creates an elastic, spring-like effect that overshoots and oscillates
    before settling.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with overshoot
*/
template<class T>
T easeElasticIn (T p)
{
    return std::sin (13 * (juce::MathConstants<T>::pi / 2) * p) * static_cast<T> (std::pow (2, 10 * (p - 1)));
}

/** Elastic easing out - decelerating with oscillation.

    Modeled after a damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1.
    Creates an elastic, spring-like effect that overshoots and oscillates
    before settling.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with overshoot
*/
template<class T>
T easeElasticOut (T p)
{
    return std::sin (-13 * (juce::MathConstants<T>::pi / 2) * (p + 1)) * static_cast<T> (std::pow (2, -10 * p)) + 1;
}

/** Elastic easing in/out - acceleration and deceleration with oscillation.

    Modeled after a piecewise exponentially-damped sine wave:
    - y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1)) for [0, 0.5)
    - y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with overshoot
*/
template<class T>
T easeElasticInOut (T p)
{
    if (p < 0.5)
        return T (0.5 * std::sin (13 * (juce::MathConstants<T>::pi / 2) * (2 * p)) * std::pow (2, 10 * ((2 * p) - 1)));
    else
        return T (0.5 * (std::sin (-13 * (juce::MathConstants<T>::pi / 2) * ((2 * p - 1) + 1)) * std::pow (2, -10 * (2 * p - 1)) + 2));
}

/** Back easing in - accelerating with slight overshoot.

    Modeled after the overshooting cubic y = x^3-x*sin(x*pi). Pulls back
    slightly before accelerating forward, creating anticipation.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with slight overshoot
*/
template<class T>
T easeBackIn (T p)
{
    return p * p * p - p * std::sin (p * juce::MathConstants<T>::pi);
}

/** Back easing out - decelerating with overshoot.

    Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi)).
    Overshoots the target slightly before settling back.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with overshoot
*/
template<class T>
T easeBackOut (T p)
{
    T f = (1 - p);
    return 1 - (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi));
}

/** Back easing in/out - acceleration and deceleration with overshoot.

    Modeled after the piecewise overshooting cubic function:
    - y = (1/2)*((2x)^3-(2x)*sin(2*x*pi)) for [0, 0.5)
    - y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) for [0.5, 1]

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with overshoot
*/
template<class T>
T easeBackInOut (T p)
{
    if (p < 0.5)
    {
        T f = 2 * p;
        return T (0.5 * (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi)));
    }
    else
    {
        T f = (1 - (2*p - 1));
        return T (0.5 * (1 - (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi))) + 0.5);
    }
}

/** Bounce easing out - decelerating with bounce effect.

    Simulates a bouncing motion with decreasing amplitude, like a ball
    bouncing and coming to rest. Uses piecewise parabolic segments to
    create the bounce effect.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with bouncing
*/
template<class T>
T easeBounceOut (T p)
{
    if (p < 4/11.0)
        return T((121 * p * p) / 16.0);
    else if (p < 8/11.0)
        return T ((363/40.0 * p * p) - (99/10.0 * p) + 17/5.0);
    else if (p < 9/10.0)
        return T ((4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0);
    else
        return T ((54/5.0 * p * p) - (513/25.0 * p) + 268/25.0);
}

/** Bounce easing in - accelerating with bounce effect.

    The inverse of easeBounceOut. Creates a bouncing motion at the start
    before accelerating to the final value.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with bouncing
*/
template<class T>
T easeBounceIn (T p)
{
    return 1 - easeBounceOut (1 - p);
}

/** Bounce easing in/out - bouncing at both ends.

    Combines bounce in and bounce out, creating a bouncing effect at both
    the start and end of the transition.

    @param p  Normalized time value [0, 1]
    @returns  Eased value [0, 1] with bouncing
*/
template<class T>
T easeBounceInOut (T p)
{
    if (p < 0.5)
        return T(0.5 * easeBounceIn (p * 2));
    else
        return T(0.5 * easeBounceOut (p * 2 - 1) + 0.5);
}

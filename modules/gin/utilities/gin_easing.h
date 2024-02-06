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

// Modeled after the line y = x
template<class T>
T easeLinear (T p)
{
    return p;
}

// Modeled after the parabola y = x^2
template<class T>
T easeQuadraticIn (T p)
{
    return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
template<class T>
T easeQuadraticOut (T p)
{
    return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
template<class T>
T easeQuadraticInOut (T p)
{
    if (p < 0.5)
        return 2 * p * p;
    else
        return (-2 * p * p) + (4 * p) - 1;
}

// Modeled after the cubic y = x^3
template<class T>
T easeCubicIn (T p)
{
    return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
template<class T>
T easeCubicOut (T p)
{
    T f = (p - 1);
    return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
template<class T>
T easeCubicInOut (T p)
{
    if (p < 0.5)
        return 4 * p * p * p;

    T f = ((2 * p) - 2);
    return 0.5 * f * f * f + 1;
}

// Modeled after the quartic x^4
template<class T>
T easeQuarticIn (T p)
{
    return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
template<class T>
T easeQuarticOut (T p)
{
    T f = (p - 1);
    return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
template<class T>
T easeQuarticInOut (T p)
{
    if (p < 0.5)
        return 8 * p * p * p * p;

    T f = (p - 1);
    return -8 * f * f * f * f + 1;
}

// Modeled after the quintic y = x^5
template<class T>
T easeQuinticIn (T p)
{
    return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
template<class T>
T easeQuinticOut (T p)
{
    T f = (p - 1);
    return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
template<class T>
T easeQuinticInOut (T p)
{
    if (p < 0.5)
        return 16 * p * p * p * p * p;

    T f = ((2 * p) - 2);
    return  0.5 * f * f * f * f * f + 1;
}

// Modeled after quarter-cycle of sine wave
template<class T>
T easeSineIn (T p)
{
    return std::sin ((p - 1) * (juce::MathConstants<T>::pi / 2)) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
template<class T>
T easeSineOut (T p)
{
    return std::sin (p * juce::MathConstants<T>::pi / 2);
}

// Modeled after half sine wave
template<class T>
T easeSineInOut (T p)
{
    return T (0.5) * (1 - std::cos (p * juce::MathConstants<T>::pi));
}

// Modeled after shifted quadrant IV of unit circle
template<class T>
T easeCircularIn (T p)
{
    return 1 - std::sqrt (1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
template<class T>
T easeCircularOut (T p)
{
    return std::sqrt ((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
template<class T>
T easeCircularInOut (T p)
{
    if (p < 0.5)
        return 0.5 * (1 - std::sqrt (1 - 4 * (p * p)));
    else
        return 0.5 * (std::sqrt (-((2 * p) - 3) * ((2 * p) - 1)) + 1);
}

// Modeled after the exponential function y = 2^(10(x - 1))
template<class T>
T easeExponentialIn (T p)
{
    return T ((p == 0.0) ? p : std::pow (2, 10 * (p - 1)));
}

// Modeled after the exponential function y = -2^(-10x) + 1
template<class T>
T easeExponentialOut (T p)
{
    return T ((p == 1.0) ? p : 1 - std::pow (2, -10 * p));
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
template<class T>
T easeExponentialInOut (T p)
{
    if (p == 0.0 || p == 1.0) return p;

    if (p < 0.5)
        return T (0.5 * std::pow (2, (20 * p) - 10));
    else
        return T (-0.5 * std::pow (2, (-20 * p) + 10) + 1);
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
template<class T>
T easeElasticIn (T p)
{
    return std::sin (13 * (juce::MathConstants<T>::pi / 2) * p) * std::pow (2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
template<class T>
T easeElasticOut (T p)
{
    return std::sin (-13 * (juce::MathConstants<T>::pi / 2) * (p + 1)) * std::pow (2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
template<class T>
T easeElasticInOut (T p)
{
    if (p < 0.5)
        return 0.5 * std::sin (13 * (juce::MathConstants<T>::pi / 2) * (2 * p)) * std::pow (2, 10 * ((2 * p) - 1));
    else
        return 0.5 * (std::sin (-13 * (juce::MathConstants<T>::pi / 2) * ((2 * p - 1) + 1)) * std::pow (2, -10 * (2 * p - 1)) + 2);
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
template<class T>
T easeBackIn (T p)
{
    return p * p * p - p * std::sin (p * juce::MathConstants<T>::pi);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
template<class T>
T easeBackOut (T p)
{
    T f = (1 - p);
    return 1 - (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
template<class T>
T easeBackInOut (T p)
{
    if (p < 0.5)
    {
        T f = 2 * p;
        return 0.5 * (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi));
    }
    else
    {
        T f = (1 - (2*p - 1));
        return 0.5 * (1 - (f * f * f - f * std::sin (f * juce::MathConstants<T>::pi))) + 0.5;
    }
}

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

template<class T>
T easeBounceIn (T p)
{
    return 1 - easeBounceOut (1 - p);
}

template<class T>
T easeBounceInOut (T p)
{
    if (p < 0.5)
        return T(0.5 * easeBounceIn (p * 2));
    else
        return T(0.5 * easeBounceOut (p * 2 - 1) + 0.5);
}

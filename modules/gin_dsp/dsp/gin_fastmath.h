/*
MIT License

Copyright (c) 2023 Mike Jarmy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

//------------------------------------------------------------------------------
/**
    Fast approximations for trigonometric and hyperbolic functions.

    FastMath provides optimized approximations of trigonometric and hyperbolic
    functions that trade accuracy for speed. These are useful in DSP applications
    where perceptual accuracy is more important than mathematical precision,
    such as waveshaping, modulation, and real-time synthesis.

    Key Features:
    - Fast sine approximation (valid range: -π to π)
    - Fast hyperbolic tangent approximation
    - Template class supports float or double
    - Inline functions for maximum performance
    - Suitable for audio-rate processing

    Accuracy vs Speed:
    These approximations are significantly faster than std:: equivalents but
    introduce small errors. For most audio applications, the errors are
    inaudible and the speed benefit is worthwhile.

    Usage:
    @code
    // Fast sine for LFO or modulation
    float phase = 0.5f; // 0 to 1
    float x = (phase * 2.0f - 1.0f) * M_PI; // Convert to -π to π
    float sineValue = FastMath<float>::fastSin(x);

    // Fast tanh for waveshaping/saturation
    float input = audioSample * 3.0f;
    float shaped = FastMath<float>::fastTanh(input);
    @endcode

    Credits: Based on algorithms by Mike Jarmy (MIT License)

    @see math namespace
*/
template <class F> class FastMath
{
public:
    /**
        Fast sine wave approximation.

        Computes an approximation of sin(x) using a polynomial approximation.
        Provides good accuracy with significantly better performance than std::sin.

        Valid Range: -π to π
        - Values outside this range will produce incorrect results
        - For normalized phase (0 to 1), convert to radians first

        Algorithm: Based on parabolic approximation with extra precision term.

        @param x Angle in radians (must be in range [-π, π])
        @return Approximation of sin(x) in range [-1.0, 1.0]

        @see https://web.archive.org/web/20100613230051/http://www.devmaster.net/forums/showthread.php?t=5784
        @see https://www.desmos.com/calculator/f0eryaepsl
    */
    static inline F fastSin (F x)
    {
        static constexpr F B = F (4 / juce::MathConstants<double>::pi);
        static constexpr F C = F (-4 / (juce::MathConstants<double>::pi * juce::MathConstants<double>::pi));
        static constexpr F P = F (0.225);

        F y = B * x + C * x * std::abs(x);

        // Extra precision.
        y = P * (y * std::abs(y) - y) + y;

        return y;
    }

    /**
        Fast hyperbolic tangent approximation.

        Computes an approximation of tanh(x) using a rational polynomial.
        Particularly useful for waveshaping and soft-clipping in audio applications.

        Characteristics:
        - Smooth S-curve from -1 to 1
        - Asymptotically approaches ±1 for large |x|
        - Good for soft saturation and distortion
        - Much faster than std::tanh
        - Valid for all input ranges

        Algorithm: Rational polynomial approximation optimized for audio.

        @param x Input value (any real number)
        @return Approximation of tanh(x) in range [-1.0, 1.0]

        @see https://www.kvraudio.com/forum/viewtopic.php?p=5447225#p5447225
        @see https://www.desmos.com/calculator/bjc7zsl4ek
    */
    static inline F fastTanh (const F x)
    {
        const F ax = std::abs(x);
        const F x2 = x * x;
        const F z =
            x * (0.773062670268356 + ax +
                 (0.757118539838817 + 0.0139332362248817 * x2 * x2) * x2 * ax);

        return z / (0.795956503022967 + std::abs(z));
    }

private:
    FastMath() {}
    ~FastMath() {}

    FastMath (const FastMath&) = delete;
    FastMath& operator= (const FastMath&) = delete;
};

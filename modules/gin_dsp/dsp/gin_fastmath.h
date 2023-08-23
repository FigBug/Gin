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
// FastMath contains some fast approximations for trigonometric functions.
//------------------------------------------------------------------------------

template <class F> class FastMath
{
public:
    // Sine approximation. Range is [-pi, pi].
    //
    // https://web.archive.org/web/20100613230051/http://www.devmaster.net/forums/showthread.php?t=5784
    // https://www.desmos.com/calculator/f0eryaepsl
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

    // Hyperbolic tangent approximation.
    //
    // https://www.kvraudio.com/forum/viewtopic.php?p=5447225#p5447225
    // https://www.desmos.com/calculator/bjc7zsl4ek
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

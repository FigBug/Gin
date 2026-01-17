/*
  ==============================================================================

    Catenary.h
    Created: 30 Jan 2021 7:26:28pm
    Author:  Adrian Pflugshaupt

    Calculates points on a catenary curve between two 2d points
    with addLength "rope" added to the distance.

    (c) 2021 Adrian Pflugshaupt

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
  ==============================================================================
*/

#pragma once

//==============================================================================
/** Calculates the curve of a hanging cable (catenary).

    A catenary is the natural curve formed by a cable, rope, or chain hanging between
    two points under its own weight. This class calculates the mathematical catenary
    curve and provides methods to query points along it.

    Common uses:
    - Drawing realistic cable connections in modular synthesizer UIs
    - Visualizing hanging wires, chains, or ropes
    - Creating natural-looking curves between two points

    The curve is defined by two endpoints and the additional length of cable beyond
    the straight-line distance between the points.

    @see calculate, calcY, getVertex
*/
class Catenary
{
public:
    /** Creates a catenary curve between two points.

        @param x0             X coordinate of the first endpoint
        @param y0             Y coordinate of the first endpoint
        @param x1             X coordinate of the second endpoint
        @param y1             Y coordinate of the second endpoint
        @param addLength      Additional cable length beyond the straight-line distance
        @param numIterations  Number of Newton-Raphson iterations for precision (default: 1)
        @param screenCoords   If true, uses screen coordinates (Y increases downward, 0,0 at top-left).
                              If false (default), uses math coordinates (Y increases upward).
    */
    Catenary (float x0, float y0, float x1, float y1, float addLength, int numIterations = 1, bool screenCoords = false)
        : useScreenCoords (screenCoords)
    {
        calculate (x0, y0, x1, y1, addLength, numIterations);
    }

    /** Calculates or recalculates the catenary curve between two points.

        The addLength parameter determines how much "slack" the cable has. A value of 0
        would be a straight line (impossible for a real cable). Larger values create
        more pronounced sag.

        @param x0             X coordinate of the first endpoint
        @param y0             Y coordinate of the first endpoint
        @param x1             X coordinate of the second endpoint
        @param y1             Y coordinate of the second endpoint
        @param addLength      Additional cable length beyond the straight-line distance (must be > 0)
        @param numIterations  Number of Newton-Raphson iterations for precision (default: 1)
    */
    void calculate (float x0, float y0, float x1, float y1, float addLength, int numIterations = 1)
    {
        jassert (! juce::exactlyEqual (x0, x1));
        jassert (addLength > 0.0f);

        // For screen coordinates, flip Y to work in math space, then flip back in calcY
        if (useScreenCoords)
        {
            y0 = -y0;
            y1 = -y1;
        }

        // make sure x1 is right
        if (x0 > x1)
        {
            std::swap (x0, x1);
            std::swap (y0, y1);
        }

        auto x_d = x1 - x0;
        auto y_d = y1 - y0;

        auto d = std::hypot (x_d, y_d);
        auto L = d + addLength;

        auto x_f = std::sqrt (L * L - y_d * y_d) / x_d; // always > 1

        // need to solve sinh(xi)/xi - x_f = 0

        // auto xi_approx = 1.15f * std::log (x_f - 0.8f) + 1.9f; // manual fit for xi>2, guaranteed too low.
        auto xi_approx = 1.16f * std::log (x_f - 0.75f) + 1.9f; // manual fit for large 2<xi<10..
        if (xi_approx < 2.0f)
        {
            // taylor-approx: 1 + xi^2/6 + xi^4/120 + xi^6/5040 - x_f = 0
            // use inverse taylor series for smallish values. Always slightly too high.
            auto tmp = std::cbrt (std::sqrt (15680.0f) * std::sqrt (x_f * (405.0f * x_f + 198.0f) + 62.0f) + 2520.0f * x_f + 616.0f);
            xi_approx = std::sqrt (tmp - 84.0f / tmp - 14.0f);
        }

        auto xi = xi_approx;
        for (auto n = 0; n < numIterations; ++n)
        { // newton iterations to improve precision
            auto x = xi;
            auto exp_half = 0.5f * std::exp (x); // sinh and cosh at the same time from exp
            auto iexp_half = 0.25f / exp_half;
            auto sinhx = exp_half - iexp_half;
            auto coshx = exp_half + iexp_half;
            auto val = sinhx / x - x_f;
            auto der = (coshx * x - sinhx) / (x * x);
            xi -= val / der;
        }

        a = 0.5f * x_d / xi;
        inva = 1.0f / a;
        b = 0.5f * (x0 + x1) - a*std::asinh (0.5f * y_d / (a * std::sinh (xi)));
        c = y0 - (a * std::cosh ((x0 - b) * inva));
    }

    /** Calculates the y coordinate for a given x position on the catenary curve.

        Use this to sample points along the curve for drawing.

        @param x  The x coordinate
        @returns  The y coordinate at that x position
    */
    float calcY (float x) const
    {
        auto y = a * std::cosh ((x - b) * inva) + c;
        return useScreenCoords ? -y : y;
    }

    /** Returns the vertex (lowest/highest point) of the catenary curve.

        In math coordinates, this is the lowest point. In screen coordinates,
        this is the highest point visually (largest Y value).

        @returns  A pair containing (x, y) coordinates of the vertex
    */
    std::pair<float, float> getVertex() const
    {
        auto y = a + c;
        return {b, useScreenCoords ? -y : y};
    }

private:
    float a;     // Curvature parameter = radius of the osculating circle at the vertex
    float inva;  // Inverse of a (cached to avoid division per point)
    float b;     // X offset (x position of the vertex)
    float c;     // Y offset (note that cosh(0) = 1)
    bool useScreenCoords = false;  // If true, Y axis is flipped for screen coordinates
};

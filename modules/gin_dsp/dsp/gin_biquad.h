/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Single RBJ biquad, per sample.

    A small, self contained second order section for the places a filter is
    part of an algorithm rather than a thing the user configures - detector
    paths, side chain shaping, band limiting inside an effect. Filter is the
    one to reach for when a plugin exposes a filter to the user: it handles
    channels, slopes and modulation. This one is a struct with five
    coefficients and four state variables, and can sit inside a loop.

    Coefficients follow Robert Bristow-Johnson's cookbook.

    Usage:
    @code
    Biquad hp;
    hp.setHighpass (80.0f, 0.707f, sampleRate);

    for (int i = 0; i < numSamples; i++)
        data[i] = hp.process (data[i]);
    @endcode

    Note: one instance holds the state for one channel.

    @see Filter, EQ
*/
struct Biquad
{
    void setLowpass (double freq, double q, double sampleRate)
    {
        auto [w0, alpha, a0] = common (freq, q, sampleRate);
        auto cosw0 = std::cos (w0);

        set (((1.0 - cosw0) / 2.0) / a0,
             (1.0 - cosw0) / a0,
             ((1.0 - cosw0) / 2.0) / a0,
             (-2.0 * cosw0) / a0,
             (1.0 - alpha) / a0);
    }

    void setHighpass (double freq, double q, double sampleRate)
    {
        auto [w0, alpha, a0] = common (freq, q, sampleRate);
        auto cosw0 = std::cos (w0);

        set (((1.0 + cosw0) / 2.0) / a0,
             (-(1.0 + cosw0)) / a0,
             ((1.0 + cosw0) / 2.0) / a0,
             (-2.0 * cosw0) / a0,
             (1.0 - alpha) / a0);
    }

    void setBandpass (double freq, double q, double sampleRate)
    {
        auto [w0, alpha, a0] = common (freq, q, sampleRate);

        set (alpha / a0,
             0.0,
             -alpha / a0,
             (-2.0 * std::cos (w0)) / a0,
             (1.0 - alpha) / a0);
    }

    /** Peaking EQ. Positive gainDb boosts around freq, negative cuts. */
    void setPeak (double freq, double q, double gainDb, double sampleRate)
    {
        auto A = std::pow (10.0, gainDb / 40.0);
        auto w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        auto alpha = std::sin (w0) / (2.0 * q);
        auto a0 = 1.0 + alpha / A;

        set ((1.0 + alpha * A) / a0,
             (-2.0 * std::cos (w0)) / a0,
             (1.0 - alpha * A) / a0,
             (-2.0 * std::cos (w0)) / a0,
             (1.0 - alpha / A) / a0);
    }

    /** Passes everything through unchanged. */
    void setBypass()
    {
        set (1.0, 0.0, 0.0, 0.0, 0.0);
    }

    float process (float x)
    {
        auto y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;

        x2 = x1; x1 = x;
        y2 = y1; y1 = y;

        return y;
    }

    void reset()
    {
        x1 = x2 = y1 = y2 = 0.0f;
    }

    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f, a1 = 0.0f, a2 = 0.0f;
    float x1 = 0.0f, x2 = 0.0f, y1 = 0.0f, y2 = 0.0f;

private:
    static std::tuple<double, double, double> common (double freq, double q, double sampleRate)
    {
        auto w0 = 2.0 * juce::MathConstants<double>::pi * freq / sampleRate;
        auto alpha = std::sin (w0) / (2.0 * q);

        return { w0, alpha, 1.0 + alpha };
    }

    void set (double b0_, double b1_, double b2_, double a1_, double a2_)
    {
        b0 = float (b0_); b1 = float (b1_); b2 = float (b2_);
        a1 = float (a1_); a2 = float (a2_);
    }
};

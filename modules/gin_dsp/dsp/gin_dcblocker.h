/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


/**
    DC blocking filter (high-pass filter) to remove DC offset.

    DCBlocker implements a simple first-order high-pass filter to remove DC
    offset from audio signals. This is useful after certain DSP operations
    (like waveshaping or oscillators) that may introduce DC bias.

    Key Features:
    - Configurable cutoff frequency (default 10 Hz)
    - First-order IIR filter
    - Very low CPU usage
    - Reset for clearing filter state

    Usage:
    @code
    DCBlocker blocker;
    blocker.setSampleRate(44100.0f);
    blocker.setCutoff(10.0f); // Block below 10 Hz

    // Per-sample processing
    float output = blocker.process(input);
    @endcode

    Note: This is a per-sample filter. For stereo, use two instances.

    @see WTOscillator
*/
class DCBlocker
{
public:
    DCBlocker() {}
    ~DCBlocker() {}

    void setSampleRate (float sampleRate_)
    {
        sampleRate = sampleRate_;
        recalc();
    }

    void setCutoff (float cutoff_ /* Hz */)
    {
        cutoff = cutoff_;
        recalc();
    }

    float process (float x)
    {
        z = x * a + z * b;
        return z;
    }

    void reset()
    {
        a = 0;
        b = 0;
        z = 0;
    }

private:
    float sampleRate = 1;
    float cutoff = 10.0f;

    float a = 0;
    float b = 0;
    float z = 0;

    void recalc()
    {
        b = std::exp (-2.0f * juce::MathConstants<float>::pi * cutoff / sampleRate);
        a = 1.0f - b;
    }
};

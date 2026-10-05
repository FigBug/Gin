/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Stereo phase correlation and balance meter.

    Correlation is the normalised cross-correlation of left and right at zero
    lag: +1 for identical channels (mono), 0 for unrelated signals, -1 for
    identical but inverted channels. Balance is derived from the RMS of each
    side: -1 is all left, +1 all right. Both are averaged with a one pole
    filter whose time constant is set by setWindow().

    Call process() from the audio thread; the getters are safe from any thread.
*/
class CorrelationMeter
{
public:
    CorrelationMeter() = default;

    void prepare (double sampleRate, double windowSeconds = 0.3);

    /** Averaging time constant. Realtime safe. */
    void setWindow (double windowSeconds);
    double getWindowSeconds() const     { return windowSeconds; }

    /** Clears the averages. Safe from any thread. */
    void reset();

    void process (const float* left, const float* right, int numSamples);
    /** Uses the first two channels. A mono buffer reads as fully correlated. */
    void process (const juce::AudioBuffer<float>& buffer);

    /** -1 to +1. Returns 0 when both channels are silent. */
    float getCorrelation() const        { return correlation.load (std::memory_order_relaxed); }
    /** -1 (left only) to +1 (right only). 0 when balanced or silent. */
    float getBalance() const            { return balance.load (std::memory_order_relaxed); }

private:
    double sampleRate = 44100.0, windowSeconds = 0.3;
    std::atomic<float> coefficient { 0.0f };

    double ll = 0.0, rr = 0.0, lr = 0.0;
    std::atomic<float> correlation { 0.0f }, balance { 0.0f };
    std::atomic<bool> resetPending { false };
};

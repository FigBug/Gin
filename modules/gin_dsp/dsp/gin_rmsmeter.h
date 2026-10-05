/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Sliding window RMS meter, one reading per channel.

    The window length is fixed at prepare() time. Readings are plain RMS, so a
    full scale sine reads -3.01 dBFS; add 3.01 dB for an AES-17 style scale
    where a full scale sine reads 0.

    Call process() from the audio thread; the getters are safe from any thread.
*/
class RMSMeter
{
public:
    RMSMeter() = default;

    /** Not realtime safe. */
    void prepare (double sampleRate, int numChannels, double windowSeconds = 0.3);

    /** Clears the window. Safe from any thread. */
    void reset();

    void process (const juce::AudioBuffer<float>& buffer);

    /** RMS of the window, dBFS. */
    float getRMS (int channel) const;
    /** RMS of the window, linear. */
    float getRMSLinear (int channel) const;

    int getNumChannels() const      { return numChannels; }
    double getWindowSeconds() const { return windowSeconds; }

    static constexpr float silence = -100.0f;

private:
    struct Channel
    {
        std::vector<float> squares;
        double sum = 0.0;
        int pos = 0, filled = 0;
        std::atomic<float> rms { 0.0f };
    };

    std::vector<std::unique_ptr<Channel>> channels;
    int numChannels = 0;
    double windowSeconds = 0.3;
    std::atomic<bool> resetPending { false };
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** The two stage K-weighting filter from ITU-R BS.1770: a high shelf that
    models the acoustic effect of the head followed by a high pass. Processes
    audio in place, one filter state per channel.
*/
class KWeightingFilter
{
public:
    struct Coefficients
    {
        double b0 = 1.0, b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0;
    };

    /** The pre-filter (high shelf) for a given sample rate. */
    static Coefficients stage1Coefficients (double sampleRate);
    /** The RLB weighting (high pass) for a given sample rate. */
    static Coefficients stage2Coefficients (double sampleRate);

    void prepare (double sampleRate, int numChannels);
    void reset();

    void process (juce::AudioBuffer<float>& buffer);
    void process (float* samples, int numSamples, int channel);

    int getNumChannels() const      { return int (stage1.size()); }

private:
    struct Biquad
    {
        Coefficients c;
        double z1 = 0.0, z2 = 0.0;

        inline float process (float x) noexcept
        {
            const double y = c.b0 * x + z1;
            z1 = c.b1 * x - c.a1 * y + z2;
            z2 = c.b2 * x - c.a2 * y;
            return float (y);
        }
    };

    std::vector<Biquad> stage1, stage2;
};

//==============================================================================
/** Loudness meter per ITU-R BS.1770-4 / EBU R128.

    Call process() from the audio thread. The momentary (400 ms) and short-term
    (3 s) values are updated every 100 ms and can be read from any thread.
    Integrated loudness and loudness range are gated measurements over the whole
    history since the last reset; they are computed on demand from that history
    by getIntegrated() / getLoudnessRange(), which are intended to be called
    from the message thread as they do a little work.

    Channel weighting follows the standard: 1.0 for the first three channels
    (L, R, C) and 1.41 for channels 4 and 5 (Ls, Rs).

    Values are in LUFS (LU for the range). Below the measurable floor, or before
    any audio has been processed, getters return `silence`.
*/
class LoudnessMeter
{
public:
    LoudnessMeter() = default;

    /** maxHistorySeconds bounds the memory used for the integrated / range
        histories (about 40 kB per hour). Not realtime safe. */
    void prepare (double sampleRate, int numChannels, double maxHistorySeconds = 24.0 * 60.0 * 60.0);

    /** Clears everything, including the integrated history. Safe to call from
        any thread; takes effect on the next process() call. */
    void reset();

    /** Audio thread. Uses up to the prepared number of channels. */
    void process (const juce::AudioBuffer<float>& buffer);

    float getMomentary() const          { return momentary.load (std::memory_order_relaxed); }
    float getShortTerm() const          { return shortTerm.load (std::memory_order_relaxed); }
    float getMaxMomentary() const       { return maxMomentary.load (std::memory_order_relaxed); }
    float getMaxShortTerm() const       { return maxShortTerm.load (std::memory_order_relaxed); }

    /** Gated integrated loudness since the last reset. Message thread. */
    float getIntegrated() const;
    /** Loudness range (EBU Tech 3342) since the last reset, in LU. Message thread. */
    float getLoudnessRange() const;

    /** Seconds of audio measured since the last reset. */
    double getElapsedSeconds() const    { return double (blocksProcessed.load (std::memory_order_relaxed)) * 0.1; }

    void setChannelWeight (int channel, float weight);

    static constexpr float silence = -100.0f;

    /** Converts a mean square power to loudness. */
    static float powerToLoudness (double power);
    static double loudnessToPower (float loudness);

private:
    void finishBlock();
    void applyReset();

    KWeightingFilter filter;
    juce::AudioBuffer<float> weighted;

    double sampleRate = 44100.0;
    int numChannels = 0;
    std::vector<float> channelWeights;

    // Current 100 ms block
    int samplesPerBlock = 4410, samplesInBlock = 0;
    std::vector<double> blockSumSquares;

    // Ring of the last 30 block powers (3 s) for momentary and short-term
    static constexpr int ringSize = 30;
    double ring[ringSize] = {};
    int ringPos = 0, ringCount = 0;

    // Histories for the gated measurements: momentary power every 100 ms,
    // short-term power every 1 s. Preallocated, written by the audio thread,
    // read by the message thread up to the published count.
    std::vector<float> momentaryHistory, shortTermHistory;
    std::atomic<int> momentaryCount { 0 }, shortTermCount { 0 };

    std::atomic<int> blocksProcessed { 0 };
    std::atomic<float> momentary { silence }, shortTerm { silence }, maxMomentary { silence }, maxShortTerm { silence };
    std::atomic<bool> resetPending { false };
};

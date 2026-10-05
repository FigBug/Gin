/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** True peak meter per ITU-R BS.1770 Annex 2: the signal is oversampled
    (4x below 96 kHz, 2x below 192 kHz) so peaks that fall between samples are
    measured. Reports the peak of the most recent block and the highest peak
    held since reset, per channel, in dBTP.

    Call process() from the audio thread; the getters are safe from any thread.
*/
class TruePeakMeter
{
public:
    TruePeakMeter() = default;

    /** Not realtime safe. */
    void prepare (double sampleRate, int numChannels, int maxBlockSize);

    /** Clears the held peaks. Safe from any thread. */
    void reset();

    void process (const juce::AudioBuffer<float>& buffer);

    /** Peak of the last processed block, dBTP. */
    float getTruePeak (int channel) const;
    /** Highest peak since reset, dBTP. */
    float getMaxTruePeak (int channel) const;
    /** Highest peak since reset across all channels, dBTP. */
    float getMaxTruePeak() const;

    int getNumChannels() const      { return numChannels; }
    int getOversamplingFactor() const;

    static constexpr float silence = -100.0f;

private:
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    int numChannels = 0, maxBlockSize = 0;

    std::unique_ptr<std::atomic<float>[]> blockPeak, maxPeak;
    std::atomic<bool> resetPending { false };
};

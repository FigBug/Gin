/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void TruePeakMeter::prepare (double sampleRate, int channels, int blockSize)
{
    numChannels  = channels;
    maxBlockSize = juce::jmax (1, blockSize);

    const int factorLog2 = sampleRate < 96000.0 ? 2 : (sampleRate < 192000.0 ? 1 : 0);

    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (size_t (juce::jmax (1, channels)), size_t (factorLog2),
                                                                      juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple, true, false);
    oversampling->initProcessing (size_t (maxBlockSize));

    blockPeak.reset (new std::atomic<float>[size_t (juce::jmax (1, channels))]);
    maxPeak.reset (new std::atomic<float>[size_t (juce::jmax (1, channels))]);

    for (int ch = 0; ch < juce::jmax (1, channels); ch++)
    {
        blockPeak[size_t (ch)].store (silence, std::memory_order_relaxed);
        maxPeak[size_t (ch)].store (silence, std::memory_order_relaxed);
    }
}

int TruePeakMeter::getOversamplingFactor() const
{
    return oversampling != nullptr ? int (oversampling->getOversamplingFactor()) : 1;
}

void TruePeakMeter::reset()
{
    resetPending.store (true, std::memory_order_release);
}

void TruePeakMeter::process (const juce::AudioBuffer<float>& buffer)
{
    if (oversampling == nullptr || numChannels == 0)
        return;

    if (resetPending.exchange (false, std::memory_order_acq_rel))
        for (int ch = 0; ch < numChannels; ch++)
            maxPeak[size_t (ch)].store (silence, std::memory_order_relaxed);

    const int channels = juce::jmin (numChannels, buffer.getNumChannels());
    const int total    = buffer.getNumSamples();

    float peaks[64] = {};
    const int tracked = juce::jmin (channels, 64);

    int pos = 0;
    while (pos < total)
    {
        const int n = juce::jmin (total - pos, maxBlockSize);

        const float* ptrs[64];
        for (int ch = 0; ch < tracked; ch++)
            ptrs[ch] = buffer.getReadPointer (ch, pos);

        juce::dsp::AudioBlock<const float> input (ptrs, size_t (tracked), size_t (n));
        auto up = oversampling->processSamplesUp (input);

        for (int ch = 0; ch < tracked; ch++)
        {
            const auto range = juce::FloatVectorOperations::findMinAndMax (up.getChannelPointer (size_t (ch)), int (up.getNumSamples()));
            peaks[ch] = juce::jmax (peaks[ch], std::abs (range.getStart()), std::abs (range.getEnd()));
        }

        pos += n;
    }

    for (int ch = 0; ch < tracked; ch++)
    {
        const float db = juce::Decibels::gainToDecibels (peaks[ch], silence);
        blockPeak[size_t (ch)].store (db, std::memory_order_relaxed);

        if (db > maxPeak[size_t (ch)].load (std::memory_order_relaxed))
            maxPeak[size_t (ch)].store (db, std::memory_order_relaxed);
    }
}

float TruePeakMeter::getTruePeak (int channel) const
{
    if (! juce::isPositiveAndBelow (channel, numChannels))
        return silence;

    return blockPeak[size_t (channel)].load (std::memory_order_relaxed);
}

float TruePeakMeter::getMaxTruePeak (int channel) const
{
    if (! juce::isPositiveAndBelow (channel, numChannels))
        return silence;

    return maxPeak[size_t (channel)].load (std::memory_order_relaxed);
}

float TruePeakMeter::getMaxTruePeak() const
{
    float m = silence;
    for (int ch = 0; ch < numChannels; ch++)
        m = juce::jmax (m, getMaxTruePeak (ch));
    return m;
}

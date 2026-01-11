/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Multi-band graphic equalizer with configurable band count and per-channel control.

    GraphicEQ provides a professional graphic equalizer with selectable band counts
    (5, 7, 10, 15, or 31 bands) using ISO standard center frequencies. Each channel
    can have independent gain settings, and frequency response can be queried per channel.

    Key Features:
    - Selectable band count (5, 7, 10, 15, or 31 bands)
    - Configurable number of channels
    - Independent gain control per channel per band
    - Per-channel frequency response calculation
    - Uses gin::Filter internally for high-quality processing

    Band Configurations:
    - 5 bands:  60, 250, 1k, 4k, 16k Hz
    - 7 bands:  63, 160, 400, 1k, 2.5k, 6.3k, 16k Hz
    - 10 bands: 31.5, 63, 125, 250, 500, 1k, 2k, 4k, 8k, 16k Hz
    - 15 bands: 25, 40, 63, 100, 160, 250, 400, 630, 1k, 1.6k, 2.5k, 4k, 6.3k, 10k, 16k Hz
    - 31 bands: ISO 1/3 octave from 20Hz to 20kHz

    Usage:
    @code
    GraphicEQ eq(GraphicEQ::Bands31);
    eq.setNumChannels(2);
    eq.setSampleRate(44100.0);

    eq.setBandGain(0, 0, 3.0f);   // Channel 0, Band 0: +3dB
    eq.setBandGain(1, 0, -3.0f);  // Channel 1, Band 0: -3dB

    eq.process(audioBuffer);

    // Get frequency response for channel 0 at 1kHz
    float response = eq.getFrequencyResponse(0, 1000.0f);
    @endcode

    @see EQ, Filter
*/
class GraphicEQ
{
public:
    /** Number of bands configuration */
    enum NumBands
    {
        Bands5 = 5,
        Bands7 = 7,
        Bands10 = 10,
        Bands15 = 15,
        Bands31 = 31
    };

    /** Creates a graphic equalizer with the specified number of bands.
        @param bands  Number of bands (5, 7, 10, 15, or 31)
    */
    GraphicEQ (NumBands bands = Bands31)
        : numBands (bands)
    {
        initializeFrequencies();
    }

    /** Sets the sample rate for processing.
        @param sampleRate  Sample rate in Hz
    */
    void setSampleRate (double sr)
    {
        sampleRate = sr;

        for (auto& channelFilters : filters)
            for (auto& filter : channelFilters)
                filter.setSampleRate (sr);
    }

    /** Sets the maximum block size for processing.
        Call this before processing to allocate working buffers.
        @param blockSize  Maximum number of samples per block
    */
    void setBlockSize (int blockSize)
    {
        tempBuffer.setSize (1, blockSize);
        accumulator.setSize (1, blockSize);
    }

    /** Sets the number of audio channels.
        @param ch  Number of channels
    */
    void setNumChannels (int ch)
    {
        if (numChannels == ch)
            return;

        numChannels = ch;
        filters.resize (static_cast<size_t> (ch));
        bandGains.resize (static_cast<size_t> (ch));

        for (int c = 0; c < ch; ++c)
        {
            filters[static_cast<size_t> (c)].resize (static_cast<size_t> (numBands));
            bandGains[static_cast<size_t> (c)].resize (static_cast<size_t> (numBands), 0.0f);

            for (int b = 0; b < numBands; ++b)
            {
                auto& filter = filters[static_cast<size_t> (c)][static_cast<size_t> (b)];
                filter.setSampleRate (sampleRate);
                filter.setNumChannels (1);
                filter.setType (Filter::bandpass);
                filter.setSlope (Filter::db12);
                updateBand (c, b);
            }
        }
    }

    /** Gets the number of bands.
        @return  Number of bands
    */
    int getNumBands() const { return numBands; }

    /** Gets the number of channels.
        @return  Number of channels
    */
    int getNumChannels() const { return numChannels; }

    /** Resets the filter state for all bands and channels. */
    void reset()
    {
        for (auto& channelFilters : filters)
            for (auto& filter : channelFilters)
                filter.reset();
    }

    /** Sets the gain for a specific band on a specific channel.
        @param channel  Channel index
        @param band     Band index
        @param gainDb   Gain in decibels (positive = boost, negative = cut)
    */
    void setBandGain (int channel, int band, float gainDb)
    {
        jassert (channel >= 0 && channel < numChannels);
        jassert (band >= 0 && band < numBands);

        bandGains[static_cast<size_t> (channel)][static_cast<size_t> (band)] = gainDb;
        updateBand (channel, band);
    }

    /** Gets the gain for a specific band on a specific channel.
        @param channel  Channel index
        @param band     Band index
        @return         Gain in decibels
    */
    float getBandGain (int channel, int band) const
    {
        jassert (channel >= 0 && channel < numChannels);
        jassert (band >= 0 && band < numBands);

        return bandGains[static_cast<size_t> (channel)][static_cast<size_t> (band)];
    }

    /** Gets the center frequency for a specific band.
        @param band  Band index
        @return      Center frequency in Hz
    */
    float getBandFrequency (int band) const
    {
        jassert (band >= 0 && band < numBands);
        return bandFrequencies[static_cast<size_t> (band)];
    }

    /** Sets the gain for all bands on a specific channel.
        @param channel  Channel index
        @param gains    Array of gain values in dB (must have numBands elements)
    */
    void setAllBandGains (int channel, const float* gains)
    {
        jassert (channel >= 0 && channel < numChannels);

        for (int b = 0; b < numBands; ++b)
        {
            bandGains[static_cast<size_t> (channel)][static_cast<size_t> (b)] = gains[b];
            updateBand (channel, b);
        }
    }

    /** Sets all bands on a specific channel to 0 dB (flat response).
        @param channel  Channel index
    */
    void flatten (int channel)
    {
        jassert (channel >= 0 && channel < numChannels);

        for (int b = 0; b < numBands; ++b)
        {
            bandGains[static_cast<size_t> (channel)][static_cast<size_t> (b)] = 0.0f;
            updateBand (channel, b);
        }
    }

    /** Sets all bands on all channels to 0 dB (flat response). */
    void flattenAll()
    {
        for (int c = 0; c < numChannels; ++c)
            flatten (c);
    }

    /** Gets the frequency response magnitude for a specific channel at a given frequency.
        @param channel      Channel index
        @param frequency    Frequency in Hz
        @return             Response magnitude (linear scale, 1.0 = unity gain)
    */
    float getFrequencyResponse (int channel, float frequency) const
    {
        jassert (channel >= 0 && channel < numChannels);

        // For parallel filters: response = 1 + sum((gain - 1) * H_bandpass(f))
        float response = 1.0f;

        for (int b = 0; b < numBands; ++b)
        {
            float linearGain = juce::Decibels::decibelsToGain (
                bandGains[static_cast<size_t> (channel)][static_cast<size_t> (b)]);
            float bandResponse = const_cast<Filter&> (
                filters[static_cast<size_t> (channel)][static_cast<size_t> (b)]).getResponseMagnitude (frequency);
            response += (linearGain - 1.0f) * bandResponse;
        }

        return response;
    }

    /** Gets the frequency response in dB for a specific channel at a given frequency.
        @param channel      Channel index
        @param frequency    Frequency in Hz
        @return             Response in decibels
    */
    float getFrequencyResponseDb (int channel, float frequency) const
    {
        return juce::Decibels::gainToDecibels (getFrequencyResponse (channel, frequency));
    }

    /** Processes an audio buffer through the equalizer.
        @param buffer  Audio buffer to process in-place
    */
    void process (juce::AudioSampleBuffer& buffer)
    {
        const int bufferChannels = buffer.getNumChannels();
        const int channelsToProcess = std::min (bufferChannels, numChannels);
        const int numSamples = buffer.getNumSamples();

        jassert (tempBuffer.getNumSamples() >= numSamples);

        for (int c = 0; c < channelsToProcess; ++c)
        {
            float* channelData = buffer.getWritePointer (c);
            accumulator.clear();

            // For each band: y_i = bandpass_i(x), y += gain_i * y_i
            for (int b = 0; b < numBands; ++b)
            {
                // Copy original input to temp buffer
                tempBuffer.copyFrom (0, 0, channelData, numSamples);

                // Filter the original input through this band's bandpass
                float* tempData = tempBuffer.getWritePointer (0);
                juce::AudioSampleBuffer tempView (&tempData, 1, numSamples);
                filters[static_cast<size_t> (c)][static_cast<size_t> (b)].process (tempView);

                // Accumulate: y += (linearGain - 1) * filtered
                // Using (linearGain - 1) so that 0dB gain contributes nothing
                float linearGain = juce::Decibels::decibelsToGain (
                    bandGains[static_cast<size_t> (c)][static_cast<size_t> (b)]);
                accumulator.addFrom (0, 0, tempBuffer, 0, 0, numSamples, linearGain - 1.0f);
            }

            // output = x + y
            buffer.addFrom (c, 0, accumulator, 0, 0, numSamples);
        }
    }

private:
    void initializeFrequencies()
    {
        bandFrequencies.clear();

        switch (numBands)
        {
            case Bands5:
                bandFrequencies = { 60.0f, 250.0f, 1000.0f, 4000.0f, 16000.0f };
                bandQ = 1.4f; // Wider Q for fewer bands
                break;

            case Bands7:
                bandFrequencies = { 63.0f, 160.0f, 400.0f, 1000.0f, 2500.0f, 6300.0f, 16000.0f };
                bandQ = 2.0f;
                break;

            case Bands10:
                bandFrequencies = { 31.5f, 63.0f, 125.0f, 250.0f, 500.0f, 1000.0f, 2000.0f, 4000.0f, 8000.0f, 16000.0f };
                bandQ = 2.9f; // 1 octave spacing
                break;

            case Bands15:
                bandFrequencies = { 25.0f, 40.0f, 63.0f, 100.0f, 160.0f, 250.0f, 400.0f, 630.0f,
                                    1000.0f, 1600.0f, 2500.0f, 4000.0f, 6300.0f, 10000.0f, 16000.0f };
                bandQ = 3.4f; // 2/3 octave spacing
                break;

            case Bands31:
            default:
                bandFrequencies = { 20.0f, 25.0f, 31.5f, 40.0f, 50.0f, 63.0f, 80.0f, 100.0f,
                                    125.0f, 160.0f, 200.0f, 250.0f, 315.0f, 400.0f, 500.0f,
                                    630.0f, 800.0f, 1000.0f, 1250.0f, 1600.0f, 2000.0f, 2500.0f,
                                    3150.0f, 4000.0f, 5000.0f, 6300.0f, 8000.0f, 10000.0f,
                                    12500.0f, 16000.0f, 20000.0f };
                bandQ = 4.318f; // 1/3 octave spacing
                break;
        }
    }

    void updateBand (int channel, int band)
    {
        if (channel >= numChannels || band >= numBands)
            return;

        // Use unity gain in the filter - actual gain is applied in process()
        filters[static_cast<size_t> (channel)][static_cast<size_t> (band)].setParams (
            bandFrequencies[static_cast<size_t> (band)], bandQ, 1.0f);
    }

    int numBands = 31;
    int numChannels = 0;
    double sampleRate = 44100.0;
    float bandQ = 4.318f;

    std::vector<float> bandFrequencies;
    std::vector<std::vector<float>> bandGains;           // [channel][band]
    std::vector<std::vector<Filter>> filters;            // [channel][band]

    // Working buffers for parallel processing
    juce::AudioSampleBuffer tempBuffer;
    juce::AudioSampleBuffer accumulator;
};

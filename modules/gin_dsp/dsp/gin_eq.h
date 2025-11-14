/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Multi-channel, multi-band equalizer with flexible filter types.

    EQ provides a professional multi-band equalizer supporting multiple filter
    types per band. Each band can be configured independently as a shelving filter,
    peak/notch filter, or high/low-pass filter. Supports any number of channels.

    Key Features:
    - Configurable number of bands
    - Multiple filter types per band (lowshelf, highshelf, peak, highpass, lowpass)
    - Multi-channel support
    - Independent frequency, Q, and gain per band
    - Based on high-quality biquad filters

    Filter Types:
    - lowshelf: Boost/cut low frequencies
    - highshelf: Boost/cut high frequencies
    - peak: Boost/cut at specific frequency (bell curve)
    - highpass: Remove frequencies below cutoff
    - lowpass: Remove frequencies above cutoff

    Usage:
    @code
    EQ equalizer(3); // 3-band EQ
    equalizer.setSampleRate(44100.0);
    equalizer.setNumChannels(2); // Stereo

    // Low shelf: boost bass at 100Hz
    equalizer.setParams(0, EQ::lowshelf, 100.0f, 0.7f, 3.0f);

    // Peak: cut at 1kHz
    equalizer.setParams(1, EQ::peak, 1000.0f, 2.0f, -6.0f);

    // High shelf: boost treble at 8kHz
    equalizer.setParams(2, EQ::highshelf, 8000.0f, 0.7f, 2.0f);

    equalizer.process(audioBuffer);
    @endcode

    @see Filter
*/
class EQ
{
public:
    enum Type
    {
        lowshelf,
        highshelf,
        peak,
        highpass,
        lowpass,
    };

    EQ (int numBands)
    {
        for (int i = 0; i < numBands; i++)
            bands.add (new Filter());
    }

    void setSampleRate (double sr)
    {
        for (auto b : bands)
            b->setSampleRate (sr);
    }

    void setNumChannels (int ch)
    {
        for (auto b : bands)
            b->setNumChannels (ch);
    }

    void reset()
    {
        for (auto& b : bands)
            b->reset();
    }

    void setParams (int band, Type t, float freq, float q, float g)
    {
        auto map = [] (Type tIn)
        {
            switch (tIn)
            {
                case lowshelf:  return Filter::lowshelf;
                case highshelf: return Filter::highshelf;
                case lowpass:   return Filter::lowpass;
                case highpass:  return Filter::highpass;
                case peak:      return Filter::peak;
                default:
                    jassertfalse;
                    return Filter::lowshelf;
            }
        };

        bands[band]->setType (map (t));
        bands[band]->setParams (freq, q, g);
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        for (auto& b : bands)
            b->process (buffer);
    }

private:
    juce::OwnedArray<Filter> bands;
};

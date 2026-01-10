/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Multi-channel biquad filter with multiple types and slopes.

    Filter provides a flexible multi-channel filtering solution supporting
    common filter types (lowpass, highpass, bandpass, etc.) with selectable
    12dB or 24dB/octave slopes. It uses high-quality biquad filters with
    matched z-transform (MZTi) coefficient calculation.

    Key Features:
    - Multiple filter types (8 types available)
    - 12dB or 24dB/octave slopes
    - Multi-channel processing
    - Frequency response calculation
    - Low/high shelving and parametric peak
    - Real-time parameter updates

    Filter Types:
    - lowpass: Remove frequencies above cutoff
    - highpass: Remove frequencies below cutoff
    - bandpass: Keep only frequencies near center
    - notch: Remove frequencies near center
    - lowshelf: Boost/cut low frequencies
    - highshelf: Boost/cut high frequencies
    - peak: Boost/cut at specific frequency
    - allpass: Phase shift without amplitude change

    Slopes:
    - db12: 12dB/octave (one biquad stage)
    - db24: 24dB/octave (two biquad stages)

    Usage:
    @code
    Filter filter;
    filter.setSampleRate(44100.0);
    filter.setNumChannels(2);
    filter.setType(Filter::lowpass);
    filter.setSlope(Filter::db24);
    filter.setParams(1000.0f, 0.707f); // 1kHz, Q=0.707

    filter.process(audioBuffer);

    // Get frequency response at 500Hz
    float response = filter.getResponseMagnitude(500.0f);
    @endcode

    @see EQ
*/
class Filter
{
public:
    enum Type
    {
        none = 0,
        lowpass,
        highpass,
        bandpass,
        notch,
        lowshelf,
        highshelf,
        peak,
        allpass,
    };

    enum Slope
    {
        db12 = 0,
        db24,
    };

    Filter (int maxOrder = 2)
        : biquad1 (2 * ((maxOrder + 1) / 2)), biquad2 (2 * ((maxOrder + 1) / 2))
    {
        biquad1.resize (1);
        biquad2.resize (1);
    }

    void setSampleRate (double sr)  { sampleRate = sr;  }

    void setNumChannels (int ch)
    {
        if (channels != ch)
        {
            channels = ch;
            filters.clear();

            for (int i = 0; i < 2; i++)
                filters.push_back (std::make_unique<AudioFilter::FilterInstance<float>> (ch));
        }
    }

    void setType (Type t)           { type = t;         }
    void setSlope (Slope s)         { slope = s;        }

    void reset()
    {
        for (auto& f : filters)
            f->clear();
    }

    float getFrequency()            { return freq;      }

    void setParams (float freq_, float q_, float g_ = 0.0f)
    {
        freq = freq_;
        q = q_;
        g = g_;

        if (type == none) return;

        AudioFilter::ParametricCreator::createMZTiStage (biquad1[0], freq, juce::Decibels::gainToDecibels (g), q, conv (type), sampleRate);
        filters[0]->setParams (biquad1[0]);

        AudioFilter::ParametricCreator::createMZTiStage (biquad2[0], freq, juce::Decibels::gainToDecibels (g), Q, conv (type), sampleRate);
        filters[1]->setParams (biquad2[0]);
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        if (type == none) return;

        const int numSamples = buffer.getNumSamples();

        auto o = (float**)buffer.getArrayOfWritePointers();
        auto i = (const float**)buffer.getArrayOfReadPointers();

        switch (slope)
        {
            case db12:
                filters[0]->processBlock (o, i, numSamples);
                break;
            case db24:
                filters[0]->processBlock (o, i, numSamples);
                filters[1]->processBlock (o, i, numSamples);
                break;
        }
    }

    float getResponseMagnitude (float atFreq)
    {
        auto res = 1.0f;

        if (slope >= db12)
        {
            const int maxOrder = 2;
            AudioFilter::BiquadParamCascade biquadRes { 2 * ((maxOrder + 1) / 2) };
            biquadRes.resize (1);
            AudioFilter::ParametricCreator::createMZTiStage (biquadRes[0], freq, juce::Decibels::gainToDecibels (g), q, conv (type), sampleRate);

            res *= float (AudioFilter::Response::getResponsePoint (biquadRes, atFreq, sampleRate));
        }

        if (slope >= db24)
        {
            const int maxOrder = 2;
            AudioFilter::BiquadParamCascade biquadRes { 2 * ((maxOrder + 1) / 2) };
            biquadRes.resize (1);
            AudioFilter::ParametricCreator::createMZTiStage (biquadRes[0], freq, juce::Decibels::gainToDecibels (g), Q, conv (type), sampleRate);

            res *= float (AudioFilter::Response::getResponsePoint (biquadRes, atFreq, sampleRate));
        }

        return res;
    }

private:
    AudioFilter::FilterType conv (Type t)
    {
        switch (t)
        {
            case lowpass:       return AudioFilter::afLoPass;
            case highpass:      return AudioFilter::afHiPass;
            case bandpass:      return AudioFilter::afBandPass;
            case notch:         return AudioFilter::afNotch;
            case lowshelf:      return AudioFilter::afLoShelf;
            case highshelf:     return AudioFilter::afHiShelf;
            case peak:          return AudioFilter::afPeak;
            case allpass:       return AudioFilter::afAllPass;
            case none:
            default:
                jassertfalse;
                return AudioFilter::afAllPass;
        }
    }

    Type type = none;
    Slope slope = db12;

    int channels = 0;
    double sampleRate = 44100;

    float freq = 2000.0f;
    float q = 0.70710678118655f;
    float g = 0.0f;

    AudioFilter::BiquadParamCascade biquad1, biquad2;
    std::vector<std::unique_ptr<AudioFilter::FilterInstance<float>>> filters;
};

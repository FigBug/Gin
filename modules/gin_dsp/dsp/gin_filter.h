/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Basic multi channel filter
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
        channels = ch;
        filters.clear();

        for (int i = 0; i < 2; i++)
            filters.push_back (std::make_unique<AudioFilter::FilterInstance<float>> (ch));
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

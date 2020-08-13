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
    };

    enum Slope
    {
        db12 = 0,
        db24,
    };

    Filter ()
    {
    }

    void setSampleRate (double sr)  { sampleRate = sr;  }
    void setNumChannels (int ch)    { channels = ch; filters.resize (size_t (channels * 2));    }

    void setType (Type t)           { type = t;         }
    void setSlope (Slope s)         { slope = s;        }

    void reset()
    {
        for (auto& f : filters)
            f.reset();
    }

    float getFrequency()            { return freq;      }

    void setParams (float freq_, float q_, float g_ = 0.0f)
    {
        freq = freq_;
        q = q_;
        g = g_;

        if (type == none) return;

        juce::IIRCoefficients coeffs1, coeffs2;

        switch (type)
        {
            case lowpass:
                coeffs1 = juce::IIRCoefficients::makeLowPass (sampleRate, freq, q);
                coeffs2 = juce::IIRCoefficients::makeLowPass (sampleRate, freq, Q);
                break;
            case highpass:
                coeffs1 = juce::IIRCoefficients::makeHighPass (sampleRate, freq, q);
                coeffs2 = juce::IIRCoefficients::makeHighPass (sampleRate, freq, Q);
                break;
            case bandpass:
                coeffs1 = juce::IIRCoefficients::makeBandPass (sampleRate, freq, q);
                coeffs2 = juce::IIRCoefficients::makeBandPass (sampleRate, freq, Q);
                break;
            case notch:
                coeffs1 = juce::IIRCoefficients::makeNotchFilter (sampleRate, freq, q);
                coeffs2 = juce::IIRCoefficients::makeNotchFilter (sampleRate, freq, Q);
                break;
            case lowshelf:
                coeffs1 = juce::IIRCoefficients::makeLowShelf (sampleRate, freq, q, g);
                coeffs2 = juce::IIRCoefficients::makeLowShelf (sampleRate, freq, Q, g);
                break;
            case highshelf:
                coeffs1 = juce::IIRCoefficients::makeHighShelf (sampleRate, freq, q, g);
                coeffs2 = juce::IIRCoefficients::makeHighShelf (sampleRate, freq, Q, g);
                break;
            case peak:
                coeffs1 = juce::IIRCoefficients::makePeakFilter (sampleRate, freq, q, g);
                coeffs2 = juce::IIRCoefficients::makePeakFilter (sampleRate, freq, Q, g);
                break;
            case none:
            default:
                jassertfalse;
                break;
        }

        for (int ch = 0; ch < channels; ch++)
        {
            filters[size_t (ch * 2 + 0)].setCoefficients (coeffs1);
            filters[size_t (ch * 2 + 1)].setCoefficients (coeffs2);
        }
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        if (type == none) return;

        const int numSamples = buffer.getNumSamples();

        for (int ch = 0; ch < channels; ch++)
        {
            switch (slope)
            {
                case db12:
                    filters[size_t (ch * 2)].processSamples (buffer.getWritePointer (ch), numSamples);
                    break;
                case db24:
                    filters[size_t (ch * 2 + 0)].processSamples (buffer.getWritePointer (ch), numSamples);
                    filters[size_t (ch * 2 + 1)].processSamples (buffer.getWritePointer (ch), numSamples);
                    break;
            }
        }
    }

private:
    Type type = none;
    Slope slope = db12;

    int channels = 0;
    double sampleRate = 44100;

    float freq = 2000.0f;
    float q = 0.70710678118655f;
    float g = 0.0f;

    std::vector<juce::IIRFilter> filters;
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
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

    void setParams (float freq_, float q_)
    {
        freq = freq_;
        q = q_;

        if (type == none) return;

        IIRCoefficients coeffs1, coeffs2;

        switch (type)
        {
            case lowpass:
                coeffs1 = IIRCoefficients::makeLowPass (sampleRate, freq, q);
                coeffs2 = IIRCoefficients::makeLowPass (sampleRate, freq, Q);
                break;
            case highpass:
                coeffs1 = IIRCoefficients::makeHighPass (sampleRate, freq, q);
                coeffs2 = IIRCoefficients::makeHighPass (sampleRate, freq, Q);
                break;
            case bandpass:
                coeffs1 = IIRCoefficients::makeBandPass (sampleRate, freq, q);
                coeffs2 = IIRCoefficients::makeBandPass (sampleRate, freq, Q);
                break;
            case notch:
                coeffs1 = IIRCoefficients::makeNotchFilter (sampleRate, freq, q);
                coeffs2 = IIRCoefficients::makeNotchFilter (sampleRate, freq, Q);
                break;
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

    void process (AudioSampleBuffer& buffer)
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

    std::vector<juce::IIRFilter> filters;
};

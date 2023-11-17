/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Basic multi channel / multi band EQ
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

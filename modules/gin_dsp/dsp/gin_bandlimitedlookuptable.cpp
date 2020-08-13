/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#include <cmath>

//==============================================================================
inline int oddEven (int x)
{
    return (x % 2 == 0) ? 1 : -1;
}

//==============================================================================
double sine (double phase, double, double)
{
    return std::sin (phase * 2 * juce::MathConstants<double>::pi);
}

double triangle (double phase, double freq, double sampleRate)
{
    double sum = 0;
    int k = 1;
    while (freq * k < sampleRate / 2)
    {
        sum += std::pow (-1, (k - 1) / 2.0f) / (k * k) * std::sin (k * (phase * 2 * juce::MathConstants<double>::pi));
        k += 2;
    }
    return float (8.0f / (juce::float_Pi * juce::float_Pi) * sum);
}

double sawUp (double phase, double freq, double sampleRate)
{
    double sum = 0;
    int k = 1;
    while (freq * k < sampleRate / 2)
    {
        sum += oddEven (k) * std::sin (k * (phase * 2 * juce::MathConstants<double>::pi)) / k;
        k++;
    }
    return float (-2.0f / juce::float_Pi * sum);
}

double sawDown (double phase, double freq, double sampleRate)
{
    double sum = 0;
    int k = 1;
    while (freq * k < sampleRate / 2)
    {
        sum += oddEven (k) * std::sin (k * (phase * 2 * juce::MathConstants<double>::pi)) / k;
        k++;
    }
    return float (2.0f / juce::float_Pi * sum);
}

double pulse (double phase, double pw, double freq, double sampleRate)
{
    if (pw == 0.5)
    {
        double sum = 0;
        int i = 1;
        while (freq * (2 * i - 1) < sampleRate / 2)
        {
            sum += std::sin ((2 * i - 1) * (phase * 2 * juce::MathConstants<double>::pi)) / ((2 * i - 1));
            i++;
        }

        return float (4.0f / juce::float_Pi * sum);
    }
    else
    {
        pw = juce::jlimit (0.05, 0.95, pw);
        return sawUp (phase + 0.5 * pw, freq, sampleRate) - sawUp (phase - 0.5 * pw, freq, sampleRate);
    }
}

double squareWave (double phase, double freq, double sampleRate)
{
    double sum = 0;
    int i = 1;
    while (freq * (2 * i - 1) < sampleRate / 2)
    {
        sum += std::sin ((2 * i - 1) * (phase * 2 * juce::MathConstants<double>::pi)) / ((2 * i - 1));
        i++;
    }

    return float (4.0f / juce::float_Pi * sum);
}

double noise()
{
    const float mean = 0.0f;
    const float stddev = 0.1f;

    static std::default_random_engine generator;
    static std::normal_distribution<float> dist (mean, stddev);

    return dist (generator);
}

//==============================================================================
void BandLimitedLookupTable::loadFromBuffer (juce::AudioSampleBuffer& buffer, double sampleRate, int notesPerTable_)
{
    tables.clear();

    double duration = buffer.getNumSamples() / sampleRate;
    double baseFreq = 1.0 / duration;
    int sz = buffer.getNumSamples();

    notesPerTable = notesPerTable_;

    for (double note = notesPerTable + 0.5; note < 127.0; note += notesPerTable)
    {
        auto noteFreq = getMidiNoteInHertz (note);
        if (noteFreq < baseFreq)
        {
            auto func = [&] (float phase)
            {
                auto data = buffer.getReadPointer (0);
                return data[int (phase * sz) % sz];
            };

            tables.add (new juce::dsp::LookupTableTransform<float> (func, 0.0f, 1.0f, (size_t) sz + 1));
        }
        else
        {
            auto func = [&] (float phase)
            {
                auto data = buffer.getReadPointer (0);
                return data[int (phase * sz) % sz];
            };

            tables.add (new juce::dsp::LookupTableTransform<float> (func, 0.0f, 1.0f, (size_t) sz + 1));
        }
     }
}

//==============================================================================
BandLimitedLookupTables::BandLimitedLookupTables (double sampleRate_, int notesPerTable_, int tableSize_)
  : sampleRate (sampleRate_),
    notesPerTable (notesPerTable_),
    tableSize (tableSize_),
    sineTable (sine, sampleRate, 64, tableSize),
    sawUpTable (sawUp, sampleRate, notesPerTable, tableSize),
    sawDownTable (sawDown, sampleRate, notesPerTable, tableSize),
    triangleTable (triangle, sampleRate, notesPerTable, tableSize)
{

}

void BandLimitedLookupTables::setSampleRate (double sampleRate_)
{
    if (sampleRate != sampleRate_)
    {
        sampleRate = sampleRate_;
        sineTable.reset (sine, sampleRate, 64, tableSize);
        sawUpTable.reset (sawUp, sampleRate, notesPerTable, tableSize);
        sawDownTable.reset (sawDown, sampleRate, notesPerTable, tableSize);
        triangleTable.reset (triangle, sampleRate, notesPerTable, tableSize);
    }
}

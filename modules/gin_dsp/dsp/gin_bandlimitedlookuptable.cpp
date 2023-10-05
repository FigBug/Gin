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
float sine (float phase, float, float)
{
    return std::sin (phase * 2 * juce::MathConstants<float>::pi);
}

float triangle (float phase, float freq, float sampleRate)
{
    float sum = 0.0f;
    float k = 1.0f;
    while (freq * k < sampleRate / 2)
    {
        sum += std::pow (-1.0f, (k - 1) / 2.0f) / (k * k) * std::sin (k * (phase * 2 * juce::MathConstants<float>::pi));
        k += 2;
    }
    return float (8.0f / (juce::MathConstants<float>::pi * juce::MathConstants<float>::pi) * sum);
}

float sawUp (float phase, float freq, float sampleRate)
{
    float sum = 0;
    int k = 1;
    float fk = 1.0f;
    while (freq * k < sampleRate / 2)
    {
        sum += oddEven (k) * std::sin (fk * (phase * 2 * juce::MathConstants<float>::pi)) / fk;
        k++;
        fk++;
    }
    return float (-2.0f / juce::MathConstants<float>::pi * sum);
}

float sawDown (float phase, float freq, float sampleRate)
{
    float sum = 0;
    int k = 1;
    float fk = 1.0f;
    while (freq * k < sampleRate / 2)
    {
        sum += oddEven (k) * std::sin (fk * (phase * 2 * juce::MathConstants<float>::pi)) / fk;
        k++;
        fk++;
    }
    return float (2.0f / juce::MathConstants<float>::pi * sum);
}

float pulse (float phase, float pw, float freq, float sampleRate)
{
    if (pw == 0.5)
    {
        float sum = 0;
        float i = 1;
        while (freq * (2 * i - 1) < sampleRate / 2)
        {
            sum += std::sin ((2 * i - 1) * (phase * 2 * juce::MathConstants<float>::pi)) / ((2 * i - 1));
            i++;
        }

        return float (4.0f / juce::MathConstants<float>::pi * sum);
    }
    else
    {
        pw = juce::jlimit (0.05f, 0.95f, pw);
        return sawUp (phase + 0.5f * pw, freq, sampleRate) - sawUp (phase - 0.5f * pw, freq, sampleRate);
    }
}

float squareWave (float phase, float freq, float sampleRate)
{
    float sum = 0;
    float i = 1;
    while (freq * (2 * i - 1) < sampleRate / 2)
    {
        sum += std::sin ((2 * i - 1) * (phase * 2 * juce::MathConstants<float>::pi)) / ((2 * i - 1));
        i++;
    }

    return float (4.0f / juce::MathConstants<float>::pi * sum);
}

//==============================================================================
void BandLimitedLookupTable::loadFromBuffer (std::unique_ptr<juce::dsp::FFT>& fft, float playbackSampleRate, juce::AudioSampleBuffer& buffer, float fileSampleRate, int notesPerTable_)
{
    tables.clear();

    float duration = buffer.getNumSamples() / fileSampleRate;
    float baseFreq = 1.0f / duration;
    int sz = buffer.getNumSamples();

    tableSize = sz;
    notesPerTable = notesPerTable_;

    if (fft == nullptr || fft->getSize() != sz)
         fft = std::make_unique<juce::dsp::FFT> (juce::roundToInt (std::log2 (sz)));
    jassert (fft->getSize() == sz);

    std::vector<juce::dsp::Complex<float>> time;
    std::vector<juce::dsp::Complex<float>> freq;

    time.resize (size_t (sz));
    freq.resize (size_t (sz));

    auto d = buffer.getReadPointer (0);
    for (auto i = 0; i < sz; i++)
        time[size_t (i)] = { d[i], 0.0f };

    fft->perform (time.data(), freq.data(), false);

    for (float note = notesPerTable + 0.5f; note < 127.0f; note += notesPerTable)
    {
        auto noteFreq = getMidiNoteInHertz (note);
        if (noteFreq < baseFreq)
        {
            auto& t = tables.emplace_back (std::vector<float>());
            t.resize (size_t (sz));
            std::memcpy (t.data(), buffer.getReadPointer (0), size_t (sz) * sizeof (float));
        }
        else
        {
            auto index2Freq = [&] (int i)
            {
                return float (i * (fileSampleRate / sz));
            };

            auto ratio = noteFreq / baseFreq;

            for (auto i = 0; i < sz; i++)
                if (index2Freq (i) * ratio > playbackSampleRate / 2)
                    freq[size_t (i)] = {0.0f, 0.0f};

            fft->perform (freq.data(), time.data(), true);

            auto& t = tables.emplace_back (std::vector<float>());
            t.resize (size_t (sz));

            for (auto i = 0; i < sz; i++)
                t[size_t (i)] = time[size_t(i)].real();
        }
     }
}

//==============================================================================
BandLimitedLookupTables::BandLimitedLookupTables (double sampleRate_, int notesPerTable_, int tableSize_)
  : sampleRate (sampleRate_),
    notesPerTable (notesPerTable_),
    tableSize (tableSize_),
    sineTable (sine, float (sampleRate), 64, tableSize),
    sawUpTable (sawUp, float (sampleRate), notesPerTable, tableSize),
    sawDownTable (sawDown, float (sampleRate), notesPerTable, tableSize),
    triangleTable (triangle, float (sampleRate), notesPerTable, tableSize)
{

}

void BandLimitedLookupTables::setSampleRate (double sampleRate_)
{
    if (! juce::approximatelyEqual (sampleRate, sampleRate_))
    {
        sampleRate = sampleRate_;
        sineTable.reset (sine, float (sampleRate), 64, tableSize);
        sawUpTable.reset (sawUp, float (sampleRate), notesPerTable, tableSize);
        sawDownTable.reset (sawDown, float (sampleRate), notesPerTable, tableSize);
        triangleTable.reset (triangle, float (sampleRate), notesPerTable, tableSize);
    }
}

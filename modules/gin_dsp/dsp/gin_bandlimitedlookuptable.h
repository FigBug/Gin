/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
double sine (double phase, double unused1 = 0, double unused2 = 0);
double triangle (double phase, double freq, double sampleRate);
double sawUp (double phase, double freq, double sampleRate);
double sawDown (double phase, double freq, double sampleRate);
double pulse (double phase, double pw, double freq, double sampleRate);
double squareWave (double phase, double freq, double sampleRate);
double noise();

//==============================================================================
/** Lookup tables for holding bandlimited waveforms. Holds one waveform for every N number notes
*/
class BandLimitedLookupTable
{
public:
    BandLimitedLookupTable() = default;

    BandLimitedLookupTable (std::function<double (double, double, double)> function, double sampleRate,
                            int notesPerTable_ = 6, int tableSize_ = 2048)
    {
        reset (function, sampleRate, notesPerTable_, tableSize_);
    }

    void reset (std::function<double (double, double, double)> function, double sampleRate,
                int notesPerTable_ = 6, int tableSize = 2048)
    {
        tables.clear();

        notesPerTable = notesPerTable_;

        for (double note = notesPerTable + 0.5; note < 127.0; note += notesPerTable)
        {
            auto freq = getMidiNoteInHertz (note);

            auto func = [function, freq, sampleRate] (float phase) -> float
            {
                return float (function (phase, freq, sampleRate));
            };

            tables.add (new juce::dsp::LookupTableTransform<float> (func, 0.0f, 1.0f, (size_t) tableSize));
        }
    }

    inline float process (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, tables.size() - 1, int ((note - 0.5) / notesPerTable));
        return tables[tableIndex]->processSampleUnchecked (phase);
    }

    void loadFromBuffer (juce::AudioSampleBuffer& buffer, double sampleRate, int notesPerTable);

    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> tables;

    int notesPerTable = 0;
};

//==============================================================================
enum class Wave
{
    silence     = 0,
    sine        = 1,
    triangle    = 2,
    sawUp       = 3,
    sawDown     = 4,
    pulse       = 5,
    square      = 6,
    noise       = 7,
    wavetable   = 8,
};

//==============================================================================
/** Generate and hold bandlimited lookup tabkes for all the common waveforms
*/
class BandLimitedLookupTables
{
public:
    BandLimitedLookupTables (double sampleRate = 44100, int notesPerTable = 3, int tableSize = 2048);

    void setSampleRate (double sampleRate);

    inline float processSine (float phase)
    {
        return sineTable.tables[0]->processSampleUnchecked (phase);
    }

    inline float processTriangle (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, triangleTable.tables.size() - 1, int ((note - 0.5) / triangleTable.notesPerTable));
        return triangleTable.tables[tableIndex]->processSampleUnchecked (phase);
    }

    inline float processSawUp (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, sawUpTable.tables.size() - 1, int ((note - 0.5) / sawUpTable.notesPerTable));
        return sawUpTable.tables[tableIndex]->processSampleUnchecked (phase);
    }

    inline float processSawDown (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, sawDownTable.tables.size() - 1, int ((note - 0.5) / sawDownTable.notesPerTable));
        return sawDownTable.tables[tableIndex]->processSampleUnchecked (phase);
    }

    inline float processSquare (float note, float phase)
    {
        float phaseUp   = phase + 0.25f;
        float phaseDown = phase - 0.25f;

        if (phaseUp   > 1.0f) phaseUp   -= 1.0f;
        if (phaseDown < 0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, count - 1, int ((note - 0.5) / count));

        auto s1 = sawDownTable.tables[tableIndex]->processSampleUnchecked (phaseDown);
        auto s2 = sawUpTable.tables[tableIndex]->processSampleUnchecked (phaseUp);

        return s1 + s2;
    }

    inline float processPulse (float note, float phase, float pw)
    {
        float phaseUp   = phase + 0.5f * pw;
        float phaseDown = phase - 0.5f * pw;

        if (phaseUp   > 1.0f) phaseUp   -= 1.0f;
        if (phaseDown < 0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, count - 1, int ((note - 0.5) / count));

        auto s1 = sawDownTable.tables[tableIndex]->processSampleUnchecked (phaseDown);
        auto s2 = sawUpTable.tables[tableIndex]->processSampleUnchecked (phaseUp);

        return s1 + s2;
    }

    inline float process (Wave wave, float note, float phase, float pw)
    {
        switch (wave)
        {
            case Wave::silence:     return 0;
            case Wave::sine:        return processSine (phase);
            case Wave::triangle:    return processTriangle (note, phase);
            case Wave::sawUp:       return processSawUp (note, phase);
            case Wave::sawDown:     return processSawDown (note, phase);
            case Wave::pulse:       return processPulse (note, phase, pw);
            case Wave::square:      return processSquare (note, phase);
            case Wave::noise:       return (float) noise();
            case Wave::wavetable:
            default:
                jassertfalse;
                return 0.0f;
        }
    }

    double getSampleRate()
    {
        return sampleRate;
    }

private:
    double sampleRate = 0;
    int notesPerTable = 3, tableSize = 2048;
    BandLimitedLookupTable sineTable, sawUpTable, sawDownTable, triangleTable;
};

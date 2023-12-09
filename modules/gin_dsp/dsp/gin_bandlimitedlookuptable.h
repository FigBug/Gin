/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "gin_noise.h"

//==============================================================================
float sine (float phase, float unused1 = 0, float unused2 = 0);
float triangle (float phase, float freq, float sampleRate);
float sawUp (float phase, float freq, float sampleRate);
float sawDown (float phase, float freq, float sampleRate);
float pulse (float phase, float pw, float freq, float sampleRate);
float squareWave (float phase, float freq, float sampleRate);

//==============================================================================
/** Lookup tables for holding bandlimited waveforms. Holds one waveform for every N number notes
*/
class BandLimitedLookupTable
{
public:
    BandLimitedLookupTable() = default;

    BandLimitedLookupTable (std::function<float (float, float, float)> function, float sampleRate,
                            int notesPerTable_ = 6, int tableSize_ = 2048)
    {
        reset (function, sampleRate, notesPerTable_, tableSize_);
    }

    void reset (std::function<float (float, float, float)> function, float sampleRate,
                int notesPerTable_ = 6, int tableSize_ = 2048)
    {
        tables.clear();

        tableSize = tableSize_;
        notesPerTable = notesPerTable_;

        for (float note = notesPerTable + 0.5f; note < 127.0f; note += notesPerTable)
        {
            auto freq = getMidiNoteInHertz (note);

            auto& t = tables.emplace_back (std::vector<float>());
            t.resize (size_t (tableSize));

            for (auto i = 0; i < tableSize; i++)
            {
                auto v = juce::jmap (float (i), 0.0f, tableSize - 1.0f, 0.0f, 1.0f);
                t[size_t (i)] = function (v, freq, sampleRate);
            }
        }
    }
    
    inline int tableIndexForNote (float note)
    {
        return juce::jlimit (0, int (tables.size() - 1), int ((note - 0.5) / notesPerTable));
    }
    
    inline std::vector<float>* tableForNote (float note)
    {
        return &tables[size_t (tableIndexForNote (note))];
    }

    inline float process (float note, float phase)
    {
        auto tableIndex = juce::jlimit (0, int (tables.size() - 1), int ((note - 0.5) / notesPerTable));
        auto pos = int (phase * tableSize);

        jassert (pos >= 0 && pos < tableSize);

        return tables[size_t (tableIndex)][size_t (pos)];
    }
    
   #if GIN_HAS_SIMD
    inline mipp::Reg<float> process (float note, mipp::Reg<float> phase)
    {
        static_assert (mipp::N<float>() == 4);

        auto tableIndex = juce::jlimit (0, int (tables.size() - 1), int ((note - 0.5) / notesPerTable));
        phase *= float (tableSize);
        
        float pos[4];
        phase.store (pos);

        mipp::Reg<float> res =
        {
            tables[size_t (tableIndex)][size_t (pos[0])],
            tables[size_t (tableIndex)][size_t (pos[1])],
            tables[size_t (tableIndex)][size_t (pos[2])],
            tables[size_t (tableIndex)][size_t (pos[3])],
        };
        return res;
    }
   #endif

    inline float get (int tableIndex, float phase)
    {
        auto pos = int (phase * tableSize);

        jassert (pos >= 0 && pos < tableSize);

        return tables[size_t (tableIndex)][size_t (pos)];
    }

    void loadFromBuffer (std::unique_ptr<juce::dsp::FFT>& fft, float playbackSampleRate, juce::AudioSampleBuffer& buffer, float fileSampleRate, int notesPerTable);

    std::vector<std::vector<float>> tables;

    int tableSize = 0;
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
    whiteNoise  = 7,
    pinkNoise   = 8,
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
        return sineTable.get (0, phase);
    }

    inline float processTriangle (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (triangleTable.tables.size() - 1), int ((note - 0.5) / triangleTable.notesPerTable));
        return triangleTable.get (tableIndex, phase);
    }

    inline float processSawUp (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (sawUpTable.tables.size() - 1), int ((note - 0.5) / sawUpTable.notesPerTable));
        return sawUpTable.get (tableIndex, phase);
    }

    inline float processSawDown (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (sawDownTable.tables.size() - 1), int ((note - 0.5) / sawDownTable.notesPerTable));
        return sawDownTable.get (tableIndex, phase);
    }

    inline float processSquare (float note, float phase)
    {
        float phaseUp   = phase + 0.25f;
        float phaseDown = phase - 0.25f;

        if (phaseUp   >= 1.0f) phaseUp   -= 1.0f;
        if (phaseDown <  0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, int (count - 1), int ((note - 0.5) / count));

        auto s1 = sawDownTable.get (tableIndex, phaseDown);
        auto s2 = sawUpTable.get (tableIndex, phaseUp);

        return s1 + s2;
    }

    inline float processPulse (float note, float phase, float pw)
    {
        float phaseUp   = phase + 0.5f * pw;
        float phaseDown = phase - 0.5f * pw;

        if (phaseUp   >= 1.0f) phaseUp   -= 1.0f;
        if (phaseDown <  0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, int (count - 1), int ((note - 0.5) / count));

        auto s1 = sawDownTable.get (tableIndex, phaseDown);
        auto s2 = sawUpTable.get (tableIndex, phaseUp);

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
            case Wave::whiteNoise:  return (float) whiteNoise.nextSample();
            case Wave::pinkNoise:   return (float) pinkNoise.nextSample();
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
    WhiteNoise whiteNoise;
    PinkNoise pinkNoise;
};

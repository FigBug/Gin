/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "gin_noise.h"

//==============================================================================
/**
    Band-limited sine wave generator.

    Generates a pure sine wave at the given phase. No band-limiting is needed
    for sine waves as they contain only the fundamental frequency.

    @param phase Phase position (0.0 to 1.0)
    @param unused1 Unused parameter for API consistency
    @param unused2 Unused parameter for API consistency
    @return Sample value (-1.0 to 1.0)
*/
float sine (float phase, float unused1 = 0, float unused2 = 0);

/**
    Band-limited triangle wave generator.

    Generates an anti-aliased triangle wave using pre-computed band-limited
    wavetables based on the fundamental frequency.

    @param phase Phase position (0.0 to 1.0)
    @param freq Fundamental frequency in Hz
    @param sampleRate Current sample rate
    @return Sample value (-1.0 to 1.0)
*/
float triangle (float phase, float freq, float sampleRate);

/**
    Band-limited upward sawtooth wave generator.

    Generates an anti-aliased sawtooth wave that rises from -1 to 1 using
    pre-computed band-limited wavetables.

    @param phase Phase position (0.0 to 1.0)
    @param freq Fundamental frequency in Hz
    @param sampleRate Current sample rate
    @return Sample value (-1.0 to 1.0)
*/
float sawUp (float phase, float freq, float sampleRate);

/**
    Band-limited downward sawtooth wave generator.

    Generates an anti-aliased sawtooth wave that falls from 1 to -1 using
    pre-computed band-limited wavetables.

    @param phase Phase position (0.0 to 1.0)
    @param freq Fundamental frequency in Hz
    @param sampleRate Current sample rate
    @return Sample value (-1.0 to 1.0)
*/
float sawDown (float phase, float freq, float sampleRate);

/**
    Band-limited pulse wave generator with variable pulse width.

    Generates an anti-aliased pulse wave with adjustable pulse width (duty cycle).
    Uses pre-computed band-limited wavetables for alias-free waveforms.

    @param phase Phase position (0.0 to 1.0)
    @param pw Pulse width / duty cycle (0.0 to 1.0, 0.5 = square wave)
    @param freq Fundamental frequency in Hz
    @param sampleRate Current sample rate
    @return Sample value (-1.0 to 1.0)
*/
float pulse (float phase, float pw, float freq, float sampleRate);

/**
    Band-limited square wave generator (50% duty cycle pulse).

    Generates an anti-aliased square wave using pre-computed band-limited
    wavetables. Equivalent to pulse() with pw = 0.5.

    @param phase Phase position (0.0 to 1.0)
    @param freq Fundamental frequency in Hz
    @param sampleRate Current sample rate
    @return Sample value (-1.0 to 1.0)
*/
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
            t.push_back (t[0]); // let the table wrap so we can interpolate without doing a mod
        }
    }
    
    inline int tableIndexForNote (float note)
    {
        return juce::jlimit (0, int (tables.size() - 1), int ((note - 0.5) / notesPerTable));
    }
    
    inline std::vector<float>& tableForNote (float note)
    {
        return tables[size_t (tableIndexForNote (note))];
    }

    inline float process (float note, float phase)
    {
        auto& table = tableForNote (note);
        auto pos = int (phase * tableSize);

        jassert (pos >= 0 && pos < tableSize);

        return table[size_t (pos)];
    }
    
    inline float processLinear (float note, float phase)
    {
        auto& table = tableForNote (note);
        auto pos = int (phase * tableSize);
        auto frac = (phase * tableSize) - pos;
        
        jassert (pos >= 0 && pos + 1 < int (table.size()));
        return (table[size_t (pos)] * (1.0f - frac)) + (table[size_t (pos + 1)] * (frac));
    }
    
   #if GIN_HAS_SIMD
    inline mipp::Reg<float> process (float note, mipp::Reg<float> phase)
    {
        static_assert (mipp::N<float>() == 4);

        auto& table = tableForNote (note);
        phase *= float (tableSize);
        
        float pos[4];
        phase.store (pos);

        jassert (pos[0] >= 0 && pos[0] < table.size());
        jassert (pos[1] >= 0 && pos[1] < table.size());
        jassert (pos[2] >= 0 && pos[2] < table.size());
        jassert (pos[3] >= 0 && pos[3] < table.size());

        mipp::Reg<float> res =
        {
            table[size_t (pos[0])],
            table[size_t (pos[1])],
            table[size_t (pos[2])],
            table[size_t (pos[3])],
        };
        return res;
    }
    
    inline mipp::Reg<float> processLinear (float note, mipp::Reg<float> phase)
    {
        static_assert (mipp::N<float>() == 4);

        auto& table = tableForNote (note);
        auto pos = mipp::trunc (phase * float (tableSize));
        auto frac = (phase * float (tableSize)) - pos;
        
        float p[4];
        pos.store (p);

        float f[4];
        frac.store (f);

        jassert (pos[0] >= 0 && pos[0] + 1 < table.size());
        jassert (pos[1] >= 0 && pos[1] + 1 < table.size());
        jassert (pos[2] >= 0 && pos[2] + 1 < table.size());
        jassert (pos[3] >= 0 && pos[3] + 1 < table.size());

        mipp::Reg<float> res =
        {
            (table[size_t (p[0])] * (1.0f - f[0])) + (table[size_t (p[0] + 1)] * (f[0])),
            (table[size_t (p[1])] * (1.0f - f[1])) + (table[size_t (p[1] + 1)] * (f[1])),
            (table[size_t (p[2])] * (1.0f - f[2])) + (table[size_t (p[2] + 1)] * (f[2])),
            (table[size_t (p[3])] * (1.0f - f[3])) + (table[size_t (p[3] + 1)] * (f[3])),
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
    
    inline float getLinear (int tableIndex, float phase)
    {
        auto pos = int (phase * tableSize);
        auto frac = (phase * tableSize) - pos;
        
        return (tables[size_t (tableIndex)][size_t (pos)] * (1.0f - frac)) +
               (tables[size_t (tableIndex)][size_t (pos + 1)] * (frac));
    }

    /** Load from an audio sample buffer, using an FFT to rerove high frequencies. Usefull for wavetables.
     */
    void loadFromBuffer (std::unique_ptr<juce::dsp::FFT>& fft, float playbackSampleRate, juce::AudioSampleBuffer& buffer, float fileSampleRate, int notesPerTable);

    /** Load from an audio sample buffer, using low pass filter to rerove high frequencies. Usefull for samples.
     */
    void loadFromBuffer (float playbackSampleRate, juce::AudioSampleBuffer& buffer, float fileSampleRate, float fileFreq, int notesPerTable);

    std::vector<std::vector<float>> tables;

    int tableSize = 0;
    int notesPerTable = 0;
};

//==============================================================================
/**
    Waveform types for band-limited oscillators.

    Wave enum defines the available waveform shapes for oscillators using
    band-limited lookup tables. Each waveform (except noise types) is
    pre-computed with appropriate band-limiting to prevent aliasing.

    Available Waveforms:
    - silence: No output (0.0)
    - sine: Pure sine wave (fundamental only, no harmonics)
    - triangle: Triangle wave with soft harmonics
    - sawUp: Upward sawtooth (ramp up from -1 to 1)
    - sawDown: Downward sawtooth (ramp down from 1 to -1)
    - pulse: Variable pulse width waveform
    - square: Square wave (50% duty cycle pulse)
    - whiteNoise: White noise (equal energy across all frequencies)
    - pinkNoise: Pink noise (equal energy per octave, -3dB/octave)

    @see BandLimitedLookupTables, BandLimitedLookupTable
*/
enum class Wave
{
    silence     = 0,  ///< Silence (outputs 0.0)
    sine        = 1,  ///< Sine wave
    triangle    = 2,  ///< Triangle wave
    sawUp       = 3,  ///< Upward sawtooth
    sawDown     = 4,  ///< Downward sawtooth
    pulse       = 5,  ///< Pulse wave (variable width)
    square      = 6,  ///< Square wave (50% pulse)
    whiteNoise  = 7,  ///< White noise
    pinkNoise   = 8,  ///< Pink noise
};

//==============================================================================
/** Generate and hold bandlimited lookup tables for all the common waveforms
*/
class BandLimitedLookupTables
{
public:
    BandLimitedLookupTables (double sampleRate = 44100, int notesPerTable = 3, int tableSize = 2048);

    void setSampleRate (double sampleRate);

    inline float processSine (float phase)
    {
        return sineTable.getLinear (0, phase);
    }

    inline float processTriangle (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (triangleTable.tables.size() - 1), int ((note - 0.5) / triangleTable.notesPerTable));
        return triangleTable.getLinear (tableIndex, phase);
    }

    inline float processSawUp (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (sawUpTable.tables.size() - 1), int ((note - 0.5) / sawUpTable.notesPerTable));
        return sawUpTable.getLinear (tableIndex, phase);
    }

    inline float processSawDown (float note, float phase)
    {
        int tableIndex = juce::jlimit (0, int (sawDownTable.tables.size() - 1), int ((note - 0.5) / sawDownTable.notesPerTable));
        return sawDownTable.getLinear (tableIndex, phase);
    }

    inline float processSquare (float note, float phase)
    {
        float phaseUp   = phase + 0.25f;
        float phaseDown = phase - 0.25f;

        if (phaseUp   >= 1.0f) phaseUp   -= 1.0f;
        if (phaseDown <  0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, int (count - 1), int ((note - 0.5) / count));

        auto s1 = sawDownTable.getLinear (tableIndex, phaseDown);
        auto s2 = sawUpTable.getLinear (tableIndex, phaseUp);

        return s1 + s2;
    }

    inline float processPulse (float note, float phase, float pw)
    {
        pw = std::clamp (pw, 0.01f, 0.99f);
        
        auto phaseUp   = phase + 0.5f * pw;
        auto phaseDown = phase - 0.5f * pw;

        if (phaseUp   >= 1.0f) phaseUp   -= 1.0f;
        if (phaseDown <  0.0f) phaseDown += 1.0f;

        auto count = std::min (sawDownTable.tables.size(), sawDownTable.tables.size());
        int tableIndex = juce::jlimit (0, int (count - 1), int ((note - 0.5) / count));

        auto dc = 2.0f * pw - 1.0f;
        auto s1 = sawDownTable.getLinear (tableIndex, phaseDown);
        auto s2 = sawUpTable.getLinear (tableIndex, phaseUp);

        return -dc + s1 + s2;
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

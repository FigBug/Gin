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
class BandLimitedLookupTable
{
public:
    BandLimitedLookupTable (std::function<double (double, double, double)> function, double sampleRate, int notesPerTable = 6, int tableSize = 1024);

    juce::OwnedArray<juce::dsp::LookupTableTransform<float>> tables;

    int notesPerTable = 0;
};

//==============================================================================
class BandLimitedLookupTables
{
public:
    BandLimitedLookupTables (double sampleRate);

    float processSine (float phase);
    float processTriangle (float note, float phase);
    float processSawUp (float note, float phase);
    float processSawDown (float note, float phase);
    float processSquare (float note, float phase);
    float processPulse (float note, float phase, float pw);

private:
    BandLimitedLookupTable sineTable, sawUpTable, sawDownTable, triangleTable;
};

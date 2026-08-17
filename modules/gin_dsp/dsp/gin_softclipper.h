/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from MGA Softclamp, which is published under the WTFPL.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Soft clipper with an adjustable knee, worked out in the dB domain.

    Below the knee nothing is touched at all. Through the knee the curve bends
    quadratically in dB, and above it the output is pinned to the ceiling.
    Doing the curve in dB rather than on the sample value is what makes it
    sound gradual: a waveshaper that looks smooth on a linear plot is still
    bending most sharply where the ear is most sensitive to it.

    The knee is expressed as a fraction of the drive, so pushing the drive
    widens the region being softened instead of just hitting the ceiling
    harder and sooner.

    Key Features:
    - Drive, knee width and ceiling
    - Continuous curve - gain and slope both match at the knee edges
    - Per sample, so it can be used anywhere in a chain
    - No allocation, no state, so it can be reset or reconfigured freely

    Usage:
    @code
    SoftClipper clipper;
    clipper.setDrive (6.0f);      // 6 dB in
    clipper.setKnee (0.5f);       // knee spans +/-3 dB around the ceiling
    clipper.setCeiling (-0.1f);

    clipper.process (buffer);
    @endcode

    @see Distortion, Maximizer
*/
class SoftClipper
{
public:
    SoftClipper() = default;
    ~SoftClipper() = default;

    /** Gain applied before the curve, in dB. More drive means more of the
        signal ends up in the knee and above it. */
    void setDrive (float db)
    {
        driveDb = db;
        driveGain = juce::Decibels::decibelsToGain (db);
        recalc();
    }

    /** Knee width as a fraction of the drive, 0 to 1. At 0 this is a hard
        clip at the ceiling; at 1 the knee spans the whole drive range either
        side of it. */
    void setKnee (float k)
    {
        knee = juce::jlimit (0.0f, 1.0f, k);
        recalc();
    }

    /** Output ceiling in dB - the level the curve flattens out at. */
    void setCeiling (float db)
    {
        ceilingGain = juce::Decibels::decibelsToGain (db);
    }

    float processSample (float x) const
    {
        auto driven = x * driveGain;
        auto mag = std::abs (driven);

        if (mag < 1.0e-10f)
            return 0.0f;

        auto db = juce::Decibels::gainToDecibels (mag);

        if (db <= kneeStart)
            return driven * ceilingGain;

        float gainDb;

        if (db < kneeEnd && kneeEnd > kneeStart)
        {
            // Quadratic through the knee. At db == kneeStart this is 0, and at
            // db == kneeEnd it is -db, which is what pins the output to the
            // ceiling from there on up.
            auto t = 1.0f - (db - kneeStart) / (kneeEnd - kneeStart);
            gainDb = kneeStart * t * t - db;
        }
        else
        {
            gainDb = -db;
        }

        return driven * juce::Decibels::decibelsToGain (gainDb) * ceilingGain;
    }

    void process (juce::AudioSampleBuffer& buffer) const
    {
        juce::ScopedNoDenormals noDenormals;

        for (int ch = 0; ch < buffer.getNumChannels(); ch++)
        {
            auto* d = buffer.getWritePointer (ch);

            for (int i = buffer.getNumSamples(); --i >= 0;)
            {
                *d = processSample (*d);
                d++;
            }
        }
    }

private:
    void recalc()
    {
        kneeStart = -knee * driveDb;
        kneeEnd   =  knee * driveDb;
    }

    float driveDb = 0.0f, driveGain = 1.0f;
    float knee = 0.5f;
    float ceilingGain = 1.0f;
    float kneeStart = 0.0f, kneeEnd = 0.0f;
};

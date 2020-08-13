/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
LevelTracker::LevelTracker (float decayPerSecond)
    : decayRate (decayPerSecond)
{
}

void LevelTracker::trackBuffer (juce::AudioSampleBuffer& buffer)
{
    for (int i = 0; i < buffer.getNumChannels(); i++)
        trackBuffer (buffer.getReadPointer (i), buffer.getNumSamples());
}

void LevelTracker::trackBuffer (const float* buffer, int numSamples)
{
    juce::Range<float> range = juce::FloatVectorOperations::findMinAndMax (buffer, numSamples);
    float v1 = std::fabs (range.getStart());
    float v2 = std::fabs (range.getEnd());

    float peakDB = juce::Decibels::gainToDecibels (juce::jmax (v1, v2));

    if (peakDB > 0)
        clip = true;

    if (decayRate < 0)
    {
        if (peakDB < getLevel())
        {
            const float time = float (juce::Time::getMillisecondCounterHiRes() / 1000.0f);

            peakLevel = peakDB;
            peakTime = time;
        }
    }
    else
    {
        if (peakDB > getLevel())
        {
            const float time = float (juce::Time::getMillisecondCounterHiRes() / 1000.0f);

            peakLevel = peakDB;
            peakTime = time;
        }
    }
}

void LevelTracker::trackSample (float f)
{
    float peakDB = juce::Decibels::gainToDecibels (std::abs (f));

    if (peakDB > 0)
        clip = true;

    if (decayRate < 0)
    {
        if (peakDB < getLevel())
        {
            const float time = float (juce::Time::getMillisecondCounterHiRes() / 1000.0f);

            peakLevel = peakDB;
            peakTime = time;
        }
    }
    else
    {
        if (peakDB > getLevel())
        {
            const float time = float (juce::Time::getMillisecondCounterHiRes() / 1000.0f);

            peakLevel = peakDB;
            peakTime = time;
        }
    }
}

float LevelTracker::getLevel() const
{
    const float hold = 50.0f / 1000.0f;

    const float elapsed = float (juce::Time::getMillisecondCounterHiRes() / 1000.0f) - peakTime;

    if (elapsed < hold)
        return peakLevel;

    return peakLevel - (decayRate * (elapsed - hold));
}

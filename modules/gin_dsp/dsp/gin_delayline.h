/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/** Audio Delay with linear interpolation
*/
class DelayLine
{
public:
    DelayLine (int channels, double maximumDelay = 0.001f, double sr = 44100.0f)
    {
        setSize (channels, maximumDelay, sr);
    }

    void setSize (int channels, double maximumDelay, double sr)
    {
        sampleRate = sr;

        buffer.setSize (channels, (int) std::ceil (maximumDelay * sampleRate));
        buffer.clear();

        data = buffer.getArrayOfWritePointers();

        writePos = 0;
    }

    void clear()
    {
        writePos = 0;
        buffer.clear();
    }

    inline float readLagrange (int ch, double t)
    {
        int numSamples = buffer.getNumSamples();

        jassert (t >= 0.0f && t < (numSamples / sampleRate));

        float readPos = std::fmod (float (writePos - 1 + numSamples - (t * sampleRate)), float (numSamples));

        int i1 = int (std::floor (readPos));
        int i2 = (i1 + 1) % numSamples;
        int i3 = (i1 + 2) % numSamples;
        int i4 = (i1 + 3) % numSamples;

        jassert (i1 >= 0 && i1 < numSamples);
        float fraction = readPos - float (i1);

        float x[] = { 0.0f, 1.0f, 2.0f, 3.0f };
        float y[] = { data[ch][i1], data[ch][i2], data[ch][i3], data[ch][i4] };

        return Lagrange::interpolate (x, y, 4, fraction);
    }

    inline float readLinear (int ch, double t)
    {
        int numSamples = buffer.getNumSamples();

        jassert (t >= 0.0f && t < (numSamples / sampleRate));

        float readPos = std::fmod (float (writePos - 1 + numSamples - (t * sampleRate)), float (numSamples));
        int prev = int (std::floor (readPos));
        int next = (prev + 1) % numSamples;

        jassert (next >= 0 && next < numSamples);
        jassert (prev >= 0 && prev < numSamples);

        float fraction = readPos - float (prev);

        return (1.0f - fraction) * data[ch][prev] + fraction * data[ch][next];
    }

    inline float readSample (int ch, int samplePos)
    {
        int numSamples = buffer.getNumSamples();

        jassert (samplePos >= 0 && samplePos < numSamples);

        auto readPos = (writePos - 1 + numSamples - samplePos) % numSamples;
        jassert (readPos >= 0 && readPos < numSamples);
        return data[ch][readPos];
    }

    inline float readSampleLagrange (int ch, float samplePos)
    {
        int numSamples = buffer.getNumSamples();

        float readPos = std::fmod (float (writePos - 1 + numSamples - samplePos), float (numSamples));

        int i1 = int (std::floor (readPos));
        int i2 = (i1 + 1) % numSamples;
        int i3 = (i1 + 2) % numSamples;
        int i4 = (i1 + 3) % numSamples;

        jassert (i1 >= 0 && i1 < numSamples);
        float fraction = readPos - float (i1);

        float x[] = { 0.0f, 1.0f, 2.0f, 3.0f };
        float y[] = { data[ch][i1], data[ch][i2], data[ch][i3], data[ch][i4] };

        return Lagrange::interpolate (x, y, 4, fraction);
    }

    inline void write (int ch, float input)
    {
        data[ch][writePos] = input;
    }

    inline void writeFinished()
    {
        writePos++;

        if (writePos >= buffer.getNumSamples())
            writePos = 0;
    }

protected:
    juce::AudioSampleBuffer buffer;
    float* const* data = nullptr;

    double sampleRate = 44100.0;
    int writePos = 0;
};

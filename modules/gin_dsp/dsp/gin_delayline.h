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

    inline float readLinear (int ch, double t)
    {
        int numSamples = buffer.getNumSamples();
        
        jassert (t >= 0.0f && t < (numSamples / sampleRate));
        
        float readPos = (float) std::fmod (writePos + numSamples - (t * sampleRate), numSamples);
        int prev = int (std::floor (readPos));
        int next = (prev + 1) % numSamples;

        float fraction = readPos - prev;
        
        return (1.0f - fraction) * data[ch][prev] + fraction * data[ch][next];
    }
    
    inline float readSample (int ch, int samplePos)
    {
        int numSamples = buffer.getNumSamples();
        
        jassert (samplePos >= 0 && samplePos < numSamples);
        
        auto readPos = (writePos + numSamples - samplePos) % numSamples;
        return data[ch][readPos];
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
    float** data = nullptr;
    
    double sampleRate = 44100.0;
    int writePos = 0;
};

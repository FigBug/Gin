/*
  ==============================================================================

    AudioUtil.h
    Created: 24 Jan 2018 7:06:35pm
    Author:  Roland Rabien

  ==============================================================================
*/

#pragma once

//==============================================================================
inline Value findValue (ValueTree& state, Identifier name, var value)
{
    if (state.hasProperty (name))
        return state.getPropertyAsValue (name, nullptr);
    
    state.setProperty (name, value, nullptr);
    return state.getPropertyAsValue (name, nullptr);
}

//==============================================================================
class AudioFifo
{
public:
    AudioFifo (int channels, int numSamples)
      : fifo (numSamples), buffer (channels, numSamples)
    {
    }
    
    void setSize (int numChannels, int numSamples)
    {
        fifo.setTotalSize (numSamples);
        buffer.setSize (numChannels, numSamples);
    }
    
    int getFreeSpace() const noexcept       { return fifo.getFreeSpace(); }
    int getNumReady() const noexcept        { return fifo.getNumReady(); }
    void reset() noexcept                   { fifo.reset(); }
    
    void ensureFreeSpace (int numSamples)
    {
        const int freeSpace = getFreeSpace();
        if (numSamples > freeSpace)
        {
            const int samplesRequired = numSamples - freeSpace;
            jassert (samplesRequired <= getNumReady());
            fifo.finishedRead (samplesRequired);
        }
    }
    
    bool write (const AudioSampleBuffer& src)
    {
        return write (src.getArrayOfReadPointers(), src.getNumSamples());
    }
    
    bool write (const float* const* data, int numSamples)
    {
        if (numSamples <= 0)
            return true;
        
        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);
        
        if (size1 + size2 < numSamples)
            return false;
        
        for (int i = buffer.getNumChannels(); --i >= 0;)
        {
            buffer.copyFrom (i, start1, data[i], size1);
            buffer.copyFrom (i, start2, data[i] + size1, size2);
        }
        
        fifo.finishedWrite (size1 + size2);
        return true;
    }
    
    bool read (AudioSampleBuffer& dest, int startSampleInDestBuffer)
    {
        return read (dest, startSampleInDestBuffer, dest.getNumSamples());
    }
    
    bool read (AudioSampleBuffer& dest, int startSampleInDestBuffer, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);
        
        if ((size1 + size2) < numSamples)
            return false;
        
        for (int i = buffer.getNumChannels(); --i >= 0;)
        {
            dest.copyFrom (i, startSampleInDestBuffer, buffer, i, start1, size1);
            dest.copyFrom (i, startSampleInDestBuffer + size1, buffer, i, start2, size2);
        }
        
        fifo.finishedRead (size1 + size2);
        return true;
    }
    
    bool readAdding (AudioSampleBuffer& dest, int startSampleInDestBuffer)
    {
        return readAdding (dest, startSampleInDestBuffer, dest.getNumSamples());
    }
    
    bool readAdding (AudioSampleBuffer& dest, int startSampleInDestBuffer, int numSamples)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);
        
        if ((size1 + size2) < numSamples)
            return false;
        
        for (int i = buffer.getNumChannels(); --i >= 0;)
        {
            dest.addFrom (i, startSampleInDestBuffer, buffer, i, start1, size1);
            dest.addFrom (i, startSampleInDestBuffer + size1, buffer, i, start2, size2);
        }
        
        fifo.finishedRead (size1 + size2);
        return true;
    }
    
private:
    AbstractFifo fifo;
    AudioSampleBuffer buffer;
    
    JUCE_DECLARE_NON_COPYABLE (AudioFifo)
};

//==============================================================================
inline double getMidiNoteInHertz (const double noteNumber, const double frequencyOfA = 440.0)
{
    return frequencyOfA * pow (2.0, (noteNumber - 69) / 12.0);
}

//==============================================================================
void applyGain (AudioSampleBuffer& buffer, LinearSmoothedValue<float>& gain);

void applyGain (AudioSampleBuffer& buffer, int channel, LinearSmoothedValue<float>& gain);

//==============================================================================
template <class T>
class ValueSmoother
{
public:
    void reset (double sr, double time)
    {
        delta = 1.0f / float (sr * time);
    }
    
    T getCurrentValue()
    {
        return currentValue;
    }
    
    void process (int n)
    {
        if (targetValue != currentValue)
            for (int i = 0; i < n; i++)
                updateValue();
    }
    
    void updateValue()
    {
        if (currentValue < targetValue)
            currentValue = jmin (targetValue, currentValue + delta);
        else if (currentValue > targetValue)
            currentValue = jmax (targetValue, currentValue - delta);
    }
    
    T getNextValue()
    {
        if (currentValue < targetValue)
            currentValue = jmin (targetValue, currentValue + delta);
        else if (currentValue > targetValue)
            currentValue = jmax (targetValue, currentValue - delta);
        return currentValue;
    }
    
    T* getValuePtr()
    {
        return &currentValue;
    }
    
    void setValue (T v)
    {
        targetValue = v;
    }
    
    void snapToValue()
    {
        currentValue = targetValue;
    }
    
    void setValueUnsmoothed (T v)
    {
        targetValue = v;
        currentValue = v;
    }
    
private:
    T delta = 0;
    T targetValue = 0;
    T currentValue = 0;
};

//==============================================================================
class LevelTracker
{
public:
    LevelTracker (float decayPerSecond);
    
    void trackBuffer (const float* buffer, int numSamples);
    void trackBuffer (AudioSampleBuffer& buffer);
    
    float getLevel();
    bool getClip()      { return clip;  }
    void clearClip()    { clip = false; }
    
protected:
    float peakTime  {0.0f};
    float peakLevel {-100.0f};
    float decayRate {0.0f};
    bool clip {false};
};

//==============================================================================
// Type string for a midi message
String getMidiMessageType (const MidiMessage& msg);

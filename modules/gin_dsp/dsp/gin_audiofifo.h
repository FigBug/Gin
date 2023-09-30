/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** FIFO - stuff audio in one end and it pops out the other.
    Lock free for single producer / consumer
*/
class AudioFifo
{
public:
    AudioFifo (int channels = 2, int numSamples = 128)
      : fifo (numSamples), buffer (channels, numSamples)
    {
    }

    void setSize (int numChannels, int numSamples)
    {
        fifo.setTotalSize (numSamples);
        buffer.setSize (numChannels, numSamples);
    }

    int getNumChannels() const noexcept     { return buffer.getNumChannels(); }
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

    bool write (const juce::AudioSampleBuffer& src, int numSamples = -1)
    {
        if (numSamples == -1)
            numSamples = src.getNumSamples();

        return write (src.getArrayOfReadPointers(), numSamples);
    }

    bool write (const float* const* data, int numSamples)
    {
        if (numSamples <= 0)
            return true;

        jassert (numSamples <= getFreeSpace());

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

    bool writeMono (const float* data, int numSamples)
    {
        jassert (numSamples <= getFreeSpace());
        jassert (buffer.getNumChannels() == 1);

        if (numSamples <= 0)
            return true;

        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

        if (size1 + size2 < numSamples)
            return false;

        buffer.copyFrom (0, start1, data, size1);
        buffer.copyFrom (0, start2, data + size1, size2);

        fifo.finishedWrite (size1 + size2);
        return true;
    }

    bool read (juce::AudioSampleBuffer& dest)
    {
        return read (dest, 0, dest.getNumSamples());
    }

    bool read (juce::AudioSampleBuffer& dest, int startSampleInDestBuffer, int numSamples)
    {
        jassert (getNumReady() >= numSamples);

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

    bool readMono (float* data, int numSamples)
    {
        jassert (getNumReady() >= numSamples);
        jassert (buffer.getNumChannels() == 1);

        if (numSamples <= 0)
            return true;

        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);

        if ((size1 + size2) < numSamples)
            return false;

        juce::FloatVectorOperations::copy (data, buffer.getReadPointer (0, start1), size1);
        juce::FloatVectorOperations::copy (data + size1, buffer.getReadPointer (0, start2), size2);

        fifo.finishedRead (size1 + size2);

        return true;
    }

    bool readAdding (juce::AudioSampleBuffer& dest)
    {
        return readAdding (dest, 0, dest.getNumSamples());
    }

    bool readAdding (juce::AudioSampleBuffer& dest, int startSampleInDestBuffer, int numSamples)
    {
        jassert (getNumReady() >= numSamples);

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
    juce::AbstractFifo fifo;
    juce::AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE (AudioFifo)
};

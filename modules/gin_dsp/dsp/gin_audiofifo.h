/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Lock-free audio FIFO buffer for single producer/consumer scenarios.

    AudioFifo provides a thread-safe circular buffer for passing audio between threads,
    typically from an audio callback to a background processing thread or vice versa.
    It uses JUCE's AbstractFifo for lock-free operation with a single producer and
    single consumer thread.

    Key Features:
    - Lock-free for single producer/consumer (real-time safe)
    - Multi-channel audio support
    - Read/write operations with optional adding
    - Peek operations (read without consuming)
    - Silence writing
    - Automatic space management

    Common Use Cases:
    - Passing audio from real-time thread to UI analyzer
    - Buffering between processing stages
    - Circular recording buffer
    - Audio preview/monitoring

    Thread Safety:
    - Safe for ONE producer thread and ONE consumer thread
    - NOT safe for multiple producers or multiple consumers
    - Write operations must be on one thread only
    - Read operations must be on one thread only

    Usage:
    @code
    AudioFifo fifo(2, 4096); // 2 channels, 4096 samples

    // In audio thread (producer)
    if (fifo.getFreeSpace() >= buffer.getNumSamples())
        fifo.write(buffer);

    // In background thread (consumer)
    if (fifo.getNumReady() >= desiredSamples)
    {
        AudioBuffer<float> output(2, desiredSamples);
        fifo.read(output);
        // ... process output
    }
    @endcode

    @see MidiFifo, AudioMidiFifo, ResamplingFifo
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

    bool writeSilence (int numSamples)
    {
        if (numSamples <= 0)
            return true;

        jassert (numSamples <= getFreeSpace());

        int start1, size1, start2, size2;
        fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

        if (size1 + size2 < numSamples)
            return false;

        buffer.clear (start1, size1);
        buffer.clear (start2, size2);

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

	bool peek (juce::AudioSampleBuffer& dest)
	{
		return peek (dest, 0, dest.getNumSamples());
	}

	bool peek (juce::AudioSampleBuffer& dest, int startSampleInDestBuffer, int numSamples)
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

		return true;
	}

    float peekSample (int channel, int sample)
    {
        jassert (sample < getNumReady());

        int start1, size1, start2, size2;
        fifo.prepareToRead (sample + 1, start1, size1, start2, size2);

        if (sample < size1)
            return buffer.getSample (channel, start1 + sample);
        else
            return buffer.getSample (channel, start2 + (sample - size1));
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

    bool pop (int numSamples)
    {
        jassert (getNumReady() >= numSamples);

        int start1, size1, start2, size2;
        fifo.prepareToRead (numSamples, start1, size1, start2, size2);

        if ((size1 + size2) < numSamples)
            return false;

        fifo.finishedRead (size1 + size2);
        return true;
    }

private:
    juce::AbstractFifo fifo;
    juce::AudioSampleBuffer buffer;

    JUCE_DECLARE_NON_COPYABLE (AudioFifo)
};

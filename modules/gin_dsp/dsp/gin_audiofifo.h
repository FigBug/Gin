/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

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
    /** Creates an AudioFifo with the specified number of channels and sample capacity.
        @param channels Number of audio channels (default: 2)
        @param numSamples Maximum number of samples the FIFO can hold (default: 128)
    */
    AudioFifo (int channels = 2, int numSamples = 128)
      : fifo (numSamples + 1), buffer (channels, numSamples + 1)
    {
    }

    /** Resizes the FIFO to a new capacity.
        Note: This is not thread-safe and should only be called when no other
        threads are accessing the FIFO.
        @param numChannels New number of audio channels
        @param numSamples New maximum number of samples
    */
    void setSize (int numChannels, int numSamples)
    {
        fifo.setTotalSize (numSamples + 1);
        buffer.setSize (numChannels, numSamples + 1);
    }

    /** Returns the number of audio channels. */
    int getNumChannels() const noexcept     { return buffer.getNumChannels(); }

    /** Returns the number of samples that can be written before the FIFO is full. */
    int getFreeSpace() const noexcept       { return fifo.getFreeSpace(); }

    /** Returns the number of samples available to read. */
    int getNumReady() const noexcept        { return fifo.getNumReady(); }

    /** Resets the FIFO to an empty state.
        Note: This is not thread-safe and should only be called when no other
        threads are accessing the FIFO.
    */
    void reset() noexcept                   { fifo.reset(); }

    /** Ensures at least the specified amount of free space by discarding oldest data if needed.
        If the requested free space is greater than currently available, this will discard
        the oldest samples to make room.
        @param numSamples The minimum number of free samples required
    */
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

    /** Writes audio data from an AudioSampleBuffer to the FIFO.
        Should only be called from the producer thread.
        @param src The source buffer to write from
        @param numSamples Number of samples to write (default: -1 writes all samples from src)
        @return true if successful, false if there wasn't enough space
    */
    bool write (const juce::AudioSampleBuffer& src, int numSamples = -1)
    {
        if (numSamples == -1)
            numSamples = src.getNumSamples();

        return write (src.getArrayOfReadPointers(), numSamples);
    }

    /** Writes audio data from a multi-channel array to the FIFO.
        Should only be called from the producer thread.
        @param data Array of channel pointers containing the audio data
        @param numSamples Number of samples to write
        @return true if successful, false if there wasn't enough space
    */
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

    /** Writes silence (zeros) to the FIFO.
        Should only be called from the producer thread.
        @param numSamples Number of silent samples to write
        @return true if successful, false if there wasn't enough space
    */
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

    /** Writes mono audio data to a single-channel FIFO.
        Should only be called from the producer thread. The FIFO must be configured
        for single-channel (mono) operation.
        @param data Pointer to the mono audio data
        @param numSamples Number of samples to write
        @return true if successful, false if there wasn't enough space
    */
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

	/** Peeks at (reads without consuming) audio data from the FIFO.
        The data remains in the FIFO and can be read again.
        Should only be called from the consumer thread.
        @param dest Destination buffer to copy the audio data into
        @return true if successful, false if there weren't enough samples available
    */
	bool peek (juce::AudioSampleBuffer& dest)
	{
		return peek (dest, 0, dest.getNumSamples());
	}

	/** Peeks at (reads without consuming) audio data from the FIFO.
        The data remains in the FIFO and can be read again.
        Should only be called from the consumer thread.
        @param dest Destination buffer to copy the audio data into
        @param startSampleInDestBuffer Starting position in the destination buffer
        @param numSamples Number of samples to peek
        @return true if successful, false if there weren't enough samples available
    */
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

    /** Peeks at a single sample without consuming it.
        Should only be called from the consumer thread.
        @param channel The channel to peek from
        @param sample The sample index to peek (0 = first available sample)
        @return The sample value at the specified position
    */
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

    /** Reads and removes audio data from the FIFO.
        Should only be called from the consumer thread.
        @param dest Destination buffer to copy the audio data into
        @return true if successful, false if there weren't enough samples available
    */
    bool read (juce::AudioSampleBuffer& dest)
    {
        return read (dest, 0, dest.getNumSamples());
    }

    /** Reads and removes audio data from the FIFO.
        Should only be called from the consumer thread.
        @param dest Destination buffer to copy the audio data into
        @param startSampleInDestBuffer Starting position in the destination buffer
        @param numSamples Number of samples to read
        @return true if successful, false if there weren't enough samples available
    */
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

    /** Reads and removes mono audio data from a single-channel FIFO.
        Should only be called from the consumer thread. The FIFO must be configured
        for single-channel (mono) operation.
        @param data Pointer to the destination array for the audio data
        @param numSamples Number of samples to read
        @return true if successful, false if there weren't enough samples available
    */
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

    /** Reads and removes audio data from the FIFO, adding it to the destination buffer.
        Unlike read(), this adds the FIFO data to the destination rather than replacing it.
        Should only be called from the consumer thread.
        @param dest Destination buffer to add the audio data to
        @return true if successful, false if there weren't enough samples available
    */
    bool readAdding (juce::AudioSampleBuffer& dest)
    {
        return readAdding (dest, 0, dest.getNumSamples());
    }

    /** Reads and removes audio data from the FIFO, adding it to the destination buffer.
        Unlike read(), this adds the FIFO data to the destination rather than replacing it.
        Should only be called from the consumer thread.
        @param dest Destination buffer to add the audio data to
        @param startSampleInDestBuffer Starting position in the destination buffer
        @param numSamples Number of samples to read
        @return true if successful, false if there weren't enough samples available
    */
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

    /** Removes (discards) samples from the FIFO without reading them.
        This is useful after peek() operations when you want to consume the peeked data.
        Should only be called from the consumer thread.
        @param numSamples Number of samples to discard
        @return true if successful, false if there weren't enough samples available
    */
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

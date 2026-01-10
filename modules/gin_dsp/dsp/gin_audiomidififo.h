/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Combined audio and MIDI buffer for synchronized processing within a single thread.

    AudioMidiFifo provides a circular buffer that handles both audio samples
    and MIDI messages together, maintaining their temporal relationship. This
    is NOT thread-safe and should only be used within a single thread for
    operations that need synchronized audio and MIDI buffering.

    Key Features:
    - Synchronized audio and MIDI storage
    - Automatic time offset management for MIDI events
    - Multi-channel audio support
    - Silence writing capability
    - Uses AbstractFifo for audio portion

    Thread Safety:
    - NOT thread-safe
    - Use only within a single thread
    - For multi-threaded use, consider separate synchronization

    The MIDI timestamps are automatically adjusted to maintain correct timing
    relative to the audio samples when reading and writing.

    Usage:
    @code
    AudioMidiFifo fifo(2, 8192); // 2 channels, 8192 samples max

    // Write audio and MIDI together (single thread)
    AudioBuffer<float> audioIn(2, 512);
    MidiBuffer midiIn;
    // ... fill buffers ...
    fifo.write(audioIn, midiIn);

    // Read audio and MIDI together (same thread)
    if (fifo.getNumSamplesAvailable() >= 512)
    {
        AudioBuffer<float> audioOut(2, 512);
        MidiBuffer midiOut;
        fifo.read(audioOut, midiOut);
        // ... process synchronized audio and MIDI
    }
    @endcode

    @see AudioFifo, MidiFifo
*/
class AudioMidiFifo
{
public:
	/** Creates an AudioMidiFifo with the specified capacity.
		@param channels Number of audio channels (default: 2)
		@param maxSize Maximum number of audio samples the FIFO can hold (default: 1024)
	*/
	AudioMidiFifo (int channels = 2, int maxSize = 1024)
	{
		setSize (channels, maxSize);
	}

	/** Resizes the FIFO to a new capacity.
		This clears any existing data in the FIFO.
		@param channels New number of audio channels
		@param maxSize New maximum number of audio samples
	*/
	void setSize (int channels, int maxSize)
	{
		fifo.setTotalSize (maxSize + 1);
		audioBuffer.setSize (channels, maxSize + 1);

		clear();
	}

	/** Clears all audio and MIDI data from the FIFO, resetting it to an empty state. */
	void clear()
	{
		fifo.reset();
		audioBuffer.clear();
		midiBuffer.clear();
	}

	/** Returns the number of audio samples available to read.
		@return The number of samples that can be read from the FIFO
	*/
	int getNumSamplesAvailable()
	{
		return fifo.getNumReady();
	}

	/** Returns the number of audio samples that can be written before the FIFO is full.
		@return The number of free sample slots
	*/
	int getNumSamplesFree()
	{
		return fifo.getFreeSpace();
	}

	/** Writes silence (zeros) to the audio portion of the FIFO.
		No MIDI events are added.
		@param numSamples Number of silent samples to write
	*/
	void writeSilence (int numSamples)
	{
		jassert (getNumSamplesFree() >= numSamples);

		int start1, size1, start2, size2;
		fifo.prepareToWrite (numSamples, start1, size1, start2, size2);

		if (size1 > 0)
			audioBuffer.clear (start1, size1);
		if (size2 > 0)
			audioBuffer.clear (start2, size2);

		fifo.finishedWrite (size1 + size2);
	}

	/** Writes audio and MIDI data to the FIFO.
		MIDI event timestamps are automatically adjusted to maintain correct timing
		within the FIFO's internal timeline.
		@param audioSrc Source audio buffer to write
		@param midiSrc Source MIDI buffer to write (timestamps relative to audioSrc)
	*/
	void write (const juce::AudioBuffer<float>& audioSrc, const juce::MidiBuffer& midiSrc)
	{
		jassert (getNumSamplesFree() >= audioSrc.getNumSamples());
		jassert (audioSrc.getNumChannels() == audioBuffer.getNumChannels());

		midiBuffer.addEvents (midiSrc, 0, audioSrc.getNumSamples(), fifo.getNumReady());

		int start1, size1, start2, size2;
		fifo.prepareToWrite (audioSrc.getNumSamples(), start1, size1, start2, size2);

		int channels = juce::jmin (audioBuffer.getNumChannels(), audioSrc.getNumChannels());
		for (int ch = 0; ch < channels; ch++)
		{
			if (size1 > 0)
				audioBuffer.copyFrom (ch, start1, audioSrc, ch, 0, size1);
			if (size2 > 0)
				audioBuffer.copyFrom (ch, start2, audioSrc, ch, size1, size2);
		}

		fifo.finishedWrite (size1 + size2);
	}

	/** Reads audio and MIDI data from the FIFO.
		MIDI event timestamps are automatically adjusted to be relative to the start
		of the destination audio buffer.
		@param audioDst Destination audio buffer to read into
		@param midiDst Destination MIDI buffer to read into (timestamps will be relative to audioDst)
	*/
	void read (juce::AudioBuffer<float>& audioDst, juce::MidiBuffer& midiDst)
	{
		jassert (getNumSamplesAvailable() >= audioDst.getNumSamples());
		jassert (audioDst.getNumChannels() == audioBuffer.getNumChannels());

		midiDst.addEvents (midiBuffer, 0, audioDst.getNumSamples(), 0);

		// Shunt midi forwards
		juce::MidiBuffer temp;
		temp.addEvents (midiBuffer, audioDst.getNumSamples(), fifo.getNumReady(), -audioDst.getNumSamples());
		midiBuffer = temp;

		int start1, size1, start2, size2;
		fifo.prepareToRead (audioDst.getNumSamples(), start1, size1, start2, size2);

		int numCh = juce::jmin (audioBuffer.getNumChannels(), audioDst.getNumChannels());
		for (int ch = 0; ch < numCh; ch++)
		{
			if (size1 > 0)
				audioDst.copyFrom (ch, 0, audioBuffer, ch, start1, size1);
			if (size2 > 0)
				audioDst.copyFrom (ch, size1, audioBuffer, ch, start2, size2);
		}

		fifo.finishedRead (size1 + size2);
	}

 private:
	juce::AbstractFifo fifo {1};
	juce::AudioBuffer<float> audioBuffer;
	juce::MidiBuffer midiBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioMidiFifo)
};

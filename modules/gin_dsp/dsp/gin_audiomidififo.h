/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

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
	AudioMidiFifo (int channels = 2, int maxSize = 1024)
	{
		setSize (channels, maxSize);
	}

	void setSize (int channels, int maxSize)
	{
		fifo.setTotalSize (maxSize + 1);
		audioBuffer.setSize (channels, maxSize + 1);

		clear();
	}

	void clear()
	{
		fifo.reset();
		audioBuffer.clear();
		midiBuffer.clear();
	}

	int getNumSamplesAvailable()
	{
		return fifo.getNumReady();
	}
	
	int getNumSamplesFree()
	{
		return fifo.getFreeSpace();
	}

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

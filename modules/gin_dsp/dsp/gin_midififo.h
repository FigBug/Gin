/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    FIFO buffer for passing MIDI messages between threads.

    MidiFifo provides a simple buffer for storing and retrieving MIDI messages,
    useful for passing MIDI data from an audio callback to a processing thread
    or for buffering MIDI events across processing boundaries.

    Unlike AudioFifo, this is a simple time-shifted buffer rather than a true
    lock-free FIFO. MIDI events are time-stamped relative to the write position
    and adjusted when read.

    Key Features:
    - Buffers MIDI messages with timing information
    - Simple read/write interface
    - Automatic time offset adjustment
    - Clear operation for reset

    Usage:
    @code
    MidiFifo midiFifo;

    // In audio callback (write MIDI)
    midiFifo.write(incomingMidi, blockSize);

    // Later (read MIDI)
    MidiBuffer outputMidi;
    if (midiFifo.getSamplesUsed() >= blockSize)
        midiFifo.read(outputMidi, blockSize);
    @endcode

    @see AudioFifo, AudioMidiFifo
*/
class MidiFifo
{
public:
    MidiFifo() = default;

    void clear()
    {
        numReady = 0;
        midiBuffer.clear();
    }

    int getSamplesUsed()
    {
        return numReady;
    }

    void write (const juce::MidiBuffer& midiSrc, int numSamples)
    {
        midiBuffer.addEvents (midiSrc, 0, numSamples, numReady);
        numReady += numSamples;
    }

    void read (juce::MidiBuffer& midiDst, int numSamples)
    {
        midiDst.addEvents (midiBuffer, 0, numSamples, 0);

        // Shunt midi forwards
        juce::MidiBuffer temp;
        temp.addEvents (midiBuffer, numSamples, numReady, -numSamples);
        midiBuffer = temp;

        numReady -= numSamples;
    }

 private:
    int numReady = 0;
    juce::MidiBuffer midiBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiFifo)
};

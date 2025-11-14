/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Simple MIDI buffer for time-shifting MIDI events within a single thread.

    MidiFifo provides a simple buffer for storing and retrieving MIDI messages
    with automatic time offset adjustment. This is NOT thread-safe and should
    only be used within a single thread for buffering MIDI events across
    processing boundaries.

    Unlike AudioFifo, this is a simple time-shifted buffer that shifts MIDI
    event timestamps as data is written and read. It's useful for delaying
    MIDI events or accumulating MIDI across multiple processing blocks.

    Key Features:
    - Buffers MIDI messages with timing information
    - Automatic time offset adjustment
    - Simple read/write interface
    - Clear operation for reset

    Thread Safety:
    - NOT thread-safe
    - Use only within a single thread
    - For multi-threaded use, consider other synchronization mechanisms

    Usage:
    @code
    MidiFifo midiFifo;

    // Accumulate MIDI in single thread
    midiFifo.write(incomingMidi, blockSize);

    // Later in same thread
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

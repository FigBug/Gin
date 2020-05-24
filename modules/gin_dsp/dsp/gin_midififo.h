/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** FIFO - stuff midi in one end and it pops out the other.
*/

//================================================================================================
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

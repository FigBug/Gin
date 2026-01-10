/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class MidiFifoTests : public juce::UnitTest
{
public:
    MidiFifoTests() : juce::UnitTest ("MIDI FIFO", "gin_dsp") {}

    void runTest() override
    {
        testBasicWriteRead();
        testSamplesUsedCount();
        testTimeShifting();
        testClear();
    }

private:
    void testBasicWriteRead()
    {
        beginTest ("Basic Write and Read");

        MidiFifo fifo;

        // Create some MIDI events
        juce::MidiBuffer writeBuf;
        writeBuf.addEvent (juce::MidiMessage::noteOn (1, 60, (juce::uint8) 100), 10);
        writeBuf.addEvent (juce::MidiMessage::noteOff (1, 60), 50);

        fifo.write (writeBuf, 100);
        expectEquals (fifo.getSamplesUsed(), 100, "Should have 100 samples used");

        juce::MidiBuffer readBuf;
        fifo.read (readBuf, 100);

        expectEquals (fifo.getSamplesUsed(), 0, "Should have 0 samples after read");
        expect (!readBuf.isEmpty(), "Read buffer should contain events");
    }

    void testSamplesUsedCount()
    {
        beginTest ("Samples Used Count");

        MidiFifo fifo;

        juce::MidiBuffer buf;
        fifo.write (buf, 50);
        expectEquals (fifo.getSamplesUsed(), 50, "Should track written samples");

        fifo.write (buf, 30);
        expectEquals (fifo.getSamplesUsed(), 80, "Should accumulate samples");

        juce::MidiBuffer readBuf;
        fifo.read (readBuf, 20);
        expectEquals (fifo.getSamplesUsed(), 60, "Should subtract read samples");
    }

    void testTimeShifting()
    {
        beginTest ("Time Shifting");

        MidiFifo fifo;

        // Write event at sample 10
        juce::MidiBuffer writeBuf;
        writeBuf.addEvent (juce::MidiMessage::noteOn (1, 60, (juce::uint8) 100), 10);

        fifo.write (writeBuf, 100);

        // Read first 50 samples - event should still be at sample 10
        juce::MidiBuffer readBuf1;
        fifo.read (readBuf1, 50);

        int eventTime = -1;
        for (const auto meta : readBuf1)
        {
            eventTime = meta.samplePosition;
            break;
        }
        expectEquals (eventTime, 10, "Event should be at sample 10 in first read");
    }

    void testClear()
    {
        beginTest ("Clear");

        MidiFifo fifo;

        juce::MidiBuffer buf;
        buf.addEvent (juce::MidiMessage::noteOn (1, 60, (juce::uint8) 100), 10);
        fifo.write (buf, 100);

        expect (fifo.getSamplesUsed() > 0, "Should have samples before clear");

        fifo.clear();
        expectEquals (fifo.getSamplesUsed(), 0, "Should have 0 samples after clear");
    }
};

static MidiFifoTests midiFifoTests;

#endif

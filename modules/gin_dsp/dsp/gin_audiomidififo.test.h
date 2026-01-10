/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AudioMidiFifoTests : public juce::UnitTest
{
public:
    AudioMidiFifoTests() : juce::UnitTest ("Audio MIDI FIFO", "gin_dsp") {}

    void runTest() override
    {
        testBasicWriteRead();
        testFreeSpaceAndAvailable();
        testCircularBuffer();
        testMidiTimestampAdjustment();
        testWriteSilence();
        testClearOperation();
        testSetSize();
        testMultipleWrites();
    }

private:
    void testBasicWriteRead()
    {
        beginTest ("Basic Write and Read");

        AudioMidiFifo fifo (2, 1024);

        // Create audio data
        juce::AudioBuffer<float> writeAudio (2, 100);
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                writeAudio.setSample (ch, i, (float) i * (ch + 1));

        // Create MIDI data
        juce::MidiBuffer writeMidi;
        writeMidi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 10);
        writeMidi.addEvent (juce::MidiMessage::noteOff (1, 60), 50);

        // Write to FIFO
        fifo.write (writeAudio, writeMidi);

        expectEquals (fifo.getNumSamplesAvailable(), 100, "Should have 100 samples available");

        // Read from FIFO
        juce::AudioBuffer<float> readAudio (2, 100);
        juce::MidiBuffer readMidi;
        fifo.read (readAudio, readMidi);

        expectEquals (fifo.getNumSamplesAvailable(), 0, "Should have 0 samples after read");

        // Verify audio data
        for (int ch = 0; ch < 2; ch++)
        {
            for (int i = 0; i < 100; i++)
            {
                float expected = (float) i * (ch + 1);
                expectWithinAbsoluteError (readAudio.getSample (ch, i), expected, 0.0001f, "Audio data should match");
            }
        }

        // Verify MIDI data
        int midiEventCount = 0;
        for (auto metadata : readMidi)
        {
            midiEventCount++;
            if (midiEventCount == 1)
            {
                expectEquals (metadata.samplePosition, 10, "First MIDI event should be at position 10");
                expect (metadata.getMessage().isNoteOn(), "First event should be note on");
                expectEquals (metadata.getMessage().getNoteNumber(), 60, "Note number should be 60");
            }
            else if (midiEventCount == 2)
            {
                expectEquals (metadata.samplePosition, 50, "Second MIDI event should be at position 50");
                expect (metadata.getMessage().isNoteOff(), "Second event should be note off");
            }
        }

        expectEquals (midiEventCount, 2, "Should have 2 MIDI events");
    }

    void testFreeSpaceAndAvailable()
    {
        beginTest ("Free Space and Available Count");

        AudioMidiFifo fifo (1, 256);

        expectEquals (fifo.getNumSamplesFree(), 256, "Initial free space should be 256");
        expectEquals (fifo.getNumSamplesAvailable(), 0, "Initially should have no samples available");

        juce::AudioBuffer<float> buffer (1, 64);
        juce::MidiBuffer midi;
        fifo.write (buffer, midi);

        expectEquals (fifo.getNumSamplesFree(), 192, "Free space should decrease");
        expectEquals (fifo.getNumSamplesAvailable(), 64, "Available count should increase");

        juce::AudioBuffer<float> readBuffer (1, 32);
        juce::MidiBuffer readMidi;
        fifo.read (readBuffer, readMidi);

        expectEquals (fifo.getNumSamplesFree(), 224, "Free space should increase after read");
        expectEquals (fifo.getNumSamplesAvailable(), 32, "Available count should decrease after read");
    }

    void testCircularBuffer()
    {
        beginTest ("Circular Buffer Behavior");

        AudioMidiFifo fifo (1, 128);

        // Write and read multiple times to wrap around
        for (int round = 0; round < 10; round++)
        {
            juce::AudioBuffer<float> writeBuffer (1, 50);
            juce::MidiBuffer writeMidi;

            // Fill with round-specific data
            for (int i = 0; i < 50; i++)
                writeBuffer.setSample (0, i, (float) (round * 100 + i));

            writeMidi.addEvent (juce::MidiMessage::noteOn (1, 60 + round, 0.8f), 25);

            fifo.write (writeBuffer, writeMidi);

            juce::AudioBuffer<float> readBuffer (1, 50);
            juce::MidiBuffer readMidi;
            fifo.read (readBuffer, readMidi);

            // Verify audio
            for (int i = 0; i < 50; i++)
            {
                float expected = (float) (round * 100 + i);
                expectWithinAbsoluteError (readBuffer.getSample (0, i), expected, 0.0001f, "Circular audio data should be correct");
            }

            // Verify MIDI
            for (auto metadata : readMidi)
            {
                expectEquals (metadata.samplePosition, 25, "MIDI event should be at position 25");
                expectEquals (metadata.getMessage().getNoteNumber(), 60 + round, "Note number should match round");
            }
        }

        expectEquals (fifo.getNumSamplesAvailable(), 0, "FIFO should be empty after all rounds");
    }

    void testMidiTimestampAdjustment()
    {
        beginTest ("MIDI Timestamp Adjustment");

        AudioMidiFifo fifo (1, 512);

        // Write first block with MIDI at timestamp 10
        juce::AudioBuffer<float> audio1 (1, 100);
        juce::MidiBuffer midi1;
        midi1.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 10);
        fifo.write (audio1, midi1);

        // Write second block with MIDI at timestamp 20
        juce::AudioBuffer<float> audio2 (1, 100);
        juce::MidiBuffer midi2;
        midi2.addEvent (juce::MidiMessage::noteOn (1, 62, 0.8f), 20);
        fifo.write (audio2, midi2);

        // The MIDI events should now be at timestamps 10 and 120 (100 + 20) in the internal buffer

        // Read first 100 samples
        juce::AudioBuffer<float> readAudio1 (1, 100);
        juce::MidiBuffer readMidi1;
        fifo.read (readAudio1, readMidi1);

        // Should only get the first MIDI event, at timestamp 10
        int eventCount = 0;
        for (auto metadata : readMidi1)
        {
            eventCount++;
            expectEquals (metadata.samplePosition, 10, "First MIDI event should be at position 10");
            expectEquals (metadata.getMessage().getNoteNumber(), 60, "First note should be 60");
        }
        expectEquals (eventCount, 1, "Should have 1 MIDI event in first read");

        // Read second 100 samples
        juce::AudioBuffer<float> readAudio2 (1, 100);
        juce::MidiBuffer readMidi2;
        fifo.read (readAudio2, readMidi2);

        // Should get the second MIDI event, now adjusted to timestamp 20
        eventCount = 0;
        for (auto metadata : readMidi2)
        {
            eventCount++;
            expectEquals (metadata.samplePosition, 20, "Second MIDI event should be at position 20 (adjusted)");
            expectEquals (metadata.getMessage().getNoteNumber(), 62, "Second note should be 62");
        }
        expectEquals (eventCount, 1, "Should have 1 MIDI event in second read");
    }

    void testWriteSilence()
    {
        beginTest ("Write Silence");

        AudioMidiFifo fifo (2, 256);

        fifo.writeSilence (100);
        expectEquals (fifo.getNumSamplesAvailable(), 100, "Should have 100 samples available");

        juce::AudioBuffer<float> readBuffer (2, 100);
        juce::MidiBuffer readMidi;
        fifo.read (readBuffer, readMidi);

        // All samples should be zero
        float maxVal = 0.0f;
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                maxVal = std::max (maxVal, std::abs (readBuffer.getSample (ch, i)));

        expectWithinAbsoluteError (maxVal, 0.0f, 0.0001f, "Silence should be zeros");

        // Should be no MIDI events
        int midiCount = 0;
        for (auto metadata : readMidi)
        {
            juce::ignoreUnused (metadata);
            midiCount++;
        }
        expectEquals (midiCount, 0, "Should have no MIDI events after writing silence");
    }

    void testClearOperation()
    {
        beginTest ("Clear Operation");

        AudioMidiFifo fifo (1, 256);

        // Write some data
        juce::AudioBuffer<float> writeBuffer (1, 100);
        juce::MidiBuffer writeMidi;
        writeMidi.addEvent (juce::MidiMessage::noteOn (1, 60, 0.8f), 10);
        fifo.write (writeBuffer, writeMidi);

        expectEquals (fifo.getNumSamplesAvailable(), 100, "Should have 100 samples");

        // Clear the FIFO
        fifo.clear();

        expectEquals (fifo.getNumSamplesAvailable(), 0, "Should have 0 samples after clear");
        expectEquals (fifo.getNumSamplesFree(), 256, "Should have full capacity free after clear");

        // Verify we can write and read normally after clear
        fifo.write (writeBuffer, writeMidi);
        expectEquals (fifo.getNumSamplesAvailable(), 100, "Should be able to write after clear");
    }

    void testSetSize()
    {
        beginTest ("Set Size Operation");

        AudioMidiFifo fifo (1, 128);

        expectEquals (fifo.getNumSamplesFree(), 128, "Initial capacity should be 128");

        // Resize to larger
        fifo.setSize (2, 256);

        expectEquals (fifo.getNumSamplesFree(), 256, "Capacity should be 256 after resize");
        expectEquals (fifo.getNumSamplesAvailable(), 0, "Should be empty after resize");

        // Write and read to verify it works
        juce::AudioBuffer<float> writeBuffer (2, 100);
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                writeBuffer.setSample (ch, i, (float) i);

        juce::MidiBuffer writeMidi;
        fifo.write (writeBuffer, writeMidi);

        expectEquals (fifo.getNumSamplesAvailable(), 100, "Should have 100 samples");

        juce::AudioBuffer<float> readBuffer (2, 100);
        juce::MidiBuffer readMidi;
        fifo.read (readBuffer, readMidi);

        // Verify data
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                expectWithinAbsoluteError (readBuffer.getSample (ch, i), (float) i, 0.0001f, "Data should be correct after resize");
    }

    void testMultipleWrites()
    {
        beginTest ("Multiple Writes Before Read");

        AudioMidiFifo fifo (1, 512);

        // Write three blocks
        for (int block = 0; block < 3; block++)
        {
            juce::AudioBuffer<float> writeBuffer (1, 50);
            juce::MidiBuffer writeMidi;

            for (int i = 0; i < 50; i++)
                writeBuffer.setSample (0, i, (float) (block * 100 + i));

            writeMidi.addEvent (juce::MidiMessage::noteOn (1, 60 + block, 0.8f), 10 + block * 5);

            fifo.write (writeBuffer, writeMidi);
        }

        expectEquals (fifo.getNumSamplesAvailable(), 150, "Should have 150 samples");

        // Read all at once
        juce::AudioBuffer<float> readBuffer (1, 150);
        juce::MidiBuffer readMidi;
        fifo.read (readBuffer, readMidi);

        // Verify audio - first 50 samples should be 0-49, next 50 should be 100-149, etc.
        for (int block = 0; block < 3; block++)
        {
            for (int i = 0; i < 50; i++)
            {
                float expected = (float) (block * 100 + i);
                expectWithinAbsoluteError (readBuffer.getSample (0, block * 50 + i), expected, 0.0001f, "Audio should be correct across blocks");
            }
        }

        // Verify MIDI - should have 3 events at appropriate timestamps
        int eventCount = 0;
        for (auto metadata : readMidi)
        {
            int expectedTimestamp = 10 + eventCount * 5 + eventCount * 50; // 10, 65, 120
            int expectedNote = 60 + eventCount;

            expectEquals (metadata.samplePosition, expectedTimestamp, "MIDI timestamp should be correct");
            expectEquals (metadata.getMessage().getNoteNumber(), expectedNote, "MIDI note should be correct");

            eventCount++;
        }

        expectEquals (eventCount, 3, "Should have 3 MIDI events");
    }
};

static AudioMidiFifoTests audioMidiFifoTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AudioFifoTests : public juce::UnitTest
{
public:
    AudioFifoTests() : juce::UnitTest ("Audio FIFO", "gin_dsp") {}

    void runTest() override
    {
        testBasicWriteRead();
        testFreeSpaceAndReady();
        testCircularBuffer();
        testPeekOperation();
        testWriteSilence();
        testEnsureFreeSpace();
    }

private:
    void testBasicWriteRead()
    {
        beginTest ("Basic Write and Read");

        AudioFifo fifo (2, 1024);

        // Write some data
        juce::AudioBuffer<float> writeBuffer (2, 100);
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                writeBuffer.setSample (ch, i, (float) i * (ch + 1));

        bool writeSuccess = fifo.write (writeBuffer);
        expect (writeSuccess, "Write should succeed");
        expectEquals (fifo.getNumReady(), 100, "Should have 100 samples ready");

        // Read it back
        juce::AudioBuffer<float> readBuffer (2, 100);
        bool readSuccess = fifo.read (readBuffer);
        expect (readSuccess, "Read should succeed");
        expectEquals (fifo.getNumReady(), 0, "Should have 0 samples ready after read");

        // Verify data
        for (int ch = 0; ch < 2; ch++)
        {
            for (int i = 0; i < 100; i++)
            {
                float expected = (float) i * (ch + 1);
                expectWithinAbsoluteError (readBuffer.getSample (ch, i), expected, 0.0001f, "Data should match");
            }
        }
    }

    void testFreeSpaceAndReady()
    {
        beginTest ("Free Space and Ready Count");

        AudioFifo fifo (1, 256);

        // Circular buffer holds capacity - 1 samples
        expectEquals (fifo.getFreeSpace(), 255, "Initial free space should be buffer size - 1");
        expectEquals (fifo.getNumReady(), 0, "Initially should have no data ready");

        juce::AudioBuffer<float> buffer (1, 64);
        fifo.write (buffer);

        expectEquals (fifo.getFreeSpace(), 191, "Free space should decrease");
        expectEquals (fifo.getNumReady(), 64, "Ready count should increase");
    }

    void testCircularBuffer()
    {
        beginTest ("Circular Buffer Behavior");

        AudioFifo fifo (1, 128);

        // Write and read multiple times to wrap around
        juce::AudioBuffer<float> writeBuffer (1, 50);
        juce::AudioBuffer<float> readBuffer (1, 50);

        for (int round = 0; round < 5; round++)
        {
            // Fill with round-specific data
            for (int i = 0; i < 50; i++)
                writeBuffer.setSample (0, i, (float) (round * 100 + i));

            fifo.write (writeBuffer);
            fifo.read (readBuffer);

            // Verify
            for (int i = 0; i < 50; i++)
            {
                float expected = (float) (round * 100 + i);
                expectWithinAbsoluteError (readBuffer.getSample (0, i), expected, 0.0001f, "Circular data should be correct");
            }
        }
    }

    void testPeekOperation()
    {
        beginTest ("Peek Operation");

        AudioFifo fifo (1, 256);

        juce::AudioBuffer<float> writeBuffer (1, 100);
        for (int i = 0; i < 100; i++)
            writeBuffer.setSample (0, i, (float) i);

        fifo.write (writeBuffer);

        // Peek should not consume data
        juce::AudioBuffer<float> peekBuffer (1, 50);
        fifo.peek (peekBuffer);
        expectEquals (fifo.getNumReady(), 100, "Peek should not consume data");

        // Peek individual sample
        float sample = fifo.peekSample (0, 10);
        expectWithinAbsoluteError (sample, 10.0f, 0.0001f, "Peek sample should return correct value");
    }

    void testWriteSilence()
    {
        beginTest ("Write Silence");

        AudioFifo fifo (2, 256);

        fifo.writeSilence (100);
        expectEquals (fifo.getNumReady(), 100, "Silence should be written");

        juce::AudioBuffer<float> readBuffer (2, 100);
        fifo.read (readBuffer);

        // All samples should be zero
        float maxVal = 0.0f;
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < 100; i++)
                maxVal = std::max (maxVal, std::abs (readBuffer.getSample (ch, i)));

        expectWithinAbsoluteError (maxVal, 0.0f, 0.0001f, "Silence should be zeros");
    }

    void testEnsureFreeSpace()
    {
        beginTest ("Ensure Free Space");

        AudioFifo fifo (1, 128);

        // Fill most of the buffer (capacity is 127 due to circular buffer)
        juce::AudioBuffer<float> buffer (1, 100);
        fifo.write (buffer);

        expectEquals (fifo.getFreeSpace(), 27, "Should have 27 free");

        // Ensure we have 50 free - should discard oldest data
        fifo.ensureFreeSpace (50);
        expectEquals (fifo.getFreeSpace(), 50, "Should now have 50 free");
        expectEquals (fifo.getNumReady(), 77, "Should have 77 ready");
    }
};

static AudioFifoTests audioFifoTests;

#endif

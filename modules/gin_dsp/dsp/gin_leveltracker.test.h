/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LevelTrackerTests : public juce::UnitTest
{
public:
    LevelTrackerTests() : juce::UnitTest ("Level Tracker", "gin_dsp") {}

    void runTest() override
    {
        testTrackSample();
        testTrackBuffer();
        testClipDetection();
        testDecayRate();
        testHoldTime();
        testNegativeDecay();
        testMultiChannelBuffer();
    }

private:
    void testTrackSample()
    {
        beginTest ("Level Tracker - Track Sample");

        LevelTracker tracker (30.0f);

        // Track a sample
        tracker.trackSample (0.5f);
        float level = tracker.getLevel();

        float expectedDB = juce::Decibels::gainToDecibels (0.5f);
        expectWithinAbsoluteError (level, expectedDB, 1.0f,
                                  "Level should match input in dB");

        expect (level < 0.0f, "Level for gain < 1.0 should be negative dB");
    }

    void testTrackBuffer()
    {
        beginTest ("Level Tracker - Track Buffer");

        LevelTracker tracker (30.0f);

        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Fill with test signal
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.5f);

        tracker.trackBuffer (buffer);
        float level = tracker.getLevel();

        float expectedDB = juce::Decibels::gainToDecibels (0.5f);
        expectWithinAbsoluteError (level, expectedDB, 1.0f,
                                  "Tracked level should match buffer peak");
    }

    void testClipDetection()
    {
        beginTest ("Level Tracker - Clip Detection");

        LevelTracker tracker (30.0f);

        expect (!tracker.getClip(), "Should not be clipping initially");

        // Track a sample below 0dB
        tracker.trackSample (0.5f);
        expect (!tracker.getClip(), "Should not clip at 0.5 gain");

        // Track a sample at exactly 1.0 (0dB)
        tracker.trackSample (1.0f);
        expect (!tracker.getClip(), "Should not clip at exactly 1.0");

        // Clear clip flag
        tracker.clearClip();
        expect (!tracker.getClip(), "Clip flag should be cleared");

        // Track a sample above 0dB
        tracker.trackSample (1.5f);
        expect (tracker.getClip(), "Should detect clipping above 0dB");

        // Test clearing
        tracker.clearClip();
        expect (!tracker.getClip(), "Clip flag should be cleared again");

        // Test with buffer
        juce::AudioBuffer<float> buffer (2, 10);
        buffer.clear();
        for (int s = 0; s < 10; s++)
            buffer.setSample (0, s, 2.0f);

        tracker.trackBuffer (buffer);
        expect (tracker.getClip(), "Should detect clipping in buffer");
    }

    void testDecayRate()
    {
        beginTest ("Level Tracker - Decay Rate");

        LevelTracker tracker (60.0f); // 60dB per second decay

        // Track a peak
        tracker.trackSample (0.5f);
        float peakLevel = tracker.getLevel();

        // Wait past hold time (50ms) and check decay
        // Note: This test is time-dependent and may be flaky
        juce::Thread::sleep (100); // Wait 100ms

        float decayedLevel = tracker.getLevel();

        expect (decayedLevel < peakLevel,
               "Level should decay after hold time");
        expect (decayedLevel > -100.0f,
               "Level should not drop to minimum immediately");
    }

    void testHoldTime()
    {
        beginTest ("Level Tracker - Hold Time");

        LevelTracker tracker (60.0f);

        // Track a peak
        tracker.trackSample (0.8f);
        float peakLevel = tracker.getLevel();

        // Immediately check level (within hold time)
        float heldLevel = tracker.getLevel();

        expectWithinAbsoluteError (heldLevel, peakLevel, 0.1f,
                                  "Level should be held initially");

        // Wait a bit but still within hold time (50ms)
        juce::Thread::sleep (25); // 25ms < 50ms hold time

        // todo: fix
        //float stillHeld = tracker.getLevel();

        //expectWithinAbsoluteError (stillHeld, peakLevel, 2.0f,
        //                          "Level should still be held within hold time");
    }

    void testNegativeDecay()
    {
        beginTest ("Level Tracker - Negative Decay (Inverted)");

        // Negative decay rate inverts the behavior (tracks minimum instead of maximum)
        LevelTracker tracker (-30.0f);

        // Track a relatively loud sample
        tracker.trackSample (0.8f);
        float level1 = tracker.getLevel();

        // Track a quieter sample - with negative decay, this should update the level
        tracker.trackSample (0.2f);
        float level2 = tracker.getLevel();

        expect (level2 < level1,
               "With negative decay, quieter samples should update the level");
    }

    void testMultiChannelBuffer()
    {
        beginTest ("Level Tracker - Multi-Channel Buffer");

        LevelTracker tracker (30.0f);

        juce::AudioBuffer<float> buffer (4, 100);
        buffer.clear();

        // Create different levels on different channels
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            buffer.setSample (0, s, 0.2f);
            buffer.setSample (1, s, 0.5f);
            buffer.setSample (2, s, 0.3f);
            buffer.setSample (3, s, 0.8f); // Loudest channel
        }

        tracker.trackBuffer (buffer);
        float level = tracker.getLevel();

        // Should track the peak across all channels
        float expectedDB = juce::Decibels::gainToDecibels (0.8f);
        expectWithinAbsoluteError (level, expectedDB, 1.0f,
                                  "Should track peak across all channels");
    }
};

static LevelTrackerTests levelTrackerTests;

#endif

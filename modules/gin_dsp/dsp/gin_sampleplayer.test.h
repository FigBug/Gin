/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class SamplePlayerTests : public juce::UnitTest
{
public:
    SamplePlayerTests() : juce::UnitTest ("Sample Player", "gin_dsp") {}

    void runTest() override
    {
        testDefaultConstruction();
        testSetBuffer();
        testPlayStop();
        testLooping();
        testPositionTracking();
        testProcessBlock();
        testProcessBlockNonLooping();
        testCrossfadeSamples();
        testLengthCalculation();
        testClear();
        testPopulatePositionInfo();
    }

private:
    void testDefaultConstruction()
    {
        beginTest ("Default Construction");

        SamplePlayer player;

        expect (! player.isPlaying(), "Should not be playing initially");
        expect (player.isLooping(), "Should be looping by default");
        expect (! player.hasFileLoaded(), "Should not have file loaded initially");
        expectEquals (player.getPosition(), 0.0, "Position should be 0");
    }

    void testSetBuffer()
    {
        beginTest ("Set Buffer");

        SamplePlayer player;

        juce::AudioBuffer<float> buffer (2, 1000);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 1000; ++i)
                buffer.setSample (ch, i, std::sin (static_cast<float> (i) * 0.1f));

        player.setBuffer (buffer, 44100.0);
        player.setPlaybackSampleRate (44100.0);

        expectEquals (player.getSourceSampleRate(), 44100.0, "Source sample rate should match");
        expectEquals (player.getBuffer().getNumSamples(), 1000, "Buffer should have 1000 samples");
        expectEquals (player.getBuffer().getNumChannels(), 2, "Buffer should have 2 channels");
    }

    void testPlayStop()
    {
        beginTest ("Play and Stop");

        SamplePlayer player;

        juce::AudioBuffer<float> buffer (1, 1000);
        buffer.clear();
        player.setBuffer (buffer, 44100.0);

        expect (! player.isPlaying(), "Should not be playing initially");

        player.play();
        expect (player.isPlaying(), "Should be playing after play()");

        player.stop();
        expect (! player.isPlaying(), "Should not be playing after stop()");
    }

    void testLooping()
    {
        beginTest ("Looping");

        SamplePlayer player;

        expect (player.isLooping(), "Should be looping by default");

        player.setLooping (false);
        expect (! player.isLooping(), "Should not be looping after setLooping(false)");

        player.setLooping (true);
        expect (player.isLooping(), "Should be looping after setLooping(true)");
    }

    void testPositionTracking()
    {
        beginTest ("Position Tracking");

        SamplePlayer player;

        juce::AudioBuffer<float> buffer (1, 44100);  // 1 second at 44100 Hz
        buffer.clear();
        player.setBuffer (buffer, 44100.0);
        player.setPlaybackSampleRate (44100.0);

        expectEquals (player.getPosition(), 0.0, "Initial position should be 0");
        expectWithinAbsoluteError (player.getPositionInSeconds(), 0.0, 0.001, "Initial position in seconds should be 0");

        player.setPosition (22050.0);
        expectWithinAbsoluteError (player.getPosition(), 22050.0, 0.001, "Position should be 22050");
        expectWithinAbsoluteError (player.getPositionInSeconds(), 0.5, 0.001, "Position should be 0.5 seconds");

        player.reset();
        expectEquals (player.getPosition(), 0.0, "Position should be 0 after reset");
    }

    void testProcessBlock()
    {
        beginTest ("Process Block");

        SamplePlayer player;

        // Create a simple sine wave buffer
        const int bufferSize = 1000;
        juce::AudioBuffer<float> sourceBuffer (1, bufferSize);
        for (int i = 0; i < bufferSize; ++i)
            sourceBuffer.setSample (0, i, std::sin (static_cast<float> (i) * 0.1f));

        player.setBuffer (sourceBuffer, 44100.0);
        player.setPlaybackSampleRate (44100.0);
        player.play();

        // Process a block
        juce::AudioBuffer<float> outputBuffer (1, 100);
        outputBuffer.clear();
        player.processBlock (outputBuffer);

        // Check that output is non-zero (audio was produced)
        float maxVal = 0.0f;
        for (int i = 0; i < outputBuffer.getNumSamples(); ++i)
            maxVal = std::max (maxVal, std::abs (outputBuffer.getSample (0, i)));

        expect (maxVal > 0.0f, "Output should contain audio data");

        // Check position advanced
        expect (player.getPosition() > 0.0, "Position should have advanced");
    }

    void testProcessBlockNonLooping()
    {
        beginTest ("Process Block Non-Looping Stop");

        SamplePlayer player;

        // Create a short buffer
        const int bufferSize = 100;
        juce::AudioBuffer<float> sourceBuffer (1, bufferSize);
        for (int i = 0; i < bufferSize; ++i)
            sourceBuffer.setSample (0, i, 1.0f);

        player.setBuffer (sourceBuffer, 44100.0);
        player.setPlaybackSampleRate (44100.0);
        player.setLooping (false);
        player.play();

        // Process a block larger than the source buffer
        juce::AudioBuffer<float> outputBuffer (1, 200);
        outputBuffer.clear();
        player.processBlock (outputBuffer);

        // Player should have stopped after reaching the end
        expect (! player.isPlaying(), "Player should stop when reaching end with looping disabled");
    }

    void testCrossfadeSamples()
    {
        beginTest ("Crossfade Samples");

        SamplePlayer player;

        player.setCrossfadeSamples (500);
        // We can't directly check the value, but we verify it doesn't crash

        player.setCrossfadeSamples (0);
        // Test with zero crossfade

        player.setCrossfadeSamples (-100);
        // Negative should be clamped to 0
    }

    void testLengthCalculation()
    {
        beginTest ("Length Calculation");

        SamplePlayer player;

        // 44100 samples at 44100 Hz = 1 second
        juce::AudioBuffer<float> buffer (1, 44100);
        buffer.clear();
        player.setBuffer (buffer, 44100.0);

        expectWithinAbsoluteError (player.getLengthInSeconds(), 1.0, 0.001, "Length should be 1 second");

        // 88200 samples at 44100 Hz = 2 seconds
        juce::AudioBuffer<float> buffer2 (1, 88200);
        buffer2.clear();
        player.setBuffer (buffer2, 44100.0);

        expectWithinAbsoluteError (player.getLengthInSeconds(), 2.0, 0.001, "Length should be 2 seconds");
    }

    void testClear()
    {
        beginTest ("Clear");

        SamplePlayer player;

        juce::AudioBuffer<float> buffer (1, 1000);
        buffer.clear();
        player.setBuffer (buffer, 44100.0);
        player.play();

        expect (player.isPlaying(), "Should be playing");
        expectEquals (player.getBuffer().getNumSamples(), 1000, "Should have samples");

        player.clear();

        expect (! player.isPlaying(), "Should not be playing after clear");
        expect (! player.hasFileLoaded(), "Should not have file loaded after clear");
        expectEquals (player.getBuffer().getNumSamples(), 0, "Buffer should be empty after clear");
    }

    void testPopulatePositionInfo()
    {
        beginTest ("Populate Position Info");

        SamplePlayer player;

        // Create a 2 second buffer
        juce::AudioBuffer<float> buffer (1, 88200);
        buffer.clear();
        player.setBuffer (buffer, 44100.0);
        player.setPlaybackSampleRate (44100.0);
        player.setLooping (true);
        player.play();

        // Advance position to 1 second
        player.setPosition (44100.0);

        auto info = player.populatePositionInfo();

        expect (info.getIsPlaying(), "Should report playing");
        expect (info.getIsLooping(), "Should report looping");
        expect (! info.getIsRecording(), "Should not report recording");

        auto timeInSeconds = info.getTimeInSeconds();
        expect (timeInSeconds.hasValue(), "Should have time in seconds");
        if (timeInSeconds.hasValue())
            expectWithinAbsoluteError (*timeInSeconds, 1.0, 0.01, "Time should be ~1 second");

        auto bpm = info.getBpm();
        expect (bpm.hasValue(), "Should have BPM");
    }
};

static SamplePlayerTests samplePlayerTests;

#endif

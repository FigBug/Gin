/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DelayLineTests : public juce::UnitTest
{
public:
    DelayLineTests() : juce::UnitTest ("Delay Line", "gin_dsp") {}

    void runTest() override
    {
        testBasicWriteRead();
        testLinearInterpolation();
        testClear();
        testMultiChannel();
    }

private:
    void testBasicWriteRead()
    {
        beginTest ("Basic Write and Read");

        DelayLine delay (1, 1.0, 44100.0); // 1 channel, 1 second max

        // Write some samples
        for (int i = 0; i < 100; i++)
        {
            delay.write (0, (float) i);
            delay.writeFinished();
        }

        // Read back with 10 sample delay (convert to seconds: 10/44100)
        float sample = delay.readLinear (0, 10.0 / 44100.0);

        // Should be sample 89 (100 - 10 - 1 because of how delay works)
        expectWithinAbsoluteError (sample, 89.0f, 1.0f, "Should read delayed sample");
    }

    void testLinearInterpolation()
    {
        beginTest ("Linear Interpolation");

        DelayLine delay (1, 0.1, 44100.0);

        // Write known pattern
        for (int i = 0; i < 100; i++)
        {
            delay.write (0, (float) i);
            delay.writeFinished();
        }

        // Read with fractional delay
        double delay10 = 10.0 / 44100.0;
        double delay11 = 11.0 / 44100.0;
        double halfWay = (delay10 + delay11) / 2.0;

        float sample10 = delay.readLinear (0, delay10);
        float sample11 = delay.readLinear (0, delay11);
        float sampleHalf = delay.readLinear (0, halfWay);

        // Interpolated value should be between the two integer delays
        float expectedMid = (sample10 + sample11) / 2.0f;
        expectWithinAbsoluteError (sampleHalf, expectedMid, 0.5f, "Interpolation should give intermediate value");
    }

    void testClear()
    {
        beginTest ("Clear");

        DelayLine delay (1, 0.1, 44100.0);

        // Write some data
        for (int i = 0; i < 50; i++)
        {
            delay.write (0, 1.0f);
            delay.writeFinished();
        }

        delay.clear();

        // After clear, write one sample and read - should be zeros except for newest
        delay.write (0, 5.0f);
        delay.writeFinished();

        // Reading a delay should give 0 (buffer was cleared)
        float sample = delay.readLinear (0, 5.0 / 44100.0);
        expectWithinAbsoluteError (sample, 0.0f, 0.1f, "Cleared buffer should read zeros");
    }

    void testMultiChannel()
    {
        beginTest ("Multi-Channel");

        DelayLine delay (2, 0.1, 44100.0);

        // Write different data to each channel
        for (int i = 0; i < 50; i++)
        {
            delay.write (0, 1.0f);
            delay.write (1, 2.0f);
            delay.writeFinished();
        }

        float ch0 = delay.readLinear (0, 10.0 / 44100.0);
        float ch1 = delay.readLinear (1, 10.0 / 44100.0);

        expectWithinAbsoluteError (ch0, 1.0f, 0.1f, "Channel 0 should have value 1");
        expectWithinAbsoluteError (ch1, 2.0f, 0.1f, "Channel 1 should have value 2");
    }
};

static DelayLineTests delayLineTests;

#endif

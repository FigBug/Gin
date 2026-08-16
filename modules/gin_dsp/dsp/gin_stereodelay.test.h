/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class StereoDelayTests : public juce::UnitTest
{
public:
    StereoDelayTests() : juce::UnitTest ("StereoDelay", "gin_dsp") {}

    void runTest() override
    {
        testDryPassThrough();
        testWetDelay();
    }

private:
    void testDryPassThrough()
    {
        beginTest ("Dry Pass Through");

        StereoDelay delay (1.0);
        delay.setSampleRate (1000.0);
        delay.setParams (0.1f, 0.0f, 0.0f, 0.0f); // fully dry
        delay.reset();

        juce::AudioSampleBuffer buffer (2, 64);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            buffer.setSample (0, i, std::sin (i * 0.1f));
            buffer.setSample (1, i, std::cos (i * 0.1f));
        }

        juce::AudioSampleBuffer expected;
        expected.makeCopyOf (buffer);

        delay.process (buffer);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i), 1.0e-5f, "Left dry");
            expectWithinAbsoluteError (buffer.getSample (1, i), expected.getSample (1, i), 1.0e-5f, "Right dry");
        }
    }

    void testWetDelay()
    {
        beginTest ("Wet Signal Is Delayed");

        const double sr = 1000.0;
        const float delayTime = 0.1f; // 100 samples at 1 kHz

        StereoDelay delay (1.0);
        delay.setSampleRate (sr);
        delay.setParams (delayTime, 1.0f, 0.0f, 0.0f); // fully wet, no feedback
        delay.reset();

        juce::AudioSampleBuffer buffer (2, 200);
        buffer.clear();
        buffer.setSample (0, 0, 1.0f); // impulse
        buffer.setSample (1, 0, 1.0f);

        delay.process (buffer);

        // Nothing should arrive before the delay time
        for (int i = 0; i < 50; i++)
        {
            expect (std::abs (buffer.getSample (0, i)) < 0.01f, "No early left output");
            expect (std::abs (buffer.getSample (1, i)) < 0.01f, "No early right output");
        }

        // The impulse should reappear around sample 100
        float peak = 0.0f;
        for (int i = 95; i <= 105; i++)
            peak = std::max (peak, std::abs (buffer.getSample (0, i)));

        expect (peak > 0.9f, "Delayed impulse should appear near the delay time");
    }
};

static StereoDelayTests stereoDelayTests;

#endif

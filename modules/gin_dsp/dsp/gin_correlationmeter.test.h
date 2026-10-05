/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class CorrelationMeterTests : public juce::UnitTest
{
public:
    CorrelationMeterTests() : juce::UnitTest ("Correlation Meter", "gin_dsp") {}

    void runTest() override
    {
        testMono();
        testInverted();
        testUncorrelated();
        testBalance();
        testSilenceAndReset();
    }

private:
    static juce::AudioBuffer<float> makeSine (int numSamples, float leftGain, float rightGain, int64_t start = 0)
    {
        juce::AudioBuffer<float> buffer (2, numSamples);
        for (int i = 0; i < numSamples; i++)
        {
            const float v = float (std::sin (2.0 * juce::MathConstants<double>::pi * 440.0 * double (start + i) / 48000.0));
            buffer.setSample (0, i, v * leftGain);
            buffer.setSample (1, i, v * rightGain);
        }
        return buffer;
    }

    static void run (CorrelationMeter& meter, float leftGain, float rightGain, int blocks = 100)
    {
        // Start each case from a clean average so the previous one doesn't bleed in
        meter.reset();

        int64_t pos = 0;
        for (int b = 0; b < blocks; b++)
        {
            meter.process (makeSine (480, leftGain, rightGain, pos));
            pos += 480;
        }
    }

    void testMono()
    {
        beginTest ("Correlation Meter - identical channels read +1");

        CorrelationMeter meter;
        meter.prepare (48000.0, 0.3);
        run (meter, 0.5f, 0.5f);

        expectWithinAbsoluteError (meter.getCorrelation(), 1.0f, 0.01f, "correlation");
        expectWithinAbsoluteError (meter.getBalance(), 0.0f, 0.01f, "balance");

        // A mono buffer is treated as fully correlated too
        juce::AudioBuffer<float> mono (1, 480);
        for (int i = 0; i < 480; i++) mono.setSample (0, i, 0.3f);
        for (int i = 0; i < 100; i++) meter.process (mono);
        expectWithinAbsoluteError (meter.getCorrelation(), 1.0f, 0.01f, "mono buffer");
    }

    void testInverted()
    {
        beginTest ("Correlation Meter - inverted channels read -1");

        CorrelationMeter meter;
        meter.prepare (48000.0, 0.3);
        run (meter, 0.5f, -0.5f);

        expectWithinAbsoluteError (meter.getCorrelation(), -1.0f, 0.01f, "correlation");
    }

    void testUncorrelated()
    {
        beginTest ("Correlation Meter - independent noise reads near 0");

        CorrelationMeter meter;
        meter.prepare (48000.0, 1.0);

        juce::Random rng (1234);
        juce::AudioBuffer<float> buffer (2, 480);
        for (int b = 0; b < 1000; b++)
        {
            for (int i = 0; i < 480; i++)
            {
                buffer.setSample (0, i, rng.nextFloat() * 2.0f - 1.0f);
                buffer.setSample (1, i, rng.nextFloat() * 2.0f - 1.0f);
            }
            meter.process (buffer);
        }

        expect (std::abs (meter.getCorrelation()) < 0.1f, "near zero: " + juce::String (meter.getCorrelation()));
    }

    void testBalance()
    {
        beginTest ("Correlation Meter - balance");

        CorrelationMeter meter;
        meter.prepare (48000.0, 0.3);

        run (meter, 0.5f, 0.0f);
        expectWithinAbsoluteError (meter.getBalance(), -1.0f, 0.01f, "left only");

        run (meter, 0.0f, 0.5f);
        expectWithinAbsoluteError (meter.getBalance(), 1.0f, 0.01f, "right only");

        run (meter, 0.5f, 0.25f);
        expectWithinAbsoluteError (meter.getBalance(), -1.0f / 3.0f, 0.02f, "right at half the level of left");
    }

    void testSilenceAndReset()
    {
        beginTest ("Correlation Meter - silence and reset");

        CorrelationMeter meter;
        meter.prepare (48000.0, 0.3);

        juce::AudioBuffer<float> silence (2, 480);
        silence.clear();
        meter.process (silence);
        expectEquals (meter.getCorrelation(), 0.0f, "silent reads 0");

        run (meter, 0.5f, -0.5f);
        meter.reset();
        meter.process (silence);
        expectEquals (meter.getCorrelation(), 0.0f, "reset clears");
    }
};

static CorrelationMeterTests correlationMeterTests;

#endif

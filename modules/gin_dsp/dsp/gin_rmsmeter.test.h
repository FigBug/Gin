/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class RMSMeterTests : public juce::UnitTest
{
public:
    RMSMeterTests() : juce::UnitTest ("RMS Meter", "gin_dsp") {}

    void runTest() override
    {
        testFullScaleSine();
        testDC();
        testWindowTracksChanges();
        testReset();
    }

private:
    static void runSine (RMSMeter& meter, double amplitude, int blocks, int64_t& pos)
    {
        juce::AudioBuffer<float> buffer (2, 480);
        for (int b = 0; b < blocks; b++)
        {
            for (int i = 0; i < 480; i++)
            {
                const float v = float (amplitude * std::sin (2.0 * juce::MathConstants<double>::pi * 1000.0 * double (pos + i) / 48000.0));
                buffer.setSample (0, i, v);
                buffer.setSample (1, i, v * 0.5f);
            }
            pos += 480;
            meter.process (buffer);
        }
    }

    void testFullScaleSine()
    {
        beginTest ("RMS Meter - full scale sine reads -3.01 dBFS");

        RMSMeter meter;
        meter.prepare (48000.0, 2, 0.3);

        int64_t pos = 0;
        runSine (meter, 1.0, 100, pos);

        expectWithinAbsoluteError (meter.getRMS (0), -3.01f, 0.05f, "left");
        expectWithinAbsoluteError (meter.getRMS (1), -9.03f, 0.05f, "right at half amplitude");
        expectWithinAbsoluteError (meter.getRMSLinear (0), 0.7071f, 0.005f, "linear");
    }

    void testDC()
    {
        beginTest ("RMS Meter - DC");

        RMSMeter meter;
        meter.prepare (44100.0, 1, 0.1);

        juce::AudioBuffer<float> buffer (1, 441);
        for (int i = 0; i < 441; i++)
            buffer.setSample (0, i, 0.5f);

        for (int b = 0; b < 20; b++)
            meter.process (buffer);

        expectWithinAbsoluteError (meter.getRMS (0), -6.02f, 0.05f, "0.5 DC");
    }

    void testWindowTracksChanges()
    {
        beginTest ("RMS Meter - window follows level changes");

        RMSMeter meter;
        meter.prepare (48000.0, 2, 0.3);

        int64_t pos = 0;
        runSine (meter, 1.0, 100, pos);
        runSine (meter, 0.1, 100, pos);   // a full second, well past the 300 ms window

        expectWithinAbsoluteError (meter.getRMS (0), -23.01f, 0.1f, "settled to the new level");
        expectEquals (meter.getRMS (5), RMSMeter::silence, "out of range channel");
    }

    void testReset()
    {
        beginTest ("RMS Meter - reset");

        RMSMeter meter;
        meter.prepare (48000.0, 2, 0.3);

        int64_t pos = 0;
        runSine (meter, 1.0, 100, pos);
        meter.reset();

        juce::AudioBuffer<float> silence (2, 480);
        silence.clear();
        meter.process (silence);

        expectEquals (meter.getRMSLinear (0), 0.0f, "cleared");
    }
};

static RMSMeterTests rmsMeterTests;

#endif

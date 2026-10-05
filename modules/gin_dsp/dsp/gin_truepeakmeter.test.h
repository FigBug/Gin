/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class TruePeakMeterTests : public juce::UnitTest
{
public:
    TruePeakMeterTests() : juce::UnitTest ("True Peak Meter", "gin_dsp") {}

    void runTest() override
    {
        testIntersamplePeak();
        testPlainSine();
        testHoldAndReset();
        testOversamplingFactor();
    }

private:
    void testIntersamplePeak()
    {
        beginTest ("True Peak Meter - finds a peak that falls between samples");

        // A sine at fs/4 sampled at 45 degrees never hits its peak: every
        // sample is +/-0.707, but the waveform reaches 1.0 in between.
        const double sampleRate = 48000.0;
        TruePeakMeter meter;
        meter.prepare (sampleRate, 1, 512);

        juce::AudioBuffer<float> buffer (1, 512);
        for (int block = 0; block < 20; block++)
        {
            for (int i = 0; i < 512; i++)
            {
                const double n = double (block * 512 + i);
                buffer.setSample (0, i, float (std::sin (juce::MathConstants<double>::pi * 0.5 * n + juce::MathConstants<double>::pi * 0.25)));
            }
            meter.process (buffer);
        }

        const float samplePeak = juce::Decibels::gainToDecibels (buffer.getMagnitude (0, 512));
        expectWithinAbsoluteError (samplePeak, -3.01f, 0.05f, "sample peak is -3 dBFS");
        expectWithinAbsoluteError (meter.getMaxTruePeak (0), 0.0f, 0.3f, "true peak is 0 dBTP");
    }

    void testPlainSine()
    {
        beginTest ("True Peak Meter - -6 dBFS sine reads -6 dBTP");

        TruePeakMeter meter;
        meter.prepare (44100.0, 2, 256);

        juce::AudioBuffer<float> buffer (2, 256);
        for (int block = 0; block < 40; block++)
        {
            for (int i = 0; i < 256; i++)
            {
                const double n = double (block * 256 + i);
                const float v = 0.5f * float (std::sin (2.0 * juce::MathConstants<double>::pi * 1000.0 * n / 44100.0));
                buffer.setSample (0, i, v);
                buffer.setSample (1, i, v * 0.5f);
            }
            meter.process (buffer);
        }

        expectWithinAbsoluteError (meter.getMaxTruePeak (0), -6.02f, 0.2f, "left");
        expectWithinAbsoluteError (meter.getMaxTruePeak (1), -12.04f, 0.2f, "right");
        expectWithinAbsoluteError (meter.getMaxTruePeak(), -6.02f, 0.2f, "overall");
    }

    void testHoldAndReset()
    {
        beginTest ("True Peak Meter - max holds until reset");

        TruePeakMeter meter;
        meter.prepare (48000.0, 1, 128);

        juce::AudioBuffer<float> loud (1, 128), quiet (1, 128);
        loud.clear();  loud.setSample (0, 64, 0.5f);
        quiet.clear(); quiet.setSample (0, 64, 0.1f);

        for (int i = 0; i < 10; i++) meter.process (loud);
        for (int i = 0; i < 10; i++) meter.process (quiet);

        expect (meter.getMaxTruePeak (0) > -7.0f, "max still holds the loud peak");
        expect (meter.getTruePeak (0) < -15.0f, "block peak reflects the quiet block");

        meter.reset();
        for (int i = 0; i < 10; i++) meter.process (quiet);
        expect (meter.getMaxTruePeak (0) < -15.0f, "max cleared by reset");
    }

    void testOversamplingFactor()
    {
        beginTest ("True Peak Meter - oversampling factor by sample rate");

        TruePeakMeter meter;
        meter.prepare (44100.0, 1, 64);
        expectEquals (meter.getOversamplingFactor(), 4, "4x at 44.1k");
        meter.prepare (96000.0, 1, 64);
        expectEquals (meter.getOversamplingFactor(), 2, "2x at 96k");
        meter.prepare (192000.0, 1, 64);
        expectEquals (meter.getOversamplingFactor(), 1, "1x at 192k");
    }
};

static TruePeakMeterTests truePeakMeterTests;

#endif

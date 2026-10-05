/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LoudnessMeterTests : public juce::UnitTest
{
public:
    LoudnessMeterTests() : juce::UnitTest ("Loudness Meter", "gin_dsp") {}

    void runTest() override
    {
        testKWeightingCoefficients();
        testStereoSineReference();
        testMonoFullScaleSine();
        testSilence();
        testRelativeGate();
        testLoudnessRange();
        testReset();
    }

private:
    static constexpr double sampleRate = 48000.0;

    // Fills a stereo buffer with a sine at the given peak amplitude on the chosen channels
    static void fillSine (juce::AudioBuffer<float>& buffer, double freq, double amplitude, int64_t startSample, bool left, bool right)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            const float v = float (amplitude * std::sin (2.0 * juce::MathConstants<double>::pi * freq * double (startSample + i) / sampleRate));
            buffer.setSample (0, i, left  ? v : 0.0f);
            buffer.setSample (1, i, right ? v : 0.0f);
        }
    }

    static void runSine (LoudnessMeter& meter, double seconds, double dbfs, bool left = true, bool right = true, int64_t* position = nullptr)
    {
        juce::AudioBuffer<float> buffer (2, 512);
        const double amp = juce::Decibels::decibelsToGain (dbfs);
        int64_t pos = position != nullptr ? *position : 0;
        const int64_t end = pos + int64_t (seconds * sampleRate);

        while (pos < end)
        {
            fillSine (buffer, 1000.0, amp, pos, left, right);
            meter.process (buffer);
            pos += buffer.getNumSamples();
        }

        if (position != nullptr)
            *position = pos;
    }

    void testKWeightingCoefficients()
    {
        beginTest ("Loudness Meter - K-weighting matches BS.1770 at 48 kHz");

        const auto s1 = KWeightingFilter::stage1Coefficients (48000.0);
        expectWithinAbsoluteError (s1.b0,  1.53512485958697, 1.0e-6, "stage 1 b0");
        expectWithinAbsoluteError (s1.b1, -2.69169618940638, 1.0e-6, "stage 1 b1");
        expectWithinAbsoluteError (s1.b2,  1.19839281085285, 1.0e-6, "stage 1 b2");
        expectWithinAbsoluteError (s1.a1, -1.69065929318241, 1.0e-6, "stage 1 a1");
        expectWithinAbsoluteError (s1.a2,  0.73248077421585, 1.0e-6, "stage 1 a2");

        const auto s2 = KWeightingFilter::stage2Coefficients (48000.0);
        expectWithinAbsoluteError (s2.b0,  1.0, 1.0e-9, "stage 2 b0");
        expectWithinAbsoluteError (s2.b1, -2.0, 1.0e-9, "stage 2 b1");
        expectWithinAbsoluteError (s2.b2,  1.0, 1.0e-9, "stage 2 b2");
        expectWithinAbsoluteError (s2.a1, -1.99004745483398, 1.0e-6, "stage 2 a1");
        expectWithinAbsoluteError (s2.a2,  0.99007225036621, 1.0e-6, "stage 2 a2");
    }

    void testStereoSineReference()
    {
        beginTest ("Loudness Meter - EBU Tech 3341 case 1: stereo -23 dBFS sine reads -23 LUFS");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);
        runSine (meter, 10.0, -23.0);

        expectWithinAbsoluteError (meter.getMomentary(),  -23.0f, 0.1f, "momentary");
        expectWithinAbsoluteError (meter.getShortTerm(),  -23.0f, 0.1f, "short-term");
        expectWithinAbsoluteError (meter.getIntegrated(), -23.0f, 0.1f, "integrated");
        expectWithinAbsoluteError (meter.getMaxMomentary(), -23.0f, 0.1f, "max momentary");
        expectWithinAbsoluteError (float (meter.getElapsedSeconds()), 10.0f, 0.2f, "elapsed");
    }

    void testMonoFullScaleSine()
    {
        beginTest ("Loudness Meter - full scale sine in one channel reads -3.01 LUFS");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);
        runSine (meter, 5.0, 0.0, true, false);

        expectWithinAbsoluteError (meter.getIntegrated(), -3.01f, 0.1f, "integrated");
    }

    void testSilence()
    {
        beginTest ("Loudness Meter - silence");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);

        expectEquals (meter.getMomentary(), LoudnessMeter::silence, "momentary before audio");
        expectEquals (meter.getIntegrated(), LoudnessMeter::silence, "integrated before audio");

        juce::AudioBuffer<float> buffer (2, 4800);
        buffer.clear();
        for (int i = 0; i < 20; i++)
            meter.process (buffer);

        expectEquals (meter.getMomentary(), LoudnessMeter::silence, "momentary on silence");
        expectEquals (meter.getIntegrated(), LoudnessMeter::silence, "integrated on silence");
        expectEquals (meter.getLoudnessRange(), 0.0f, "range on silence");
    }

    void testRelativeGate()
    {
        beginTest ("Loudness Meter - relative gate excludes quiet passages from integrated");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);

        int64_t pos = 0;
        runSine (meter, 10.0, -23.0, true, true, &pos);
        runSine (meter, 10.0, -60.0, true, true, &pos);

        // Ungated mean would be about -26; the -60 section sits more than 10 LU
        // below that so it must be gated out, leaving -23.
        expectWithinAbsoluteError (meter.getIntegrated(), -23.0f, 0.2f, "integrated");
    }

    void testLoudnessRange()
    {
        beginTest ("Loudness Meter - EBU Tech 3342: -20 then -30 dBFS gives LRA of 10 LU");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);

        int64_t pos = 0;
        runSine (meter, 20.0, -20.0, true, true, &pos);
        runSine (meter, 20.0, -30.0, true, true, &pos);

        expectWithinAbsoluteError (meter.getLoudnessRange(), 10.0f, 1.0f, "range");
    }

    void testReset()
    {
        beginTest ("Loudness Meter - reset clears history");

        LoudnessMeter meter;
        meter.prepare (sampleRate, 2);
        runSine (meter, 3.0, -10.0);
        expectWithinAbsoluteError (meter.getIntegrated(), -10.0f, 0.2f, "before reset");

        meter.reset();
        runSine (meter, 3.0, -30.0);
        expectWithinAbsoluteError (meter.getIntegrated(), -30.0f, 0.2f, "after reset only sees new audio");
        expectWithinAbsoluteError (meter.getMaxMomentary(), -30.0f, 0.2f, "max momentary cleared");
    }
};

static LoudnessMeterTests loudnessMeterTests;

#endif

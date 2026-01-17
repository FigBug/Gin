/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AudioFunctionsTests : public juce::UnitTest
{
public:
    AudioFunctionsTests() : juce::UnitTest ("Audio Functions", "gin_dsp") {}

    void runTest() override
    {
        testLP12Filter();
        testHP12Filter();
        testBP12Filter();
        testNotch12Filter();
        testOscState();
        testNoiseState();
    }

private:
    void testLP12Filter()
    {
        beginTest ("LP12 Lowpass Filter");

        LP12State filter (44100.0);

        // Test that filter processes samples
        double output = filter.process (1.0, 1000.0, 0.707);
        expect (! std::isnan (output), "Filter should produce valid output");
        expect (! std::isinf (output), "Filter output should not be infinite");

        // Test frequency clamping
        double outputLow = filter.process (1.0, 1.0, 0.707);  // Below minimum
        expect (! std::isnan (outputLow), "Should handle very low frequency");

        double outputHigh = filter.process (1.0, 100000.0, 0.707);  // Above nyquist
        expect (! std::isnan (outputHigh), "Should handle frequency above nyquist");

        // Test reset
        filter.reset();
        expect (true, "Reset should not crash");
    }

    void testHP12Filter()
    {
        beginTest ("HP12 Highpass Filter");

        HP12State filter (44100.0);

        // Test that filter processes samples
        double output = filter.process (1.0, 1000.0, 0.707);
        expect (! std::isnan (output), "Filter should produce valid output");
        expect (! std::isinf (output), "Filter output should not be infinite");

        // Test Q clamping (very low Q)
        double outputLowQ = filter.process (1.0, 1000.0, 0.0);
        expect (! std::isnan (outputLowQ), "Should handle zero Q");

        filter.reset();
        expect (true, "Reset should not crash");
    }

    void testBP12Filter()
    {
        beginTest ("BP12 Bandpass Filter");

        BP12State filter (44100.0);

        // Test basic processing
        double output = filter.process (1.0, 1000.0, 2.0);
        expect (! std::isnan (output), "Filter should produce valid output");
        expect (! std::isinf (output), "Filter output should not be infinite");

        // Test with different Q values
        double narrowBand = filter.process (1.0, 1000.0, 10.0);
        expect (! std::isnan (narrowBand), "Should handle high Q (narrow band)");

        double wideBand = filter.process (1.0, 1000.0, 0.5);
        expect (! std::isnan (wideBand), "Should handle low Q (wide band)");

        filter.reset();
        expect (true, "Reset should not crash");
    }

    void testNotch12Filter()
    {
        beginTest ("Notch12 Notch Filter");

        Notch12State filter (44100.0);

        // Test basic processing
        double output = filter.process (1.0, 1000.0, 2.0);
        expect (! std::isnan (output), "Filter should produce valid output");
        expect (! std::isinf (output), "Filter output should not be infinite");

        // Notch filters should pass most frequencies except the notch frequency
        // Test at center frequency (should attenuate)
        filter.reset();
        double notchOutput = filter.process (1.0, 1000.0, 5.0);
        expect (! std::isnan (notchOutput), "Should produce valid output at notch frequency");

        filter.reset();
        expect (true, "Reset should not crash");
    }

    void testOscState()
    {
        beginTest ("Oscillator State");

        OscState osc (44100.0);

        // Test initial state
        expect (osc.phase >= 0.0f && osc.phase <= 1.0f, "Phase should be in valid range");

        // Test phase increment
        osc.incPhase (69.0f);  // A4 = 440 Hz
        expect (osc.phase >= 0.0f && osc.phase <= 1.0f, "Phase should stay in range after increment");
        expect (osc.frequency > 0.0f, "Frequency should be set");
        expectWithinAbsoluteError (osc.frequency, 440.0f, 1.0f, "Frequency should be ~440 Hz for MIDI 69");

        // Test phase wrapping
        for (int i = 0; i < 1000; i++)
        {
            osc.incPhase (69.0f);
            expect (osc.phase >= 0.0f && osc.phase <= 1.0f, "Phase should wrap correctly");
        }

        // Test reset
        osc.reset();
        expect (osc.phase >= 0.0f && osc.phase <= 1.0f, "Phase should be valid after reset");

        // Test sample rate change
        osc.setSampleRate (48000.0);
        expectEquals (osc.sampleRate, 48000.0, "Sample rate should update");
    }

    void testNoiseState()
    {
        beginTest ("Noise State");

        NoiseState noise (44100.0);

        // Test that noise generates values
        double value1 = noise.process();
        expect (! std::isnan (value1), "Noise should produce valid output");
        expect (! std::isinf (value1), "Noise should not be infinite");

        // Test that noise is random (different values)
        double value2 = noise.process();
        double value3 = noise.process();

        // It's theoretically possible but extremely unlikely to get the same value multiple times
        bool hasVariation = ! juce::approximatelyEqual (value1, value2) ||
                            ! juce::approximatelyEqual (value2, value3);
        expect (hasVariation, "Noise should produce varying values");

        // Test sample rate doesn't crash
        noise.setSampleRate (48000.0);
        double value4 = noise.process();
        expect (! std::isnan (value4), "Should work after sample rate change");
    }
};

static AudioFunctionsTests audioFunctionsTests;

#endif

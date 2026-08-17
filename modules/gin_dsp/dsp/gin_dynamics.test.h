/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DynamicsTests : public juce::UnitTest
{
public:
    DynamicsTests() : juce::UnitTest ("Dynamics", "gin_dsp") {}

    void runTest() override
    {
        testEnvelopeDetectorPeak();
        testEnvelopeDetectorModes();
        testEnvelopeDetectorAttackRelease();
        testEnvelopeDetectorHold();
        testEnvelopeDetectorLogMode();
        testDynamicsCompressor();
        testDynamicsLimiter();
        testDynamicsExpander();
        testDynamicsGate();
        testDynamicsStereoLinking();
        testRMSWindowAverages();
        testLookaheadDelaysSignal();
        testLookaheadCatchesTransient();
        testSidechainFilterIgnoresLowEnd();
        testDetectorPeakFocusesBand();
    }

private:
    static juce::AudioSampleBuffer makeSine (int numChannels, int numSamples, float freq, float amplitude, double sr = 44100.0)
    {
        juce::AudioSampleBuffer b (numChannels, numSamples);

        for (int ch = 0; ch < numChannels; ch++)
            for (int i = 0; i < numSamples; i++)
                b.setSample (ch, i, amplitude * std::sin (juce::MathConstants<float>::twoPi * freq * float (i) / float (sr)));

        return b;
    }

    /** Gain reduction the compressor settles at, as a linear ratio. */
    static float reductionFor (Dynamics& comp, const juce::AudioSampleBuffer& input)
    {
        auto buffer = input;
        comp.process (buffer);

        auto from = buffer.getNumSamples() / 2;
        auto out = buffer.getMagnitude (0, from, buffer.getNumSamples() - from);
        auto in = input.getMagnitude (0, from, input.getNumSamples() - from);

        return in > 0.0f ? out / in : 1.0f;
    }

    void testRMSWindowAverages()
    {
        beginTest ("Envelope Detector - RMS Window");

        // A sine at amplitude 1 has an RMS of 0.707. With a window long enough
        // to cover several cycles the detector should land there rather than
        // following the waveform.
        EnvelopeDetector env;
        env.setSampleRate (44100.0);
        env.setRMSWindow (0.050f);
        env.setParams (0.001f, 0.0f, 0.001f, false, EnvelopeDetector::rms, false);
        env.reset();

        float result = 0.0f;
        for (int i = 0; i < 44100; i++)
            result = env.process (std::sin (juce::MathConstants<float>::twoPi * 500.0f * float (i) / 44100.0f));

        expectWithinAbsoluteError (result, 0.707f, 0.05f, "RMS of a unit sine should be about 0.707");

        // A short window follows the waveform much more closely, so the
        // detector swings rather than sitting still
        EnvelopeDetector shortEnv;
        shortEnv.setSampleRate (44100.0);
        shortEnv.setRMSWindow (0.0005f);
        shortEnv.setParams (0.0001f, 0.0f, 0.0001f, false, EnvelopeDetector::rms, false);
        shortEnv.reset();

        float lo = 1.0e6f, hi = -1.0e6f;
        for (int i = 0; i < 44100; i++)
        {
            auto v = shortEnv.process (std::sin (juce::MathConstants<float>::twoPi * 500.0f * float (i) / 44100.0f));

            if (i > 22050)
            {
                lo = std::min (lo, v);
                hi = std::max (hi, v);
            }
        }

        expectGreaterThan (hi - lo, 0.2f, "A short window should follow the waveform, not average it away");
    }

    void testLookaheadDelaysSignal()
    {
        beginTest ("Dynamics - Lookahead Delays The Signal");

        Dynamics comp;
        comp.setSampleRate (44100.0);
        comp.setNumChannels (1);
        comp.setMode (Dynamics::compressor);
        comp.setParams (0.01f, 0.0f, 0.1f, 0.0f, 1.0f, 0.0f);   // 1:1, so no gain change
        comp.setLookahead (0.005f);

        auto latency = comp.getLatencySamples();
        expectEquals (latency, int (0.005 * 44100.0), "Latency should match the lookahead time");

        juce::AudioSampleBuffer buffer (1, latency * 4);
        buffer.clear();
        buffer.setSample (0, 10, 0.5f);

        comp.process (buffer);

        expectWithinAbsoluteError (buffer.getSample (0, 10 + latency), 0.5f, 0.01f,
                                   "The impulse should come out delayed by the lookahead");
        expectWithinAbsoluteError (buffer.getSample (0, 10), 0.0f, 0.01f,
                                   "And nothing should be left where it went in");
    }

    void testLookaheadCatchesTransient()
    {
        beginTest ("Dynamics - Lookahead Catches The Front Of A Transient");

        // Silence, then a sustained loud tone. Without lookahead the first
        // instants get through before the envelope has moved; with it the gain
        // is already down when the tone arrives.
        juce::AudioSampleBuffer input (1, 8192);
        input.clear();

        for (int i = 2048; i < input.getNumSamples(); i++)
            input.setSample (0, i, std::sin (juce::MathConstants<float>::twoPi * 500.0f * float (i - 2048) / 44100.0f));

        auto run = [&] (float lookahead)
        {
            Dynamics comp;
            comp.setSampleRate (44100.0);
            comp.setNumChannels (1);
            comp.setMode (Dynamics::limiter);
            comp.setParams (0.005f, 0.0f, 0.1f, -20.0f, 20.0f, 0.0f);
            comp.setLookahead (lookahead);

            auto buffer = input;
            comp.process (buffer);

            // The overshoot right at the start of the tone
            return buffer.getMagnitude (0, 2048, 512);
        };

        auto without = run (0.0f);
        auto with = run (0.005f);

        expectLessThan (with, without, "Lookahead should reduce the overshoot at the transient");
    }

    void testSidechainFilterIgnoresLowEnd()
    {
        beginTest ("Dynamics - Side Chain High Pass");

        auto bass = makeSine (1, 8192, 50.0f, 1.0f);

        Dynamics plain;
        plain.setSampleRate (44100.0);
        plain.setNumChannels (1);
        plain.setMode (Dynamics::compressor);
        plain.setParams (0.001f, 0.0f, 0.05f, -20.0f, 8.0f, 0.0f);

        Dynamics filtered;
        filtered.setSampleRate (44100.0);
        filtered.setNumChannels (1);
        filtered.setMode (Dynamics::compressor);
        filtered.setParams (0.001f, 0.0f, 0.05f, -20.0f, 8.0f, 0.0f);
        filtered.setSidechainFilter (1000.0f, 0.0f);   // detector only hears above 1 kHz

        auto plainReduction = reductionFor (plain, bass);
        auto filteredReduction = reductionFor (filtered, bass);

        expectLessThan (plainReduction, 0.9f, "Without the filter the bass should compress");
        expectGreaterThan (filteredReduction, plainReduction * 2.0f,
                           "With a 1 kHz high pass on the detector, 50 Hz should barely compress");
    }

    void testDetectorPeakFocusesBand()
    {
        beginTest ("Dynamics - Detector Peaking Filter");

        auto sibilance = makeSine (1, 8192, 6000.0f, 0.2f);

        Dynamics plain;
        plain.setSampleRate (44100.0);
        plain.setNumChannels (1);
        plain.setMode (Dynamics::compressor);
        plain.setParams (0.001f, 0.0f, 0.05f, -12.0f, 8.0f, 0.0f);

        Dynamics deEsser;
        deEsser.setSampleRate (44100.0);
        deEsser.setNumChannels (1);
        deEsser.setMode (Dynamics::compressor);
        deEsser.setParams (0.001f, 0.0f, 0.05f, -12.0f, 8.0f, 0.0f);
        deEsser.setDetectorPeak (6000.0f, 1.0f, 18.0f);   // lean on 6 kHz

        auto plainReduction = reductionFor (plain, sibilance);
        auto peakedReduction = reductionFor (deEsser, sibilance);

        expectLessThan (peakedReduction, plainReduction,
                        "Boosting the detector at 6 kHz should compress a 6 kHz tone harder");
    }

    void testEnvelopeDetectorPeak()
    {
        beginTest ("Envelope Detector - Peak Mode");

        EnvelopeDetector env;
        env.setSampleRate (44100.0);
        env.setParams (0.001f, 0.0f, 0.1f, false, EnvelopeDetector::peak, false);
        env.reset();

        // Process a peak signal
        float result = env.process (0.5f);
        expect (result > 0.0f, "Should detect peak signal");
        expect (result <= 0.5f, "Envelope should not exceed input peak");

        // Process silence, should decay
        for (int i = 0; i < 100; i++)
            result = env.process (0.0f);

        expect (result < 0.5f, "Envelope should decay toward zero");
    }

    void testEnvelopeDetectorModes()
    {
        beginTest ("Envelope Detector - Detection Modes");

        float testSignal = 0.5f;

        // A zero window means no averaging, so ms and rms are the
        // instantaneous square and magnitude of a single sample
        EnvelopeDetector peakEnv;
        peakEnv.setSampleRate (44100.0);
        peakEnv.setRMSWindow (0.0f);
        peakEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::peak, false);
        peakEnv.reset();
        float peakResult = peakEnv.process (testSignal);

        // MS mode
        EnvelopeDetector msEnv;
        msEnv.setSampleRate (44100.0);
        msEnv.setRMSWindow (0.0f);
        msEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::ms, false);
        msEnv.reset();
        float msResult = msEnv.process (testSignal);

        // RMS mode - note: instantaneous RMS = |x|, same as peak
        EnvelopeDetector rmsEnv;
        rmsEnv.setSampleRate (44100.0);
        rmsEnv.setRMSWindow (0.0f);
        rmsEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::rms, false);
        rmsEnv.reset();
        float rmsResult = rmsEnv.process (testSignal);

        // Different modes should produce different results
        expectWithinAbsoluteError (peakResult, 0.5f, 0.01f, "Peak should be ~0.5");
        expectWithinAbsoluteError (msResult, 0.25f, 0.01f, "MS should be ~0.25 (0.5^2)");
        // Instantaneous RMS = sqrt(x^2) = |x|, so equals peak for single sample
        expectWithinAbsoluteError (rmsResult, 0.5f, 0.01f, "RMS should be ~0.5 (sqrt(0.5^2))");
    }

    void testEnvelopeDetectorAttackRelease()
    {
        beginTest ("Envelope Detector - Attack/Release");

        EnvelopeDetector env;
        env.setSampleRate (44100.0);

        // Fast attack, slow release
        env.setParams (0.001f, 0.0f, 0.5f, false, EnvelopeDetector::peak, false);
        env.reset();

        // Apply signal, should attack quickly
        float result1 = env.process (1.0f);
        float result2 = env.process (1.0f);
        expect (result2 > result1 * 0.9f, "Should rise quickly with fast attack");

        // Remove signal, should decay slowly
        float decayStart = env.process (0.0f);
        for (int i = 0; i < 100; i++)
            env.process (0.0f);
        float decayAfter = env.process (0.0f);

        expect (decayAfter < decayStart && decayAfter > decayStart * 0.5f,
               "Should decay slowly with slow release");
    }

    void testEnvelopeDetectorHold()
    {
        beginTest ("Envelope Detector - Hold Time");

        EnvelopeDetector env;
        env.setSampleRate (44100.0);

        // Set hold time of 0.1 seconds
        env.setParams (0.001f, 0.1f, 0.1f, false, EnvelopeDetector::peak, false);
        env.reset();

        // Attack with signal
        env.process (1.0f);
        env.process (1.0f);

        // Drop to zero, should hold
        float holdValue = env.process (0.0f);

        // Process for half the hold time (approx 2205 samples at 44100Hz)
        for (int i = 0; i < 2000; i++)
            env.process (0.0f);

        float stillHolding = env.process (0.0f);

        expect (stillHolding > holdValue * 0.8f,
               "Should maintain level during hold time");
    }

    void testEnvelopeDetectorLogMode()
    {
        beginTest ("Envelope Detector - Logarithmic Mode");

        EnvelopeDetector env;
        env.setSampleRate (44100.0);
        env.setParams (0.001f, 0.0f, 0.1f, false, EnvelopeDetector::peak, true);
        env.reset();

        // Process multiple samples to let envelope settle (1ms attack ≈ 100 samples to settle)
        float result = 0.0f;
        for (int i = 0; i < 200; i++)
            result = env.process (0.5f);

        // In log mode, should return dB value close to gainToDecibels(0.5) ≈ -6dB
        float expectedDb = juce::Decibels::gainToDecibels (0.5f);
        expect (result < 0.0f, "dB value for gain < 1 should be negative");
        expectWithinAbsoluteError (result, expectedDb, 1.0f, "Should be close to expected dB");

        // Process zero, should return very low dB
        env.reset();
        result = env.process (0.0f);
        expect (result < -50.0f, "Zero input should produce very low dB");
    }

    void testDynamicsCompressor()
    {
        beginTest ("Dynamics - Compressor");

        Dynamics comp;
        comp.setSampleRate (44100.0);
        comp.setNumChannels (2);
        comp.setMode (Dynamics::compressor);
        comp.setParams (0.01f, 0.0f, 0.1f, -20.0f, 4.0f, 0.0f);
        comp.reset();

        // Create a signal above threshold
        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Fill with loud signal (approximately -10dB = 0.316)
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.316f);

        comp.process (buffer);

        // Compressor should reduce the loud signal
        float rms = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
            rms += buffer.getSample (0, s) * buffer.getSample (0, s);
        rms = std::sqrt (rms / buffer.getNumSamples());

        expect (rms < 0.316f, "Compressor should reduce loud signals");
        expect (rms > 0.0f, "Should still have output");
    }

    void testDynamicsLimiter()
    {
        beginTest ("Dynamics - Limiter");

        Dynamics limiter;
        limiter.setSampleRate (44100.0);
        limiter.setNumChannels (2);
        limiter.setMode (Dynamics::limiter);
        limiter.setParams (0.001f, 0.0f, 0.05f, -6.0f, 100.0f, 0.0f);
        limiter.reset();

        // Create a signal well above threshold
        juce::AudioBuffer<float> buffer (2, 1000);
        buffer.clear();

        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.8f); // Loud signal

        limiter.process (buffer);

        // Limiter should strongly reduce peaks once envelope settles
        // Skip first 200 samples to allow 1ms attack time to settle
        float peak = 0.0f;
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 200; s < buffer.getNumSamples(); s++)
                peak = std::max (peak, std::abs (buffer.getSample (c, s)));

        expect (peak < 0.8f, "Limiter should reduce peaks");
    }

    void testDynamicsExpander()
    {
        beginTest ("Dynamics - Expander");

        Dynamics expander;
        expander.setSampleRate (44100.0);
        expander.setNumChannels (2);
        expander.setMode (Dynamics::expander);
        expander.setParams (0.01f, 0.0f, 0.1f, -40.0f, 2.0f, 0.0f);
        expander.reset();

        // Create a quiet signal below threshold
        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.01f); // Quiet signal

        float inputRMS = 0.01f;

        expander.process (buffer);

        // Expander should reduce quiet signals
        float outputRMS = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
            outputRMS += buffer.getSample (0, s) * buffer.getSample (0, s);
        outputRMS = std::sqrt (outputRMS / buffer.getNumSamples());

        expect (outputRMS < inputRMS,
               "Expander should reduce signals below threshold");
    }

    void testDynamicsGate()
    {
        beginTest ("Dynamics - Gate");

        Dynamics gate;
        gate.setSampleRate (44100.0);
        gate.setNumChannels (2);
        gate.setMode (Dynamics::gate);
        gate.setParams (0.001f, 0.0f, 0.05f, -40.0f, 100.0f, 0.0f);
        gate.reset();

        // Create a very quiet signal
        juce::AudioBuffer<float> buffer (2, 1000);
        buffer.clear();

        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 0.001f); // Very quiet

        gate.process (buffer);

        // Gate should heavily attenuate or silence the signal
        float rms = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
            rms += buffer.getSample (0, s) * buffer.getSample (0, s);
        rms = std::sqrt (rms / buffer.getNumSamples());

        expect (rms < 0.001f, "Gate should reduce signals below threshold");
    }

    void testDynamicsStereoLinking()
    {
        beginTest ("Dynamics - Stereo Linking");

        Dynamics comp;
        comp.setSampleRate (44100.0);
        comp.setNumChannels (2);
        comp.setMode (Dynamics::compressor);
        comp.setLinked (true);
        comp.setParams (0.01f, 0.0f, 0.1f, -20.0f, 4.0f, 0.0f);
        comp.reset();

        // Create stereo signal with different levels
        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Left channel loud, right channel quiet
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            buffer.setSample (0, s, 0.5f); // Loud left
            buffer.setSample (1, s, 0.1f); // Quiet right
        }

        comp.process (buffer);

        // With linking, both channels should be affected similarly
        float leftRMS = 0.0f;
        float rightRMS = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            leftRMS += buffer.getSample (0, s) * buffer.getSample (0, s);
            rightRMS += buffer.getSample (1, s) * buffer.getSample (1, s);
        }
        leftRMS = std::sqrt (leftRMS / buffer.getNumSamples());
        rightRMS = std::sqrt (rightRMS / buffer.getNumSamples());

        // Both channels should be compressed based on the louder channel
        expect (leftRMS < 0.5f, "Loud channel should be compressed");
        expect (rightRMS < 0.1f, "Quiet channel should also be affected when linked");

        // Test unlinked mode
        comp.setLinked (false);
        comp.reset();

        buffer.clear();
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            buffer.setSample (0, s, 0.5f); // Loud left
            buffer.setSample (1, s, 0.05f); // Very quiet right (below threshold)
        }

        comp.process (buffer);

        leftRMS = 0.0f;
        rightRMS = 0.0f;
        for (int s = 0; s < buffer.getNumSamples(); s++)
        {
            leftRMS += buffer.getSample (0, s) * buffer.getSample (0, s);
            rightRMS += buffer.getSample (1, s) * buffer.getSample (1, s);
        }
        leftRMS = std::sqrt (leftRMS / buffer.getNumSamples());
        rightRMS = std::sqrt (rightRMS / buffer.getNumSamples());

        expect (leftRMS < 0.5f, "Loud channel should be compressed when unlinked");
        // Right channel should be closer to original since it's below threshold
    }
};

static DynamicsTests dynamicsTests;

#endif

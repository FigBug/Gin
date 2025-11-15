/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

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
    }

private:
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

        // Peak mode
        EnvelopeDetector peakEnv;
        peakEnv.setSampleRate (44100.0);
        peakEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::peak, false);
        peakEnv.reset();
        float peakResult = peakEnv.process (testSignal);

        // MS mode
        EnvelopeDetector msEnv;
        msEnv.setSampleRate (44100.0);
        msEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::ms, false);
        msEnv.reset();
        float msResult = msEnv.process (testSignal);

        // RMS mode
        EnvelopeDetector rmsEnv;
        rmsEnv.setSampleRate (44100.0);
        rmsEnv.setParams (0.0f, 0.0f, 0.0f, false, EnvelopeDetector::rms, false);
        rmsEnv.reset();
        float rmsResult = rmsEnv.process (testSignal);

        // Different modes should produce different results
        expectWithinAbsoluteError (peakResult, 0.5f, 0.01f, "Peak should be ~0.5");
        expectWithinAbsoluteError (msResult, 0.25f, 0.01f, "MS should be ~0.25 (0.5^2)");
        expect (rmsResult > 0.0f && rmsResult < peakResult,
               "RMS should be between 0 and peak");
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

        // Process a known amplitude
        float result = env.process (0.5f);

        // In log mode, should return dB value
        float expectedDb = juce::Decibels::gainToDecibels (0.5f);
        expect (result < 0.0f, "dB value for gain < 1 should be negative");
        expectWithinAbsoluteError (result, expectedDb, 5.0f,
                                  "Should be in reasonable dB range");

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

        // Limiter should strongly reduce peaks
        float peak = 0.0f;
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
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

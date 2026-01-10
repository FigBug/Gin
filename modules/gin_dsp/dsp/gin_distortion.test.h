/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DistortionTests : public juce::UnitTest
{
public:
    DistortionTests() : juce::UnitTest ("Distortion", "gin_dsp") {}

    void runTest() override
    {
        testBasicProcessing();
        testDryMix();
        testSaturation();
        testReset();
    }

private:
    void testBasicProcessing()
    {
        beginTest ("Basic Processing");

        AirWindowsDistortion dist;
        dist.setSampleRate (44100.0);
        dist.setParams (0.5f, 0.0f, 1.0f, 1.0f); // moderate distortion, full wet

        juce::AudioBuffer<float> buffer (2, 512);

        // Create sine wave
        for (int i = 0; i < 512; i++)
        {
            float sample = std::sin (2.0f * juce::MathConstants<float>::pi * i / 100.0f) * 0.5f;
            buffer.setSample (0, i, sample);
            buffer.setSample (1, i, sample);
        }

        dist.process (buffer);

        // Should still have signal
        float rms = buffer.getRMSLevel (0, 0, 512);
        expect (rms > 0.0f, "Should produce output");
    }

    void testDryMix()
    {
        beginTest ("Dry Mix Passthrough");

        AirWindowsDistortion dist;
        dist.setSampleRate (44100.0);
        dist.setParams (1.0f, 0.0f, 1.0f, 0.0f); // 100% dry

        juce::AudioBuffer<float> buffer (2, 256);
        juce::AudioBuffer<float> original (2, 256);

        // Create signal
        for (int i = 0; i < 256; i++)
        {
            float sample = std::sin (2.0f * juce::MathConstants<float>::pi * i / 50.0f) * 0.3f;
            buffer.setSample (0, i, sample);
            buffer.setSample (1, i, sample);
            original.setSample (0, i, sample);
            original.setSample (1, i, sample);
        }

        dist.process (buffer);

        // Should be very close to original
        float maxDiff = 0.0f;
        for (int i = 0; i < 256; i++)
        {
            float diff = std::abs (buffer.getSample (0, i) - original.getSample (0, i));
            maxDiff = std::max (maxDiff, diff);
        }

        expect (maxDiff < 0.01f, "Dry mix should pass signal through mostly unchanged");
    }

    void testSaturation()
    {
        beginTest ("Saturation Effect");

        AirWindowsDistortion dist;
        dist.setSampleRate (44100.0);
        dist.setParams (2.0f, 0.0f, 1.0f, 1.0f); // high distortion

        juce::AudioBuffer<float> buffer (2, 256);

        // Create loud signal that should be saturated
        for (int i = 0; i < 256; i++)
        {
            float sample = std::sin (2.0f * juce::MathConstants<float>::pi * i / 50.0f) * 0.9f;
            buffer.setSample (0, i, sample);
            buffer.setSample (1, i, sample);
        }

        dist.process (buffer);

        // Check that peaks are reduced (saturation)
        float maxAbs = 0.0f;
        for (int i = 0; i < 256; i++)
        {
            maxAbs = std::max (maxAbs, std::abs (buffer.getSample (0, i)));
        }

        // With saturation, signal shouldn't exceed 1.0 by much
        expect (maxAbs < 1.5f, "Saturation should limit peaks");
    }

    void testReset()
    {
        beginTest ("Reset");

        AirWindowsDistortion dist;
        dist.setSampleRate (44100.0);
        dist.setParams (1.0f, 0.5f, 1.0f, 1.0f);

        // Process some audio to build up state
        juce::AudioBuffer<float> buffer (2, 256);
        for (int i = 0; i < 256; i++)
        {
            buffer.setSample (0, i, 0.5f);
            buffer.setSample (1, i, 0.5f);
        }
        dist.process (buffer);

        dist.reset();

        // After reset, should process cleanly without accumulated state
        buffer.clear();
        buffer.setSample (0, 0, 0.3f);
        buffer.setSample (1, 0, 0.3f);

        dist.process (buffer);

        // First sample should be processed
        expect (buffer.getSample (0, 0) != 0.0f, "Should process sample after reset");
    }
};

static DistortionTests distortionTests;

#endif

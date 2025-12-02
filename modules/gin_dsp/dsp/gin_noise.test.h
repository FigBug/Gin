/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class NoiseTests : public juce::UnitTest
{
public:
    NoiseTests() : juce::UnitTest ("Noise Generators", "gin_dsp") {}

    void runTest() override
    {
        testWhiteNoiseMean();
        testWhiteNoiseDistribution();
        testPinkNoiseGeneration();
        testNoiseVariance();
    }

private:
    void testWhiteNoiseMean()
    {
        beginTest ("White Noise Mean");

        WhiteNoise noise;

        // Generate samples and check mean is near 0
        double sum = 0.0;
        const int numSamples = 10000;

        for (int i = 0; i < numSamples; i++)
            sum += noise.nextSample();

        double mean = sum / numSamples;
        expect (std::abs (mean) < 0.01, "Mean should be close to 0");
    }

    void testWhiteNoiseDistribution()
    {
        beginTest ("White Noise Distribution");

        WhiteNoise noise;

        // Check that values are roughly within expected range
        int outsideRange = 0;
        const int numSamples = 10000;

        for (int i = 0; i < numSamples; i++)
        {
            float sample = noise.nextSample();
            if (std::abs (sample) > 0.5f) // ~5 std deviations
                outsideRange++;
        }

        // Very few samples should be outside this range
        expect (outsideRange < numSamples * 0.001, "Very few samples should be extreme outliers");
    }

    void testPinkNoiseGeneration()
    {
        beginTest ("Pink Noise Generation");

        PinkNoise noise;

        // Just verify it generates values without crashing
        float minVal = 1000.0f;
        float maxVal = -1000.0f;

        for (int i = 0; i < 10000; i++)
        {
            float sample = noise.nextSample();
            minVal = std::min (minVal, sample);
            maxVal = std::max (maxVal, sample);
        }

        expect (minVal < 0.0f, "Should generate negative values");
        expect (maxVal > 0.0f, "Should generate positive values");
        expect (maxVal - minVal > 0.01f, "Should have reasonable range");
    }

    void testNoiseVariance()
    {
        beginTest ("White Noise Variance");

        WhiteNoise noise;

        // Standard deviation should be approximately 0.1 (as per class definition)
        double sumSq = 0.0;
        const int numSamples = 10000;

        for (int i = 0; i < numSamples; i++)
        {
            float sample = noise.nextSample();
            sumSq += sample * sample;
        }

        double variance = sumSq / numSamples;
        double stddev = std::sqrt (variance);

        // Should be close to 0.1
        expectWithinAbsoluteError ((float) stddev, 0.1f, 0.02f, "Standard deviation should be ~0.1");
    }
};

static NoiseTests noiseTests;

#endif

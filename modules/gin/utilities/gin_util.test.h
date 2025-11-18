/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class UtilTests : public juce::UnitTest
{
public:
    UtilTests() : juce::UnitTest ("Utility Functions", "gin") {}

    void runTest() override
    {
        testCompareAndReset();
        testCalculateRMS();
        testCalculateMedian();
        testShuffleArray();
        testRollingAverage();
        testPerlinNoise();
    }

private:
    void testCompareAndReset()
    {
        beginTest ("Compare And Reset");

        bool flag = true;
        expect (compareAndReset (flag), "Should return true when flag is set");
        expect (!flag, "Flag should be cleared");

        expect (!compareAndReset (flag), "Should return false when flag is already clear");
        expect (!flag, "Flag should remain clear");

        flag = true;
        expect (compareAndReset (flag), "Should return true again after being set");
    }

    void testCalculateRMS()
    {
        beginTest ("Calculate RMS");

        // Test with known values
        float values1[] = {1.0f, 2.0f, 3.0f, 4.0f};
        float rms1 = calculateRMS (values1, 4);
        // RMS = sqrt((1 + 4 + 9 + 16) / 4) = sqrt(30/4) = sqrt(7.5) ≈ 2.739
        expectWithinAbsoluteError (rms1, 2.739f, 0.001f, "RMS should be ~2.739");

        // Test with zeros
        float values2[] = {0.0f, 0.0f, 0.0f};
        float rms2 = calculateRMS (values2, 3);
        expectWithinAbsoluteError (rms2, 0.0f, 0.001f, "RMS of zeros should be 0");

        // Test with constant value
        float values3[] = {2.0f, 2.0f, 2.0f, 2.0f};
        float rms3 = calculateRMS (values3, 4);
        expectWithinAbsoluteError (rms3, 2.0f, 0.001f, "RMS of constant should equal the value");

        // Test with alternating positive/negative
        float values4[] = {1.0f, -1.0f, 1.0f, -1.0f};
        float rms4 = calculateRMS (values4, 4);
        expectWithinAbsoluteError (rms4, 1.0f, 0.001f, "RMS should be 1.0");
    }

    void testCalculateMedian()
    {
        beginTest ("Calculate Median");

        // Odd number of values
        float values1[] = {5.0f, 1.0f, 3.0f, 4.0f, 2.0f};
        float median1 = calculateMedian (values1, 5);
        expectWithinAbsoluteError (median1, 3.0f, 0.001f, "Median of odd array should be middle value");

        // Even number of values
        float values2[] = {1.0f, 2.0f, 3.0f, 4.0f};
        float median2 = calculateMedian (values2, 4);
        expectWithinAbsoluteError (median2, 2.5f, 0.001f, "Median of even array should be average of two middle values");

        // Single value
        float values3[] = {7.0f};
        float median3 = calculateMedian (values3, 1);
        expectWithinAbsoluteError (median3, 7.0f, 0.001f, "Median of single value should be that value");

        // Two values
        float values4[] = {3.0f, 7.0f};
        float median4 = calculateMedian (values4, 2);
        expectWithinAbsoluteError (median4, 5.0f, 0.001f, "Median of two values should be average");

        // Unsorted input
        float values5[] = {10.0f, 1.0f, 5.0f, 8.0f, 3.0f};
        float median5 = calculateMedian (values5, 5);
        expectWithinAbsoluteError (median5, 5.0f, 0.001f, "Should handle unsorted input");
    }

    void testShuffleArray()
    {
        beginTest ("Shuffle Array");

        juce::Random rng (12345);
        juce::Array<int> arr;

        // Create ordered array
        for (int i = 0; i < 10; i++)
            arr.add (i);

        juce::Array<int> original = arr;

        // Shuffle it
        shuffleArray (rng, arr);

        // Check all elements still present
        bool allPresent = true;
        for (int i = 0; i < 10; i++)
        {
            if (!arr.contains (i))
            {
                allPresent = false;
                break;
            }
        }
        expect (allPresent, "All original elements should still be present");

        // Check size unchanged
        expectEquals (arr.size(), 10, "Array size should remain unchanged");

        // Check that order changed (highly likely with random shuffle)
        bool orderChanged = false;
        for (int i = 0; i < 10; i++)
        {
            if (arr[i] != original[i])
            {
                orderChanged = true;
                break;
            }
        }
        expect (orderChanged, "Array order should have changed");

        // Test with single element (should remain unchanged)
        juce::Array<int> single;
        single.add (42);
        shuffleArray (rng, single);
        expectEquals (single[0], 42, "Single element array should remain unchanged");
    }

    void testRollingAverage()
    {
        beginTest ("RollingAverage");

        RollingAverage avg (4);

        // Test initial state
        expectWithinAbsoluteError (avg.getAverage(), 0.0, 0.0001, "Initial average should be 0");

        // Test weighted averaging: (nextValue + numVals * currAvg) / (numVals + 1)
        // First value: (10 + 4*0) / 5 = 2.0
        double result1 = avg.average (10.0);
        expectWithinAbsoluteError (result1, 2.0, 0.0001, "First average should be 2.0");
        expectWithinAbsoluteError (avg.getAverage(), 2.0, 0.0001, "Stored average should be 2.0");

        // Second value: (5 + 4*2) / 5 = 13/5 = 2.6
        double result2 = avg.average (5.0);
        expectWithinAbsoluteError (result2, 2.6, 0.0001, "Second average should be 2.6");
        expectWithinAbsoluteError (avg.getAverage(), 2.6, 0.0001, "Stored average should be 2.6");

        // Test setAverage
        avg.setAverage (10.0);
        expectWithinAbsoluteError (avg.getAverage(), 10.0, 0.0001, "Average after setAverage should be 10.0");

        // Verify calculation continues correctly after setAverage
        // (20 + 4*10) / 5 = 60/5 = 12.0
        double result3 = avg.average (20.0);
        expectWithinAbsoluteError (result3, 12.0, 0.0001, "Average after manual set should calculate correctly");

        // Test with different weighting factor
        RollingAverage avg2 (1);
        // (5 + 1*0) / 2 = 2.5
        expectWithinAbsoluteError (avg2.average (5.0), 2.5, 0.0001, "Lower weight factor should give more weight to new value");
    }

    void testPerlinNoise()
    {
        beginTest ("PerlinNoise");

        PerlinNoise<float> noise;

        // Test that noise returns values in expected range [-1, 1]
        for (int i = 0; i < 100; i++)
        {
            float x = float (i) * 0.1f;
            float value = noise.noise (x);
            expect (value >= -1.0f && value <= 1.0f, "Noise value should be in range [-1, 1]");
        }

        // Test 2D noise
        for (int i = 0; i < 100; i++)
        {
            float x = float (i) * 0.1f;
            float y = float (i) * 0.05f;
            float value = noise.noise (x, y);
            expect (value >= -1.0f && value <= 1.0f, "2D noise value should be in range [-1, 1]");
        }

        // Test 3D noise
        for (int i = 0; i < 100; i++)
        {
            float x = float (i) * 0.1f;
            float y = float (i) * 0.05f;
            float z = float (i) * 0.02f;
            float value = noise.noise (x, y, z);
            expect (value >= -1.0f && value <= 1.0f, "3D noise value should be in range [-1, 1]");
        }

        // Test that different seeds produce different results
        PerlinNoise<float> noise1 (123);
        PerlinNoise<float> noise2 (456);

        bool foundDifference = false;
        for (int i = 0; i < 10; i++)
        {
            float val1 = noise1.noise (float (i));
            float val2 = noise2.noise (float (i));
            if (! juce::approximatelyEqual (val1, val2))
            {
                foundDifference = true;
                break;
            }
        }
        expect (foundDifference, "Different seeds should produce different noise patterns");

        // Test consistency (same input should give same output)
        float x = 3.14f;
        float value1 = noise.noise (x);
        float value2 = noise.noise (x);
        expectWithinAbsoluteError (value1, value2, 0.0001f, "Same input should produce same output");
    }
};

static UtilTests utilTests;

#endif

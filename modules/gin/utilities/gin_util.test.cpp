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
};

static UtilTests utilTests;

#endif

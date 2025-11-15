/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
int versionStringToInt (const juce::String& versionString)
{
    juce::StringArray parts;
    parts.addTokens (versionString, ".", "");
    parts.trim();
    parts.removeEmptyStrings();

    int res = 0;

    for (auto part : parts)
        res = (res << 8) + part.getIntValue();

    return res;
}

//==============================================================================
class DelayedLambdaHelper : public juce::Timer
{
public:
    DelayedLambdaHelper (std::function<void ()>& cb, int ms)
        : callback (cb)
    {
        startTimer (ms);
    }

    void timerCallback() override
    {
        callback();
        delete this;
    }

    std::function<void ()> callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayedLambdaHelper)
};

void delayedLambda (std::function<void ()> callback, int delayMS)
{
    new DelayedLambdaHelper (callback, delayMS);
}

//==============================================================================
#if JUCE_UNIT_TESTS

class GinUtilTests : public juce::UnitTest
{
public:
    GinUtilTests() : juce::UnitTest ("Gin Utilities", "gin") {}

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
        beginTest ("compareAndReset");

        int value = 42;
        expect (compareAndReset (value, 42) == true, "Should return true when values match");
        expect (value == 0, "Value should be reset to 0");

        value = 10;
        expect (compareAndReset (value, 5) == false, "Should return false when values don't match");
        expect (value == 10, "Value should not be reset when comparison fails");

        float fvalue = 3.14f;
        expect (compareAndReset (fvalue, 3.14f) == true, "Should work with floats");
        expectWithinAbsoluteError (fvalue, 0.0f, 0.0001f, "Float should be reset to 0");
    }

    void testCalculateRMS()
    {
        beginTest ("calculateRMS");

        juce::AudioBuffer<float> buffer (1, 100);
        buffer.clear();

        // Test silence
        float rms = calculateRMS (buffer, 0);
        expectWithinAbsoluteError (rms, 0.0f, 0.0001f, "RMS of silence should be 0");

        // Test with known values
        buffer.setSample (0, 0, 1.0f);
        buffer.setSample (0, 1, 1.0f);
        buffer.setSample (0, 2, 1.0f);
        buffer.setSample (0, 3, 1.0f);

        rms = calculateRMS (buffer.getReadPointer (0), 4);
        expectWithinAbsoluteError (rms, 1.0f, 0.0001f, "RMS of all 1s should be 1");

        // Test with mixed values
        buffer.clear();
        buffer.setSample (0, 0, 0.5f);
        buffer.setSample (0, 1, -0.5f);
        buffer.setSample (0, 2, 0.5f);
        buffer.setSample (0, 3, -0.5f);

        rms = calculateRMS (buffer.getReadPointer (0), 4);
        expectWithinAbsoluteError (rms, 0.5f, 0.0001f, "RMS should be 0.5");
    }

    void testCalculateMedian()
    {
        beginTest ("calculateMedian");

        // Test with odd number of elements
        juce::Array<float> oddArray = { 5.0f, 1.0f, 3.0f, 2.0f, 4.0f };
        expectWithinAbsoluteError (calculateMedian (oddArray), 3.0f, 0.0001f,
                                   "Median of {5,1,3,2,4} should be 3");

        // Test with even number of elements
        juce::Array<float> evenArray = { 1.0f, 2.0f, 3.0f, 4.0f };
        expectWithinAbsoluteError (calculateMedian (evenArray), 2.5f, 0.0001f,
                                   "Median of {1,2,3,4} should be 2.5");

        // Test with single element
        juce::Array<float> singleArray = { 42.0f };
        expectWithinAbsoluteError (calculateMedian (singleArray), 42.0f, 0.0001f,
                                   "Median of single element should be that element");

        // Test with duplicates
        juce::Array<float> duplicateArray = { 1.0f, 2.0f, 2.0f, 2.0f, 3.0f };
        expectWithinAbsoluteError (calculateMedian (duplicateArray), 2.0f, 0.0001f,
                                   "Median should handle duplicates");
    }

    void testShuffleArray()
    {
        beginTest ("shuffleArray");

        juce::Array<int> original = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        juce::Array<int> shuffled = original;

        shuffleArray (shuffled);

        // Check that all elements are still present
        expect (shuffled.size() == original.size(), "Array size should not change");

        for (auto val : original)
            expect (shuffled.contains (val), "All original elements should be present");

        // Statistical test: shuffled array should be different from original
        // (This could fail by chance but is extremely unlikely with 10 elements)
        bool isDifferent = false;
        for (int i = 0; i < original.size(); i++)
        {
            if (original[i] != shuffled[i])
            {
                isDifferent = true;
                break;
            }
        }
        expect (isDifferent, "Shuffled array should be different from original (may rarely fail)");
    }

    void testRollingAverage()
    {
        beginTest ("RollingAverage");

        RollingAverage<float> avg (4);

        // Test initial state
        expectWithinAbsoluteError (avg.getAverage(), 0.0f, 0.0001f, "Initial average should be 0");

        // Add values
        avg.add (1.0f);
        expectWithinAbsoluteError (avg.getAverage(), 1.0f, 0.0001f, "Average of [1] should be 1");

        avg.add (2.0f);
        expectWithinAbsoluteError (avg.getAverage(), 1.5f, 0.0001f, "Average of [1,2] should be 1.5");

        avg.add (3.0f);
        expectWithinAbsoluteError (avg.getAverage(), 2.0f, 0.0001f, "Average of [1,2,3] should be 2");

        avg.add (4.0f);
        expectWithinAbsoluteError (avg.getAverage(), 2.5f, 0.0001f, "Average of [1,2,3,4] should be 2.5");

        // Test rolling window
        avg.add (5.0f);
        expectWithinAbsoluteError (avg.getAverage(), 3.5f, 0.0001f, "Average of [2,3,4,5] should be 3.5");

        // Test clear
        avg.clear();
        expectWithinAbsoluteError (avg.getAverage(), 0.0f, 0.0001f, "Average after clear should be 0");
    }

    void testPerlinNoise()
    {
        beginTest ("PerlinNoise");

        PerlinNoise noise;

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
        PerlinNoise noise1 (123);
        PerlinNoise noise2 (456);

        bool foundDifference = false;
        for (int i = 0; i < 10; i++)
        {
            if (noise1.noise (float (i)) != noise2.noise (float (i)))
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

static GinUtilTests ginUtilTests;

#endif

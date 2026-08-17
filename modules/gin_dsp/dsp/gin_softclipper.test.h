/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class SoftClipperTests : public juce::UnitTest
{
public:
    SoftClipperTests() : juce::UnitTest ("SoftClipper", "gin_dsp") {}

    void runTest() override
    {
        testNeverExceedsCeiling();
        testBelowKneeIsUntouched();
        testMonotonicAndOdd();
        testKneeIsContinuous();
    }

private:
    void testNeverExceedsCeiling()
    {
        beginTest ("Never Exceeds The Ceiling");

        SoftClipper clipper;
        clipper.setDrive (12.0f);
        clipper.setKnee (0.5f);
        clipper.setCeiling (-1.0f);

        auto ceiling = juce::Decibels::decibelsToGain (-1.0f);

        for (float x = -4.0f; x <= 4.0f; x += 0.01f)
        {
            auto y = clipper.processSample (x);
            expect (std::abs (y) <= ceiling * 1.0001f,
                    "Output " + juce::String (y) + " for input " + juce::String (x) + " exceeded the ceiling");
        }
    }

    void testBelowKneeIsUntouched()
    {
        beginTest ("Below The Knee Nothing Is Shaped");

        SoftClipper clipper;
        clipper.setDrive (12.0f);
        clipper.setKnee (1.0f);      // knee runs from -12 dB up to +12 dB
        clipper.setCeiling (0.0f);

        auto drive = juce::Decibels::decibelsToGain (12.0f);

        // -30 dB in, driven to -18 dB, still well below the knee start
        auto x = juce::Decibels::decibelsToGain (-30.0f);

        expectWithinAbsoluteError (clipper.processSample (x), x * drive, 1.0e-5f,
                                   "Below the knee only the drive should apply");
    }

    void testMonotonicAndOdd()
    {
        beginTest ("Curve Is Monotonic And Symmetric");

        SoftClipper clipper;
        clipper.setDrive (6.0f);
        clipper.setKnee (0.75f);
        clipper.setCeiling (0.0f);

        float last = clipper.processSample (0.0f);

        for (float x = 0.001f; x <= 3.0f; x += 0.001f)
        {
            auto y = clipper.processSample (x);

            expect (y >= last - 1.0e-6f, "Curve should never turn back on itself");
            expectWithinAbsoluteError (clipper.processSample (-x), -y, 1.0e-6f,
                                       "Curve should be symmetric about zero");
            last = y;
        }
    }

    void testKneeIsContinuous()
    {
        beginTest ("No Step At The Knee Edges");

        SoftClipper clipper;
        clipper.setDrive (10.0f);
        clipper.setKnee (0.6f);
        clipper.setCeiling (0.0f);

        // The knee spans +/-6 dB about 0 dB after the 10 dB of drive, which is
        // -16 dB to -4 dB at the input. Step across both edges finely and check
        // nothing jumps.
        for (auto edgeDb : { -16.0f, -4.0f })
        {
            auto below = clipper.processSample (juce::Decibels::decibelsToGain (edgeDb - 0.001f));
            auto above = clipper.processSample (juce::Decibels::decibelsToGain (edgeDb + 0.001f));

            expectWithinAbsoluteError (above, below, 0.001f,
                                       "Gain should be continuous across the knee edge at "
                                       + juce::String (edgeDb) + " dB");
        }
    }
};

static SoftClipperTests softClipperTests;

#endif

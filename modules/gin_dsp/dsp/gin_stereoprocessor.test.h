/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class StereoProcessorTests : public juce::UnitTest
{
public:
    StereoProcessorTests() : juce::UnitTest ("StereoProcessor", "gin_dsp") {}

    void runTest() override
    {
        testIdentity();
        testMonoStaysCentred();
    }

private:
    void fillTestSignal (juce::AudioSampleBuffer& buffer)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            buffer.setSample (0, i, std::sin (i * 0.13f));
            buffer.setSample (1, i, std::sin (i * 0.07f + 1.0f));
        }
    }

    void testIdentity()
    {
        beginTest ("Neutral Settings Pass Signal Through");

        StereoProcessor proc;
        // width = 1, centre faders = 0.75 (standard M/S reconstruction),
        // pans centred, rotation centred (0 degrees), output at unity (0.5 -> 0 dB)
        proc.setParameters (1.0f, 0.75f, 0.0f, 0.5f, 0.0f, 0.75f, 1.0f, 0.5f);

        juce::AudioSampleBuffer buffer (2, 64);
        fillTestSignal (buffer);

        juce::AudioSampleBuffer expected;
        expected.makeCopyOf (buffer);

        proc.process (buffer);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i), 1.0e-4f, "Left unchanged");
            expectWithinAbsoluteError (buffer.getSample (1, i), expected.getSample (1, i), 1.0e-4f, "Right unchanged");
        }
    }

    void testMonoStaysCentred()
    {
        beginTest ("Mono Input Stays Balanced");

        StereoProcessor proc;
        proc.setParameters (0.7f, 0.6f, 0.0f, 0.5f, 0.0f, 0.4f, 0.3f, 0.6f);

        juce::AudioSampleBuffer buffer (2, 64);
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto v = std::sin (i * 0.2f);
            buffer.setSample (0, i, v);
            buffer.setSample (1, i, v);
        }

        proc.process (buffer);

        // A mono (identical L/R) source with centred pans must remain balanced
        for (int i = 0; i < buffer.getNumSamples(); i++)
            expectWithinAbsoluteError (buffer.getSample (0, i), buffer.getSample (1, i), 1.0e-4f, "Left equals right for mono");
    }
};

static StereoProcessorTests stereoProcessorTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class KickSynthTests : public juce::UnitTest
{
public:
    KickSynthTests() : juce::UnitTest ("KickSynth", "gin_dsp") {}

    void runTest() override
    {
        testNoWetIsTransparent();
        testHitProducesLowEnd();
        testSilenceStaysSilent();
        testSidechainKeys();
    }

private:
    static constexpr double sr = 44100.0;

    // A kick-like burst: a short low sine with a fast decay
    static juce::AudioSampleBuffer makeKick (int totalSamples, int start, int length)
    {
        juce::AudioSampleBuffer b (2, totalSamples);
        b.clear();

        for (int i = 0; i < length && start + i < totalSamples; i++)
        {
            auto env = std::exp (-4.0f * float (i) / float (length));
            auto s = env * std::sin (juce::MathConstants<float>::twoPi * 80.0f * float (i) / float (sr));
            b.setSample (0, start + i, s);
            b.setSample (1, start + i, s);
        }

        return b;
    }

    void testNoWetIsTransparent()
    {
        beginTest ("No Wet Passes The Signal Through");

        KickSynth kick;
        kick.setSampleRate (sr);
        kick.setWet (0.0f);
        kick.setDry (1.0f);

        auto buffer = makeKick (8192, 1000, 2000);
        auto expected = buffer;

        kick.process (buffer);

        for (int i = 0; i < buffer.getNumSamples(); i++)
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i), 1.0e-6f,
                                       "With no wet the dry signal should be untouched");
    }

    void testHitProducesLowEnd()
    {
        beginTest ("A Hit Triggers The Oscillator");

        KickSynth kick;
        kick.setSampleRate (sr);
        kick.setDetectorFrequency (80.0f);
        kick.setFrequency (50.0f);
        kick.setLength (0.3f);
        kick.setWet (1.0f);
        kick.setDry (0.0f);   // wet only, so anything here was synthesised

        auto buffer = makeKick (16384, 4000, 1500);
        kick.process (buffer);

        auto synthesised = buffer.getMagnitude (0, 4000, 8000);

        expectGreaterThan (synthesised, 0.001f, "A hit should trigger the oscillator");
    }

    void testSilenceStaysSilent()
    {
        beginTest ("Silence In, Silence Out");

        KickSynth kick;
        kick.setSampleRate (sr);
        kick.setWet (1.0f);
        kick.setDry (1.0f);

        juce::AudioSampleBuffer buffer (2, 8192);
        buffer.clear();

        kick.process (buffer);

        expectWithinAbsoluteError (buffer.getMagnitude (0, 0, buffer.getNumSamples()), 0.0f, 1.0e-6f,
                                   "Nothing should be synthesised from silence");
    }

    void testSidechainKeys()
    {
        beginTest ("Side Chain Keys The Oscillator");

        KickSynth kick;
        kick.setSampleRate (sr);
        kick.setDetectorFrequency (80.0f);
        kick.setFrequency (50.0f);
        kick.setWet (1.0f);
        kick.setDry (0.0f);

        // Silent signal, hit only in the key: everything out is synthesised
        // from the side chain
        juce::AudioSampleBuffer buffer (2, 16384);
        buffer.clear();

        auto key = makeKick (16384, 4000, 1500);

        kick.process (buffer, key);

        expectGreaterThan (buffer.getMagnitude (0, 4000, 8000), 0.001f,
                           "The side chain should trigger the oscillator");
    }
};

static KickSynthTests kickSynthTests;

#endif

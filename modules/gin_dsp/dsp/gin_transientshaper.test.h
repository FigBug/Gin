/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class TransientShaperTests : public juce::UnitTest
{
public:
    TransientShaperTests() : juce::UnitTest ("TransientShaper", "gin_dsp") {}

    void runTest() override
    {
        testNeutralSettles();
        testAttackSharpensTransient();
        testNegativeAttackSoftensTransient();
        testBandLimitPassesOutOfBandDry();
        testReset();
    }

private:
    static constexpr double sr = 44100.0;

    // A short burst preceded by silence - a transient with an obvious front
    static juce::AudioSampleBuffer makeHit (int totalSamples, int hitStart, int hitLength)
    {
        juce::AudioSampleBuffer b (2, totalSamples);
        b.clear();

        for (int i = 0; i < hitLength && hitStart + i < totalSamples; i++)
        {
            auto env = 1.0f - float (i) / float (hitLength);
            auto s = env * std::sin (juce::MathConstants<float>::twoPi * 220.0f * float (i) / float (sr));
            b.setSample (0, hitStart + i, s);
            b.setSample (1, hitStart + i, s);
        }

        return b;
    }

    static float peakAfter (const juce::AudioSampleBuffer& b, int from)
    {
        return b.getMagnitude (0, from, b.getNumSamples() - from);
    }

    void testNeutralSettles()
    {
        beginTest ("Neutral Settings Pass Signal Through");

        TransientShaper shaper;
        shaper.setSampleRate (sr);
        shaper.setAttack (0.0f);
        shaper.setSustain (0.0f);

        // The gain smoother starts from zero, so give it time to reach unity
        juce::AudioSampleBuffer warmup (2, int (sr));
        for (int i = 0; i < warmup.getNumSamples(); i++)
        {
            auto s = 0.25f * std::sin (juce::MathConstants<float>::twoPi * 440.0f * float (i) / float (sr));
            warmup.setSample (0, i, s);
            warmup.setSample (1, i, s);
        }

        auto expected = warmup;
        shaper.process (warmup);

        // Compare the last quarter second, by which point the smoother has settled
        auto from = int (sr * 0.75);
        for (int i = from; i < warmup.getNumSamples(); i++)
            expectWithinAbsoluteError (warmup.getSample (0, i), expected.getSample (0, i), 0.01f,
                                       "Neutral settings should not change the signal");
    }

    void testAttackSharpensTransient()
    {
        beginTest ("Positive Attack Sharpens Transient");

        auto hitStart = int (sr * 0.5);

        auto neutral = makeHit (int (sr), hitStart, 2000);
        auto shaped = neutral;

        TransientShaper a;
        a.setSampleRate (sr);
        a.setAttack (0.0f);
        a.process (neutral);

        TransientShaper b;
        b.setSampleRate (sr);
        b.setAttack (3.0f);
        b.process (shaped);

        expectGreaterThan (peakAfter (shaped, hitStart), peakAfter (neutral, hitStart),
                           "Positive attack should make the transient bigger");
    }

    void testNegativeAttackSoftensTransient()
    {
        beginTest ("Negative Attack Softens Transient");

        auto hitStart = int (sr * 0.5);

        auto neutral = makeHit (int (sr), hitStart, 2000);
        auto shaped = neutral;

        TransientShaper a;
        a.setSampleRate (sr);
        a.setAttack (0.0f);
        a.process (neutral);

        TransientShaper b;
        b.setSampleRate (sr);
        b.setAttack (-3.0f);
        b.process (shaped);

        expectLessThan (peakAfter (shaped, hitStart), peakAfter (neutral, hitStart),
                        "Negative attack should soften the transient");
    }

    void testBandLimitPassesOutOfBandDry()
    {
        beginTest ("Out Of Band Content Passes Through");

        // Shape only 2-4 kHz, feed nothing but a 100 Hz tone: whatever the
        // shaper does to its band, this signal should come out as it went in
        TransientShaper shaper;
        shaper.setSampleRate (sr);
        shaper.setAttack (4.0f);
        shaper.setSustain (-4.0f);
        shaper.setBand (2000.0f, 4000.0f);

        juce::AudioSampleBuffer buffer (2, int (sr));
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto s = 0.5f * std::sin (juce::MathConstants<float>::twoPi * 100.0f * float (i) / float (sr));
            buffer.setSample (0, i, s);
            buffer.setSample (1, i, s);
        }

        auto expected = buffer;
        shaper.process (buffer);

        auto from = int (sr * 0.75);
        for (int i = from; i < buffer.getNumSamples(); i++)
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i), 0.05f,
                                       "A 100 Hz tone should survive a 2-4 kHz band setting");
    }

    void testReset()
    {
        beginTest ("Reset Clears State");

        TransientShaper shaper;
        shaper.setSampleRate (sr);
        shaper.setAttack (2.0f);

        auto first = makeHit (8192, 1000, 2000);
        auto second = first;

        shaper.process (first);
        shaper.reset();
        shaper.process (second);

        for (int i = 0; i < second.getNumSamples(); i++)
            expectWithinAbsoluteError (second.getSample (0, i), first.getSample (0, i), 1.0e-6f,
                                       "After reset the same input should give the same output");
    }
};

static TransientShaperTests transientShaperTests;

#endif

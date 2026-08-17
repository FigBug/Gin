/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ExciterTests : public juce::UnitTest
{
public:
    ExciterTests() : juce::UnitTest ("Exciter", "gin_dsp") {}

    void runTest() override
    {
        testZeroMixIsTransparent();
        testLeavesLowContentAlone();
        testAddsHarmonics();
        testReset();
    }

private:
    static constexpr double sr = 44100.0;

    static juce::AudioSampleBuffer makeSine (int numSamples, float freq, float amplitude)
    {
        juce::AudioSampleBuffer b (2, numSamples);

        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < numSamples; i++)
                b.setSample (ch, i, amplitude * std::sin (juce::MathConstants<float>::twoPi * freq * float (i) / float (sr)));

        return b;
    }

    // Energy at a frequency, by correlating against a complex exponential
    static double energyAt (const juce::AudioSampleBuffer& b, float freq)
    {
        double re = 0.0, im = 0.0;

        for (int i = 0; i < b.getNumSamples(); i++)
        {
            auto phase = juce::MathConstants<double>::twoPi * freq * double (i) / sr;
            re += b.getSample (0, i) * std::cos (phase);
            im += b.getSample (0, i) * std::sin (phase);
        }

        return std::sqrt (re * re + im * im) / b.getNumSamples();
    }

    void testZeroMixIsTransparent()
    {
        beginTest ("Zero Mix Passes The Signal Through");

        Exciter exciter;
        exciter.setSampleRate (sr);
        exciter.setFrequency (5000.0f);
        exciter.setMix (0.0f);

        auto buffer = makeSine (4096, 1000.0f, 0.5f);
        auto expected = buffer;

        exciter.process (buffer);

        for (int i = 0; i < buffer.getNumSamples(); i++)
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i), 1.0e-6f,
                                       "With no mix the dry signal should be untouched");
    }

    void testLeavesLowContentAlone()
    {
        beginTest ("Content Below The Crossover Is Left Alone");

        Exciter exciter;
        exciter.setSampleRate (sr);
        exciter.setFrequency (8000.0f);
        exciter.setDrive (4.0f);
        exciter.setHarmonics (0.5f);
        exciter.setMix (1.0f);

        auto buffer = makeSine (8192, 100.0f, 0.5f);
        auto expected = buffer;

        exciter.process (buffer);

        auto before = energyAt (expected, 100.0f);
        auto after = energyAt (buffer, 100.0f);

        expectWithinAbsoluteError (float (after), float (before), float (before) * 0.1f,
                                   "A 100 Hz tone should survive an 8 kHz crossover");
    }

    void testAddsHarmonics()
    {
        beginTest ("Adds Harmonics Above The Crossover");

        Exciter exciter;
        exciter.setSampleRate (sr);
        exciter.setFrequency (2000.0f);
        exciter.setDrive (6.0f);
        exciter.setHarmonics (0.8f);
        exciter.setMix (1.0f);

        auto buffer = makeSine (8192, 3000.0f, 0.5f);
        auto expected = buffer;

        exciter.process (buffer);

        // A pure sine has nothing at the third harmonic; the shaper should
        // put something there
        auto before = energyAt (expected, 9000.0f);
        auto after = energyAt (buffer, 9000.0f);

        expectGreaterThan (after, before * 4.0 + 1.0e-5,
                           "Should generate harmonic content that was not there before");
    }

    void testReset()
    {
        beginTest ("Reset Clears State");

        Exciter exciter;
        exciter.setSampleRate (sr);
        exciter.setFrequency (3000.0f);
        exciter.setDrive (4.0f);
        exciter.setHarmonics (0.5f);
        exciter.setMix (1.0f);

        auto first = makeSine (2048, 5000.0f, 0.5f);
        auto second = first;

        exciter.process (first);
        exciter.reset();
        exciter.process (second);

        for (int i = 0; i < second.getNumSamples(); i++)
            expectWithinAbsoluteError (second.getSample (0, i), first.getSample (0, i), 1.0e-6f,
                                       "After reset the same input should give the same output");
    }
};

static ExciterTests exciterTests;

#endif

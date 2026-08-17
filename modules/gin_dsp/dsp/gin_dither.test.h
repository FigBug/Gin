/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DitherTests : public juce::UnitTest
{
public:
    DitherTests() : juce::UnitTest ("Dither", "gin_dsp") {}

    void runTest() override
    {
        testOutputIsQuantised();
        testErrorStaysSmall();
        testDitherDecorrelatesError();
        testNoDitherIsDeterministic();
    }

private:
    static constexpr double sr = 44100.0;

    static juce::AudioSampleBuffer makeSine (int numSamples, float amplitude, float freq = 1000.0f)
    {
        juce::AudioSampleBuffer b (2, numSamples);

        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < numSamples; i++)
                b.setSample (ch, i, amplitude * std::sin (juce::MathConstants<float>::twoPi * freq * float (i) / float (sr)));

        return b;
    }

    void testOutputIsQuantised()
    {
        beginTest ("Output Lands On The Quantisation Grid");

        Dither dither;
        dither.setBitDepth (8);
        dither.setNoiseType (Dither::triangular);

        auto buffer = makeSine (4096, 0.5f);
        dither.process (buffer);

        const float step = 1.0f / std::pow (2.0f, 8.0f);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto v = buffer.getSample (0, i);
            auto steps = v / step;

            expectWithinAbsoluteError (steps - std::round (steps), 0.0f, 1.0e-3f,
                                       "Sample " + juce::String (i) + " was not on the grid");
        }
    }

    void testErrorStaysSmall()
    {
        beginTest ("Error Stays Within A Few LSBs");

        Dither dither;
        dither.setBitDepth (12);
        dither.setNoiseType (Dither::triangular);
        dither.setAmount (2.0f);

        auto original = makeSine (4096, 0.5f);
        auto buffer = original;
        dither.process (buffer);

        const float step = 1.0f / std::pow (2.0f, 12.0f);

        for (int i = 0; i < buffer.getNumSamples(); i++)
            expect (std::abs (buffer.getSample (0, i) - original.getSample (0, i)) < step * 4.0f,
                    "Quantisation error should stay within a few LSBs");
    }

    void testDitherDecorrelatesError()
    {
        beginTest ("Dither Decorrelates The Error");

        // Truncating a sine that only just spans a few quantisation steps
        // produces an error that follows the signal. Dither should break that
        // correlation, which is the entire point of it.
        auto measure = [&] (Dither::NoiseType type)
        {
            Dither dither;
            dither.setBitDepth (6);
            dither.setNoiseType (type);
            dither.setAmount (2.0f);

            auto original = makeSine (8192, 0.02f, 700.0f);
            auto buffer = original;
            dither.process (buffer);

            double dot = 0.0, signalEnergy = 0.0;

            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                auto s = double (original.getSample (0, i));
                auto e = double (buffer.getSample (0, i)) - s;
                dot += s * e;
                signalEnergy += s * s;
            }

            return std::abs (dot) / std::max (1.0e-12, signalEnergy);
        };

        auto truncated = measure (Dither::none);
        auto dithered  = measure (Dither::triangular);

        expectLessThan (dithered, truncated,
                        "Dithered error should correlate with the signal less than plain truncation");
    }

    void testNoDitherIsDeterministic()
    {
        beginTest ("No Dither Is Plain Truncation");

        auto run = [] ()
        {
            Dither dither;
            dither.setBitDepth (10);
            dither.setNoiseType (Dither::none);
            dither.setNoiseShaping (0.0f);

            auto buffer = makeSine (1024, 0.3f);
            dither.process (buffer);
            return buffer;
        };

        auto a = run();
        auto b = run();

        for (int i = 0; i < a.getNumSamples(); i++)
            expectWithinAbsoluteError (a.getSample (0, i), b.getSample (0, i), 0.0f,
                                       "Without noise the result should be repeatable");
    }
};

static DitherTests ditherTests;

#endif

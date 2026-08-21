/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class BandLimitedOscillatorTests : public juce::UnitTest
{
public:
    BandLimitedOscillatorTests() : juce::UnitTest ("Band Limited Oscillators", "gin_dsp") {}

    void runTest() override
    {
        testSine();
        testSawUpDownAreOpposite();
        testTriangleShape();
        testPulseMatchesSquare();
        testOutputBounded();
        testTablePhaseAlignment();
        testPhaseWrapBoundary();
        testMipMapSelection();
        testSquareNotAliased();
        testFFTLoadKeepsAmplitude();
    }

private:
    void testSine()
    {
        beginTest ("Sine");

        expectWithinAbsoluteError (sine (0.0f),  0.0f, 1.0e-6f, "sine(0)");
        expectWithinAbsoluteError (sine (0.25f), 1.0f, 1.0e-6f, "sine(0.25)");
        expectWithinAbsoluteError (sine (0.5f),  0.0f, 1.0e-6f, "sine(0.5)");
        expectWithinAbsoluteError (sine (0.75f), -1.0f, 1.0e-6f, "sine(0.75)");
    }

    void testSawUpDownAreOpposite()
    {
        beginTest ("Saw Up Is Negated Saw Down");

        const float freq = 100.0f, sr = 44100.0f;
        for (float p = 0.0f; p < 1.0f; p += 0.1f)
            expectWithinAbsoluteError (sawUp (p, freq, sr), -sawDown (p, freq, sr), 1.0e-5f, "sawUp == -sawDown");
    }

    void testTriangleShape()
    {
        beginTest ("Triangle Shape");

        const float freq = 100.0f, sr = 44100.0f;
        expectWithinAbsoluteError (triangle (0.0f, freq, sr), 0.0f, 1.0e-4f, "triangle(0) is zero crossing");
        expectWithinAbsoluteError (triangle (0.25f, freq, sr), 1.0f, 0.05f, "triangle(0.25) near peak");
        expectWithinAbsoluteError (triangle (0.75f, freq, sr), -1.0f, 0.05f, "triangle(0.75) near trough");
    }

    void testPulseMatchesSquare()
    {
        beginTest ("50% Pulse Matches Square");

        const float freq = 100.0f, sr = 44100.0f;
        for (float p = 0.0f; p < 1.0f; p += 0.1f)
            expectWithinAbsoluteError (pulse (p, 0.5f, freq, sr), squareWave (p, freq, sr), 1.0e-5f, "pulse(0.5) == square");
    }

    void testOutputBounded()
    {
        beginTest ("Output Bounded");

        const float freq = 100.0f, sr = 44100.0f;
        // Band-limited waves ring slightly past +/-1 (Gibbs), so allow a small margin
        for (float p = 0.0f; p < 1.0f; p += 0.01f)
        {
            expect (std::abs (sawUp (p, freq, sr)) < 1.3f, "sawUp bounded");
            expect (std::abs (triangle (p, freq, sr)) < 1.3f, "triangle bounded");
            expect (std::abs (squareWave (p, freq, sr)) < 1.3f, "square bounded");
        }
    }

    void testTablePhaseAlignment()
    {
        beginTest ("Table Phase Alignment");

        // table samples must sit at phase i / tableSize so phase 0.5 lands exactly on a sine zero crossing
        BandLimitedLookupTable table (sine, 44100.0f, 64, 2048);

        expectWithinAbsoluteError (table.getLinear (0, 0.0f),  0.0f, 1.0e-4f, "sine table at 0");
        expectWithinAbsoluteError (table.getLinear (0, 0.25f), 1.0f, 1.0e-4f, "sine table at 0.25");
        expectWithinAbsoluteError (table.getLinear (0, 0.5f),  0.0f, 1.0e-4f, "sine table at 0.5");
        expectWithinAbsoluteError (table.getLinear (0, 0.75f), -1.0f, 1.0e-4f, "sine table at 0.75");
    }

    void testPhaseWrapBoundary()
    {
        beginTest ("Phase Wrap Boundary");

        BandLimitedLookupTable table (sawUp, 44100.0f, 12, 2048);

        // float rounding can produce a phase of exactly 1.0, which must read the wrap sample, not past the end
        expectWithinAbsoluteError (table.getLinear (0, 1.0f), table.getLinear (0, 0.0f), 1.0e-6f, "getLinear at wrap");
        expectWithinAbsoluteError (table.processLinear (60.0f, 1.0f), table.processLinear (60.0f, 0.0f), 1.0e-6f, "processLinear at wrap");
        expectWithinAbsoluteError (table.process (60.0f, 1.0f), table.process (60.0f, 0.0f), 1.0e-6f, "process at wrap");

        // pulse shifts phase internally; 0.24999997 - 0.25 + 1 rounds to exactly 1.0
        BandLimitedLookupTables tables (44100.0);
        for (float p : { 0.0f, 0.24999997f, 0.49999997f, 0.74999997f, 0.99999997f })
        {
            expect (std::abs (tables.processPulse (60.0f, p, 0.5f)) < 1.5f, "pulse bounded at wrap phase");
            expect (std::abs (tables.processSquare (60.0f, p)) < 1.5f, "square bounded at wrap phase");
        }
    }

    void testMipMapSelection()
    {
        beginTest ("Mip Map Selection");

        BandLimitedLookupTable table (sawUp, 44100.0f, 3, 2048);

        // table k is band-limited for the top note it serves, (k + 1) * notesPerTable + 0.5
        expectEquals (table.tableIndexForNote (2.0f),   0, "note 2");
        expectEquals (table.tableIndexForNote (60.0f), 19, "note 60");
        expectEquals (table.tableIndexForNote (96.0f), 31, "note 96");
        expectEquals (table.tableIndexForNote (127.0f), int (table.tables.size() - 1), "note 127 clamped");
    }

    void testSquareNotAliased()
    {
        beginTest ("Square Not Aliased");

        BandLimitedLookupTables tables (44100.0);

        // note 96 (~2154 Hz) allows odd harmonics up to the 9th; anything above bin 11 is aliasing
        constexpr int sz = 2048;
        juce::dsp::FFT fft (11);

        std::vector<float> data (size_t (sz * 2), 0.0f);
        for (int i = 0; i < sz; i++)
            data[size_t (i)] = tables.processSquare (96.0f, float (i) / float (sz));

        fft.performFrequencyOnlyForwardTransform (data.data());

        auto fundamental = data[1];
        expect (fundamental > 0.1f, "fundamental present");

        for (int bin = 12; bin < sz / 2; bin++)
            expect (data[size_t (bin)] < fundamental * 0.01f, "no energy above band limit");
    }

    void testFFTLoadKeepsAmplitude()
    {
        beginTest ("FFT Load Keeps Amplitude");

        // a single-cycle sine only occupies bin 1, so band-limiting must not change its level
        constexpr int sz = 2048;
        juce::AudioSampleBuffer buffer (1, sz);
        for (int i = 0; i < sz; i++)
            buffer.setSample (0, i, std::sin (2.0f * juce::MathConstants<float>::pi * float (i) / float (sz)));

        std::unique_ptr<juce::dsp::FFT> fft;
        BandLimitedLookupTable table;
        table.loadFromBuffer (fft, 44100.0f, buffer, 44100.0f, 12);

        auto& t = table.tableForNote (60.0f);
        auto peak = 0.0f;
        for (auto v : t)
            peak = std::max (peak, std::abs (v));

        expectWithinAbsoluteError (peak, 1.0f, 0.1f, "filtered table keeps full amplitude");
    }
};

static BandLimitedOscillatorTests bandLimitedOscillatorTests;

#endif

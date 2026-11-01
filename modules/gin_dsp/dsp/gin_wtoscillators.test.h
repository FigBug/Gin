//==============================================================================
#if GIN_UNIT_TESTS

class WTOscillatorTests : public juce::UnitTest
{
public:
    WTOscillatorTests() : juce::UnitTest ("WTOscillator", "gin_dsp") {}

    void runTest() override
    {
        testSimple();
        testComplex();
        testCrossfade();
        testRaggedBlocks();
        testNoteOnPhase();
    }

private:
    static constexpr double sampleRate = 44100.0;

    // A register holds mipp::N<float>() samples, four under SSE and NEON and eight under AVX,
    // so the wide blocks are sized well past either
    static constexpr int wideBlock = 64;

    /** One render stage: samples of audio in chunk sized calls, all on the same parameters.
        samples has to be a whole number of chunks.
    */
    struct Stage
    {
        int chunk = 1;
        int samples = 0;
        gin::WTOscillator::Params params;
    };

    static void fillWavetable (gin::Wavetable& wt)
    {
        const auto sine = [] (float phase, float, float) { return std::sin (phase * juce::MathConstants<float>::twoPi); };
        const auto saw  = [] (float phase, float, float) { return 2.0f * phase - 1.0f; };

        wt.addTable (new gin::BandLimitedLookupTable (sine, float (sampleRate)));
        wt.addTable (new gin::BandLimitedLookupTable (saw,  float (sampleRate)));
    }

    static juce::AudioSampleBuffer render (gin::Wavetable& wt, float note, const std::vector<Stage>& stages)
    {
        gin::WTOscillator osc;
        osc.setSampleRate (sampleRate);
        osc.setWavetable (&wt);
        osc.noteOn (0.0f);

        auto total = 0;

        for (auto& s : stages)
            total += s.samples;

        juce::AudioSampleBuffer out (2, total);
        auto pos = 0;

        for (auto& s : stages)
        {
            juce::AudioSampleBuffer scratch (2, s.chunk);

            for (auto done = 0; done < s.samples; done += s.chunk)
            {
                osc.process (note, s.params, scratch);

                for (int ch = 0; ch < 2; ch++)
                    out.copyFrom (ch, pos, scratch, ch, 0, s.chunk);

                pos += s.chunk;
            }
        }

        return out;
    }

    /** One sample per call can only take the scalar path, so rendering the same stretch in wide
        chunks has to agree with it over [from, to): a lane count the SIMD path got wrong, or a
        value it hoisted out of a register, shows up as a mismatch
    */
    void expectWideMatchesScalar (float note, const std::vector<Stage>& wide, int from, int to, const juce::String& what)
    {
        auto scalar = wide;

        for (auto& s : scalar)
            s.chunk = 1;

        gin::Wavetable wtWide, wtScalar;
        fillWavetable (wtWide);
        fillWavetable (wtScalar);

        const auto wideOut = render (wtWide, note, wide);
        const auto scalarOut = render (wtScalar, note, scalar);

        auto peak = 0.0f;
        auto worst = 0.0f;
        auto worstAt = -1;

        for (int ch = 0; ch < 2; ch++)
        {
            for (int i = from; i < to; i++)
            {
                peak = std::max (peak, std::abs (scalarOut.getSample (ch, i)));

                const auto error = std::abs (wideOut.getSample (ch, i) - scalarOut.getSample (ch, i));

                if (error > worst)
                {
                    worst = error;
                    worstAt = i;
                }
            }
        }

        expect (peak > 0.1f, what + ": the oscillator made sound to compare");
        expect (worst < 0.001f, what + ": wide blocks match one sample at a time, worst "
                                     + juce::String (worst) + " at sample " + juce::String (worstAt));
    }

    void testSimple()
    {
        beginTest ("Simple Matches Scalar");

        gin::WTOscillator::Params params;
        params.position = 0.25f;

        expectWideMatchesScalar (69.0f, { { wideBlock, wideBlock * 4, params } }, 0, wideBlock * 4, "Simple");
    }

    void testComplex()
    {
        beginTest ("Complex Matches Scalar");

        // Any bend or formant takes the phase distorting path
        gin::WTOscillator::Params params;
        params.position = 0.25f;
        params.bend = 0.3f;
        params.formant = 0.2f;
        params.asym = 0.4f;
        params.fold = 0.2f;

        expectWideMatchesScalar (69.0f, { { wideBlock, wideBlock * 4, params } }, 0, wideBlock * 4, "Complex");
    }

    void testCrossfade()
    {
        beginTest ("Crossfade Matches Scalar");

        // A wrap while the position names another table leaves the two table indices apart,
        // which arms the crossfading path. processAdding picks its path once per call, so the
        // arming runs a sample at a time in both renderings and the window compared after it is
        // short enough not to wrap again and send them down different paths.
        gin::WTOscillator::Params before;
        before.position = 0.0f;
        before.bend = 0.3f;

        auto after = before;
        after.position = 1.0f;

        // 27.5Hz, so a cycle is a little over 1600 samples and the window sits inside one
        const auto note = 21.0f;
        const auto arm = 1500 + 200;

        expectWideMatchesScalar (note, { { 1, 1500, before }, { 1, 200, after }, { wideBlock, wideBlock * 4, after } },
                                 arm, arm + wideBlock * 4, "Crossfade");
    }

    void testNoteOnPhase()
    {
        beginTest ("Note On Phase");

        gin::Wavetable wt;
        fillWavetable (wt);

        gin::WTOscillator::Params params;
        params.position = 0.0f;   // the sine table

        // One sample with no DC blocking is the table read straight out, so where the phase
        // started is visible in it
        const auto firstSample = [&] (float startPhase)
        {
            gin::WTOscillator osc;
            osc.setSampleRate (sampleRate);
            osc.setWavetable (&wt);
            osc.setBlockDC (false);

            startPhase >= 0.0f ? osc.noteOn (startPhase) : osc.noteOn();

            juce::AudioSampleBuffer one (2, 1);
            osc.process (69.0f, params, one);

            return one.getSample (0, 0);
        };

        expectWithinAbsoluteError (firstSample (0.0f),   0.0f, 0.05f, "A phase of zero starts the sine at zero");
        expectWithinAbsoluteError (firstSample (0.25f),  1.0f, 0.05f, "A quarter turn starts it at the peak");
        expectWithinAbsoluteError (firstSample (0.5f),   0.0f, 0.05f, "A half turn starts it back at zero");
        expectWithinAbsoluteError (firstSample (0.75f), -1.0f, 0.05f, "Three quarters starts it at the trough");

        // Asking for no particular phase randomises it, so voices don't all start together
        const auto first = firstSample (-1.0f);
        auto distinct = false;

        for (int i = 0; i < 8 && ! distinct; i++)
            distinct = std::abs (firstSample (-1.0f) - first) > 1.0e-6f;

        expect (distinct, "No phase given starts somewhere random");
    }

    void testRaggedBlocks()
    {
        beginTest ("Ragged Blocks Match Scalar");

        gin::WTOscillator::Params params;
        params.position = 0.25f;

        // A block that isn't a whole number of registers leaves a tail for the scalar loop
        for (auto chunk : { 3, 5, 7, 11, 13 })
            expectWideMatchesScalar (69.0f, { { chunk, chunk * 16, params } }, 0, chunk * 16,
                                     "Chunk " + juce::String (chunk));
    }
};

static WTOscillatorTests wtOscillatorTests;

#endif

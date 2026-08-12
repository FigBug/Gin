/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class BitCrusherTests : public juce::UnitTest
{
public:
    BitCrusherTests() : juce::UnitTest ("BitCrusher", "gin_dsp") {}

    void runTest() override
    {
        testBasicProcessing();
        testQuantisationGrid();
        testSampleAndHold();
        testAntiAliasing();
        testLatency();
        testReset();
    }

private:
    static void fillSine (juce::AudioBuffer<float>& buffer, float freq, float sr, float gain = 0.5f)
    {
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float sample = std::sin (2.0f * juce::MathConstants<float>::pi * freq * float (i) / sr) * gain;
            for (int ch = 0; ch < buffer.getNumChannels(); ch++)
                buffer.setSample (ch, i, sample);
        }
    }

    static bool isFinite (const juce::AudioBuffer<float>& buffer)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ch++)
            for (int i = 0; i < buffer.getNumSamples(); i++)
                if (! std::isfinite (buffer.getSample (ch, i)))
                    return false;

        return true;
    }

    void testBasicProcessing()
    {
        beginTest ("Basic Processing");

        BitCrusher crusher;
        crusher.setSampleRate (44100.0);
        crusher.setParams (8.0f, 6000.0f, false);

        juce::AudioBuffer<float> buffer (2, 512);
        fillSine (buffer, 441.0f, 44100.0f);

        crusher.process (buffer);

        expect (isFinite (buffer), "Output should be finite");
        expect (buffer.getRMSLevel (0, 0, 512) > 0.0f, "Should produce output");
    }

    void testQuantisationGrid()
    {
        beginTest ("Quantisation Grid");

        BitCrusher crusher;
        crusher.setSampleRate (44100.0);
        crusher.setParams (8.0f, 44100.0f, true); // dirty, hold every sample
        crusher.reset(); // snap parameter smoothing

        juce::AudioBuffer<float> buffer (2, 512);
        fillSine (buffer, 441.0f, 44100.0f);

        crusher.process (buffer);

        // in dirty mode every output value must sit exactly on the 8 bit grid
        bool onGrid = true;
        float scale = std::exp2 (7.0f);

        for (int i = 0; i < 512; i++)
        {
            float v = buffer.getSample (0, i) * scale;
            if (std::abs (v - std::round (v)) > 0.001f)
                onGrid = false;
        }

        expect (onGrid, "Dirty mode output should be quantised to the bit grid");
    }

    void testSampleAndHold()
    {
        beginTest ("Sample And Hold");

        BitCrusher crusher;
        crusher.setSampleRate (44100.0);
        crusher.setParams (16.0f, 1000.0f, true); // dirty, ~44 sample holds
        crusher.reset(); // snap parameter smoothing

        juce::AudioBuffer<float> buffer (1, 4410);
        fillSine (buffer, 100.0f, 44100.0f);

        crusher.process (buffer);

        // count hold transitions: should be close to 1000Hz * 0.1s = 100
        int transitions = 0;
        for (int i = 1; i < 4410; i++)
            if (! juce::approximatelyEqual (buffer.getSample (0, i), buffer.getSample (0, i - 1)))
                transitions++;

        expect (transitions > 80 && transitions < 120, "Hold rate should follow the rate parameter, got "
                                                        + juce::String (transitions) + " transitions");
    }

    void testAntiAliasing()
    {
        beginTest ("Anti Aliasing");

        // sample & hold of a sine creates images at k * holdHz +/- sineHz.
        // Images below Nyquist are the legitimate stepped character and are
        // preserved by BLEP; images above Nyquist fold back to inharmonic
        // frequencies, and it's that fold-back the BLEP should suppress.
        // With a 1kHz sine held at 6kHz in 44.1kHz, images at 23k, 29k, 35k
        // and 41k fold to 21.1k, 15.1k, 9.1k and 3.1k - all well away from
        // any legitimate image.
        const double sr = 44100.0;
        const float sineHz = 1000.0f;
        const float holdHz = 6000.0f;
        const int order = 12;
        const int n = 1 << order; // 4096

        auto imageEnergy = [&] (bool dirtyMode)
        {
            BitCrusher crusher;
            crusher.setSampleRate (sr);
            crusher.setParams (16.0f, holdHz, dirtyMode);
            crusher.reset(); // snap parameter smoothing

            juce::AudioBuffer<float> buffer (1, n * 2);
            fillSine (buffer, sineHz, float (sr));
            crusher.process (buffer);

            // FFT the second half, past any transients / latency
            juce::dsp::FFT fft (order);
            std::vector<float> data (size_t (n * 2), 0.0f);

            for (int i = 0; i < n; i++)
            {
                // Hann window
                float w = 0.5f - 0.5f * std::cos (2.0f * juce::MathConstants<float>::pi * float (i) / float (n - 1));
                data[size_t (i)] = buffer.getSample (0, n + i) * w;
            }

            fft.performFrequencyOnlyForwardTransform (data.data());

            // sum energy around the folded (aliased) image frequencies
            auto bin = [&] (float hz) { return int (hz / float (sr) * float (n)); };

            float energy = 0.0f;
            for (float hz : { 21100.0f, 15100.0f, 9100.0f, 3100.0f })
                for (int i = bin (hz) - 3; i <= bin (hz) + 3; i++)
                    energy += data[size_t (i)] * data[size_t (i)];

            return energy;
        };

        float naive = imageEnergy (true);
        float blep  = imageEnergy (false);

        expect (naive > 0.0f, "Naive sample & hold should produce fold-back aliasing");
        expect (blep < naive * 0.5f, "BLEP should reduce fold-back energy, naive "
                                      + juce::String (naive) + " vs blep " + juce::String (blep));
    }

    void testLatency()
    {
        beginTest ("Latency");

        BitCrusher crusher;
        crusher.setSampleRate (44100.0);

        crusher.setParams (16.0f, 44100.0f, true);
        expectEquals (crusher.getLatencySamples(), 0, "Dirty mode should have no latency");

        crusher.setParams (16.0f, 44100.0f, false);
        expect (crusher.getLatencySamples() > 0, "Anti-aliased mode should report latency");
    }

    void testReset()
    {
        beginTest ("Reset");

        BitCrusher crusher;
        crusher.setSampleRate (44100.0);
        crusher.setParams (4.0f, 500.0f, false);

        juce::AudioBuffer<float> buffer (2, 512);
        fillSine (buffer, 441.0f, 44100.0f);
        crusher.process (buffer);

        crusher.reset();

        fillSine (buffer, 441.0f, 44100.0f);
        crusher.process (buffer);

        expect (isFinite (buffer), "Should process cleanly after reset");
    }
};

static BitCrusherTests bitCrusherTests;

#endif

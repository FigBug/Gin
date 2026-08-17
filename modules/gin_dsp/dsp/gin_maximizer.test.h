/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class MaximizerTests : public juce::UnitTest
{
public:
    MaximizerTests() : juce::UnitTest ("Maximizer", "gin_dsp") {}

    void runTest() override
    {
        testLatencyMatchesCycleLength();
        testHoldsCeiling();
        testQuietSignalOnlyGetsMakeup();
        testDelaysByLatency();
    }

private:
    static constexpr double sr = 44100.0;

    static void fillSine (juce::AudioSampleBuffer& b, float freq, float amplitude)
    {
        for (int ch = 0; ch < b.getNumChannels(); ch++)
            for (int i = 0; i < b.getNumSamples(); i++)
                b.setSample (ch, i, amplitude * std::sin (juce::MathConstants<float>::twoPi * freq * float (i) / float (sr)));
    }

    void testLatencyMatchesCycleLength()
    {
        beginTest ("Latency Matches Max Cycle Length");

        Maximizer max;
        max.setNumChannels (2);
        max.setSampleRate (sr);
        max.setMaxCycleLength (0.05f);

        expectEquals (max.getLatencySamples(), int (0.05 * sr) - 1, "Latency should be one short of the buffer length");
    }

    void testHoldsCeiling()
    {
        beginTest ("Loud Signal Is Held At The Ceiling");

        Maximizer max;
        max.setNumChannels (2);
        max.setSampleRate (sr);
        max.setThreshold (-6.0f);
        max.setCeiling (-1.0f);
        max.setRelease (0.0f);

        juce::AudioSampleBuffer buffer (2, int (sr));
        fillSine (buffer, 220.0f, 1.0f);   // well above the -6 dB threshold

        max.process (buffer);

        // Skip the latency, where the output is still the initial silence
        auto from = max.getLatencySamples() + 1000;
        auto peak = buffer.getMagnitude (0, from, buffer.getNumSamples() - from);
        auto ceiling = juce::Decibels::decibelsToGain (-1.0f);

        expect (peak <= ceiling * 1.05f, "Peaks should be held at the ceiling, got "
                                         + juce::String (juce::Decibels::gainToDecibels (peak)) + " dB");
        expect (peak > ceiling * 0.7f, "And should actually reach it");
    }

    void testQuietSignalOnlyGetsMakeup()
    {
        beginTest ("Quiet Signal Only Gets Makeup Gain");

        Maximizer max;
        max.setNumChannels (2);
        max.setSampleRate (sr);
        max.setThreshold (-6.0f);
        max.setCeiling (-6.0f);   // makeup of 1, so nothing should change at all
        max.setRelease (0.0f);

        juce::AudioSampleBuffer buffer (2, int (sr));
        fillSine (buffer, 220.0f, 0.1f);  // -20 dB, never reaches the threshold

        auto expected = buffer;
        max.process (buffer);

        auto latency = max.getLatencySamples();

        for (int i = latency; i < buffer.getNumSamples(); i++)
            expectWithinAbsoluteError (buffer.getSample (0, i), expected.getSample (0, i - latency), 1.0e-6f,
                                       "Below the threshold nothing should be scaled");
    }

    void testDelaysByLatency()
    {
        beginTest ("Output Is Delayed By The Reported Latency");

        Maximizer max;
        max.setNumChannels (1);
        max.setSampleRate (sr);
        max.setMaxCycleLength (0.01f);
        max.setThreshold (0.0f);
        max.setCeiling (0.0f);
        max.setRelease (0.0f);

        auto latency = max.getLatencySamples();

        juce::AudioSampleBuffer buffer (1, latency * 3);
        buffer.clear();
        buffer.setSample (0, 5, 0.5f);

        max.process (buffer);

        expectWithinAbsoluteError (buffer.getSample (0, 5 + latency), 0.5f, 1.0e-6f,
                                   "The impulse should come out one buffer later");
    }
};

static MaximizerTests maximizerTests;

#endif

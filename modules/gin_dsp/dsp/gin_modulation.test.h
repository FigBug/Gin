/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ModulationTests : public juce::UnitTest
{
public:
    ModulationTests() : juce::UnitTest ("Modulation", "gin_dsp") {}

    void runTest() override
    {
        testBasicProcessing();
        testDryMix();
        testWetMix();
        testReset();
    }

private:
    void testBasicProcessing()
    {
        beginTest ("Basic Processing");

        Modulation mod (0.05f); // 50ms max delay
        mod.setSampleRate (44100.0);
        mod.setParams (0.02f, 1.0f, 0.01f, 0.5f, 0.5f);

        juce::AudioBuffer<float> buffer (2, 512);
        buffer.clear();

        // Put a signal in
        for (int i = 0; i < 512; i++)
        {
            buffer.setSample (0, i, std::sin (2.0f * juce::MathConstants<float>::pi * i / 100.0f));
            buffer.setSample (1, i, std::sin (2.0f * juce::MathConstants<float>::pi * i / 100.0f));
        }

        float originalRMS = buffer.getRMSLevel (0, 0, 512);
        mod.process (buffer);

        // Should still have signal output
        float outputRMS = buffer.getRMSLevel (0, 0, 512);
        expect (outputRMS > 0.0f, "Should produce output");
        expect (outputRMS > originalRMS * 0.1f, "Output shouldn't be drastically reduced");
    }

    void testDryMix()
    {
        beginTest ("Dry Mix");

        Modulation mod (0.05f);
        mod.setSampleRate (44100.0);
        mod.setParams (0.02f, 1.0f, 0.01f, 0.5f, 0.0f); // 100% dry

        juce::AudioBuffer<float> buffer (2, 256);

        // Put known signal
        for (int i = 0; i < 256; i++)
        {
            buffer.setSample (0, i, 1.0f);
            buffer.setSample (1, i, 1.0f);
        }

        mod.process (buffer);

        // With 0% wet, output should be unchanged
        for (int i = 0; i < 256; i++)
        {
            expectWithinAbsoluteError (buffer.getSample (0, i), 1.0f, 0.001f, "Dry signal should pass through");
        }
    }

    void testWetMix()
    {
        beginTest ("Wet Mix");

        Modulation mod (0.05f);
        mod.setSampleRate (44100.0);
        mod.setParams (0.02f, 1.0f, 0.01f, 0.5f, 1.0f); // 100% wet

        juce::AudioBuffer<float> buffer (2, 2048);

        // Put impulse at start
        buffer.clear();
        buffer.setSample (0, 0, 1.0f);
        buffer.setSample (1, 0, 1.0f);

        mod.process (buffer);

        // With delay, impulse should be shifted later
        float sample0 = buffer.getSample (0, 0);
        expectWithinAbsoluteError (sample0, 0.0f, 0.01f, "First sample should be nearly zero with wet mix");

        // Impulse should appear later due to delay
        bool foundImpulse = false;
        for (int i = 100; i < 1500; i++)
        {
            if (std::abs (buffer.getSample (0, i)) > 0.1f)
            {
                foundImpulse = true;
                break;
            }
        }
        expect (foundImpulse, "Delayed impulse should appear");
    }

    void testReset()
    {
        beginTest ("Reset");

        Modulation mod (0.05f);
        mod.setSampleRate (44100.0);
        mod.setParams (0.02f, 1.0f, 0.01f, 0.5f, 1.0f);

        // Process some audio
        juce::AudioBuffer<float> buffer (2, 256);
        for (int i = 0; i < 256; i++)
        {
            buffer.setSample (0, i, 1.0f);
            buffer.setSample (1, i, 1.0f);
        }
        mod.process (buffer);

        // Reset
        mod.reset();

        // After reset, processing an impulse should give same result as fresh start
        juce::AudioBuffer<float> buffer2 (2, 256);
        buffer2.clear();
        buffer2.setSample (0, 0, 1.0f);
        buffer2.setSample (1, 0, 1.0f);

        mod.process (buffer2);

        // First sample should be near zero (delayed)
        expectWithinAbsoluteError (buffer2.getSample (0, 0), 0.0f, 0.01f, "Reset should clear delay line");
    }
};

static ModulationTests modulationTests;

#endif

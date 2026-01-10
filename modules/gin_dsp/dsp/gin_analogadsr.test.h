/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AnalogADSRTests : public juce::UnitTest
{
public:
    AnalogADSRTests() : juce::UnitTest ("Analog ADSR", "gin_dsp") {}

    void runTest() override
    {
        testInitialState();
        testAttackPhase();
        testDecayPhase();
        testSustainPhase();
        testReleasePhase();
        testInstantAttack();
        testInstantRelease();
        testProcessBuffer();
        testProcessMultiplying();
        testRetrigger();
    }

private:
    void testInitialState()
    {
        beginTest ("Analog ADSR - Initial State");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);

        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::idle,
                     "Should start in idle state");
        expectEquals (adsr.getOutput(), 0.0f,
                     "Should start with zero output");
    }

    void testAttackPhase()
    {
        beginTest ("Analog ADSR - Attack Phase");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.1f); // 100ms attack
        adsr.setDecay (0.0f);
        adsr.setSustainLevel (1.0f);
        adsr.setRelease (0.1f);
        adsr.reset();

        adsr.noteOn();
        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::attack,
                     "Should enter attack state on noteOn");

        // Process some samples during attack
        float lastValue = adsr.getOutput();
        for (int i = 0; i < 100; i++)
        {
            float value = adsr.process();
            expect (value >= lastValue, "Attack should rise");
            lastValue = value;
        }

        // Process until attack completes (approximately 4410 samples at 44100Hz for 0.1s)
        for (int i = 0; i < 5000; i++)
            adsr.process();

        expect (adsr.getState() != AnalogADSR::State::attack,
               "Should exit attack state");
        expectWithinAbsoluteError (adsr.getOutput(), 1.0f, 0.01f,
                                  "Should reach peak after attack");
    }

    void testDecayPhase()
    {
        beginTest ("Analog ADSR - Decay Phase");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f); // Instant attack
        adsr.setDecay (0.1f); // 100ms decay
        adsr.setSustainLevel (0.5f);
        adsr.setRelease (0.1f);
        adsr.reset();

        adsr.noteOn();

        // Should go directly to decay since attack is 0
        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::decay,
                     "Should enter decay with instant attack");
        expectWithinAbsoluteError (adsr.getOutput(), 1.0f, 0.01f,
                                  "Should start at peak");

        // Process during decay
        float lastValue = adsr.getOutput();
        for (int i = 0; i < 100; i++)
        {
            float value = adsr.process();
            expect (value <= lastValue, "Decay should fall");
            lastValue = value;
        }

        // Process until decay completes
        for (int i = 0; i < 5000; i++)
            adsr.process();

        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::sustain,
                     "Should enter sustain after decay");
        expectWithinAbsoluteError (adsr.getOutput(), 0.5f, 0.01f,
                                  "Should reach sustain level");
    }

    void testSustainPhase()
    {
        beginTest ("Analog ADSR - Sustain Phase");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f);
        adsr.setDecay (0.0f); // Instant decay
        adsr.setSustainLevel (0.7f);
        adsr.setRelease (0.1f);
        adsr.reset();

        adsr.noteOn();

        // Process to reach sustain
        for (int i = 0; i < 100; i++)
            adsr.process();

        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::sustain,
                     "Should be in sustain");

        // Sustain should hold level
        float sustainValue = adsr.getOutput();
        for (int i = 0; i < 1000; i++)
        {
            float value = adsr.process();
            expectWithinAbsoluteError (value, sustainValue, 0.001f,
                                      "Sustain should hold steady");
        }
    }

    void testReleasePhase()
    {
        beginTest ("Analog ADSR - Release Phase");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f);
        adsr.setDecay (0.0f);
        adsr.setSustainLevel (0.8f);
        adsr.setRelease (0.1f); // 100ms release
        adsr.reset();

        adsr.noteOn();
        adsr.process (100); // Get to sustain

        adsr.noteOff();
        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::release,
                     "Should enter release on noteOff");

        // Process during release
        float lastValue = adsr.getOutput();
        for (int i = 0; i < 100; i++)
        {
            float value = adsr.process();
            expect (value <= lastValue, "Release should fall");
            lastValue = value;
        }

        // Process until release completes
        for (int i = 0; i < 6000; i++)
            adsr.process();

        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::idle,
                     "Should return to idle after release");
        expectWithinAbsoluteError (adsr.getOutput(), 0.0f, 0.01f,
                                  "Should reach zero after release");
    }

    void testInstantAttack()
    {
        beginTest ("Analog ADSR - Instant Attack");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f); // Instant
        adsr.setDecay (0.1f);
        adsr.setSustainLevel (0.5f);
        adsr.setRelease (0.1f);
        adsr.reset();

        adsr.noteOn();

        // With instant attack, should go straight to decay
        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::decay,
                     "Instant attack should skip to decay");
        expectWithinAbsoluteError (adsr.getOutput(), 1.0f, 0.01f,
                                  "Should immediately be at peak");
    }

    void testInstantRelease()
    {
        beginTest ("Analog ADSR - Instant Release");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f);
        adsr.setDecay (0.0f);
        adsr.setSustainLevel (0.8f);
        adsr.setRelease (0.0f); // Instant release
        adsr.reset();

        adsr.noteOn();
        adsr.process (100);

        float sustainLevel = adsr.getOutput();
        expect (sustainLevel > 0.7f, "Should be at sustain");

        adsr.noteOff();
        adsr.process();

        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::idle,
                     "Instant release should go to idle immediately");
        expectWithinAbsoluteError (adsr.getOutput(), 0.0f, 0.01f,
                                  "Should be at zero after instant release");
    }

    void testProcessBuffer()
    {
        beginTest ("Analog ADSR - Process Buffer");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.01f);
        adsr.setDecay (0.01f);
        adsr.setSustainLevel (0.6f);
        adsr.setRelease (0.01f);
        adsr.reset();

        juce::AudioBuffer<float> buffer (1, 1000);
        buffer.clear();

        adsr.noteOn();
        adsr.process (buffer);

        // Check that envelope values were written
        bool hasNonZero = false;
        bool hasVariation = false;
        float lastValue = buffer.getSample (0, 0);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            float value = buffer.getSample (0, i);
            if (value > 0.01f)
                hasNonZero = true;
            if (std::abs (value - lastValue) > 0.001f)
                hasVariation = true;

            expect (value >= 0.0f && value <= 1.0f,
                   "Envelope values should be in range [0, 1]");
            lastValue = value;
        }

        expect (hasNonZero, "Buffer should have non-zero values");
        expect (hasVariation, "Envelope should vary over time");
    }

    void testProcessMultiplying()
    {
        beginTest ("Analog ADSR - Process Multiplying");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.0f);
        adsr.setDecay (0.0f);
        adsr.setSustainLevel (0.5f);
        adsr.setRelease (0.1f);
        adsr.reset();

        juce::AudioBuffer<float> buffer (2, 100);
        buffer.clear();

        // Fill with test signal
        for (int c = 0; c < buffer.getNumChannels(); c++)
            for (int s = 0; s < buffer.getNumSamples(); s++)
                buffer.setSample (c, s, 1.0f);

        adsr.noteOn();
        adsr.processMultiplying (buffer);

        // Check that signal was multiplied by envelope
        for (int c = 0; c < buffer.getNumChannels(); c++)
        {
            for (int s = 0; s < buffer.getNumSamples(); s++)
            {
                float value = buffer.getSample (c, s);
                expect (value >= 0.0f && value <= 1.0f,
                       "Multiplied values should be attenuated");
            }
        }

        // At sustain level of 0.5, signal should be halved
        float avgLevel = 0.0f;
        for (int s = buffer.getNumSamples() - 10; s < buffer.getNumSamples(); s++)
            avgLevel += buffer.getSample (0, s);
        avgLevel /= 10.0f;

        expectWithinAbsoluteError (avgLevel, 0.5f, 0.1f,
                                  "Signal should be multiplied by sustain level");
    }

    void testRetrigger()
    {
        beginTest ("Analog ADSR - Retrigger");

        AnalogADSR adsr;
        adsr.setSampleRate (44100.0);
        adsr.setAttack (0.05f);
        adsr.setDecay (0.05f);
        adsr.setSustainLevel (0.5f);
        adsr.setRelease (0.05f);
        adsr.reset();

        // First note
        adsr.noteOn();
        adsr.process (1000); // Partway through envelope

        float midEnvelopeValue = adsr.getOutput();
        expect (midEnvelopeValue > 0.0f, "Should have some output");

        // Retrigger before release
        adsr.noteOn();

        // Should restart attack from current level or zero depending on implementation
        expectEquals ((int)adsr.getState(), (int)AnalogADSR::State::attack,
                     "Should be in attack after retrigger");

        // Process and verify envelope continues
        for (int i = 0; i < 5000; i++)
            adsr.process();

        expect (adsr.getState() == AnalogADSR::State::sustain ||
                adsr.getState() == AnalogADSR::State::decay,
               "Should progress through envelope after retrigger");
    }
};

static AnalogADSRTests analogADSRTests;

#endif

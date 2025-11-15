/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ADSRTests : public juce::UnitTest
{
public:
    ADSRTests() : juce::UnitTest ("ADSR Envelope", "gin_dsp") {}

    void runTest() override
    {
        testBasicEnvelope();
        testInstantAttack();
        testInstantRelease();
        testSustainLevel();
        testStateTransitions();
        testBufferProcessing();
    }

private:
    void testBasicEnvelope()
    {
        beginTest ("Basic Envelope");

        ADSR env;
        env.setSampleRate (1000.0);  // 1000 Hz for easy calculation
        env.setAttack (0.1f);         // 100 samples
        env.setDecay (0.1f);          // 100 samples
        env.setSustainLevel (0.5f);
        env.setRelease (0.1f);        // 100 samples

        // Initially idle
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::idle), "Should start in idle state");
        expectWithinAbsoluteError (env.getOutput(), 0.0f, 0.001f, "Output should be 0 when idle");

        // Start attack
        env.noteOn();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::attack), "Should be in attack state");

        // Process attack phase
        for (int i = 0; i < 100; i++)
            env.process();

        expectWithinAbsoluteError (env.getOutput(), 1.0f, 0.01f, "Should reach 1.0 after attack");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::decay), "Should transition to decay");

        // Process decay phase
        for (int i = 0; i < 100; i++)
            env.process();

        expectWithinAbsoluteError (env.getOutput(), 0.5f, 0.01f, "Should reach sustain level");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::sustain), "Should be in sustain state");

        // Release
        env.noteOff();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::release), "Should be in release state");

        for (int i = 0; i < 100; i++)
            env.process();

        expectWithinAbsoluteError (env.getOutput(), 0.0f, 0.01f, "Should reach 0 after release");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::finished), "Should be finished");
    }

    void testInstantAttack()
    {
        beginTest ("Instant Attack");

        ADSR env;
        env.setSampleRate (44100.0);
        env.setAttack (0.0f);  // Instant attack
        env.setDecay (0.1f);
        env.setSustainLevel (0.7f);
        env.setRelease (0.1f);

        env.noteOn();
        env.process();

        expectWithinAbsoluteError (env.getOutput(), 1.0f, 0.001f, "Instant attack should jump to 1.0");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::decay), "Should immediately transition to decay");
    }

    void testInstantRelease()
    {
        beginTest ("Instant Release");

        ADSR env;
        env.setSampleRate (44100.0);
        env.setAttack (0.1f);
        env.setDecay (0.1f);
        env.setSustainLevel (0.5f);
        env.setRelease (0.0f);  // Instant release

        env.noteOn();
        for (int i = 0; i < 10000; i++)
            env.process();

        // Should be in sustain
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::sustain), "Should be in sustain");

        env.noteOff();
        env.process();

        expectWithinAbsoluteError (env.getOutput(), 0.0f, 0.001f, "Instant release should jump to 0");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::finished), "Should be finished");
    }

    void testSustainLevel()
    {
        beginTest ("Sustain Level");

        ADSR env;
        env.setSampleRate (1000.0);
        env.setAttack (0.05f);
        env.setDecay (0.05f);
        env.setSustainLevel (0.3f);
        env.setRelease (0.05f);

        env.noteOn();

        // Process through attack and decay
        for (int i = 0; i < 200; i++)
            env.process();

        // Should be at sustain level
        expectWithinAbsoluteError (env.getOutput(), 0.3f, 0.01f, "Should be at sustain level");
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::sustain), "Should be in sustain state");

        // Sustain should hold
        for (int i = 0; i < 1000; i++)
            env.process();

        expectWithinAbsoluteError (env.getOutput(), 0.3f, 0.001f, "Should stay at sustain level");
    }

    void testStateTransitions()
    {
        beginTest ("State Transitions");

        ADSR env;
        env.setSampleRate (1000.0);
        env.setAttack (0.01f);
        env.setDecay (0.01f);
        env.setSustainLevel (0.5f);
        env.setRelease (0.01f);

        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::idle), "Initial state should be idle");

        env.noteOn();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::attack), "noteOn should trigger attack");

        // Process through attack
        for (int i = 0; i < 10; i++)
            env.process();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::decay), "Should transition to decay");

        // Process through decay
        for (int i = 0; i < 10; i++)
            env.process();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::sustain), "Should transition to sustain");

        env.noteOff();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::release), "noteOff should trigger release");

        // Process through release
        for (int i = 0; i < 10; i++)
            env.process();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::finished), "Should transition to finished");

        env.reset();
        expectEquals (static_cast<int> (env.getState()), static_cast<int> (ADSR::State::idle), "reset should return to idle");
    }

    void testBufferProcessing()
    {
        beginTest ("Buffer Processing");

        ADSR env;
        env.setSampleRate (1000.0);
        env.setAttack (0.1f);
        env.setDecay (0.1f);
        env.setSustainLevel (0.5f);
        env.setRelease (0.1f);

        juce::AudioBuffer<float> buffer (1, 100);
        buffer.clear();

        env.noteOn();
        env.process (buffer);

        // First sample should be close to 0
        expect (buffer.getSample (0, 0) >= 0.0f, "First sample should be >= 0");

        // Last sample should be greater (attack phase)
        expect (buffer.getSample (0, 99) > buffer.getSample (0, 0),
                "Buffer should show increasing values during attack");

        // All values should be in valid range
        for (int i = 0; i < 100; i++)
        {
            float val = buffer.getSample (0, i);
            expect (val >= 0.0f && val <= 1.0f, "All envelope values should be [0, 1]");
        }
    }
};

static ADSRTests adsrTests;

#endif

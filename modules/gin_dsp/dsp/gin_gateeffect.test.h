/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class GateEffectTests : public juce::UnitTest
{
public:
    GateEffectTests() : juce::UnitTest ("GateEffect", "gin_dsp") {}

    void runTest() override
    {
        testGatePattern();
        testNoStepsIsNoOp();
    }

private:
    void testGatePattern()
    {
        beginTest ("Gate Pattern Passes And Blocks Channels");

        GateEffect gate;
        gate.setSampleRate (1000.0);
        gate.setLength (1);
        gate.setStep (0, true, false); // left always on, right always off
        gate.setFrequency (1.0f);
        gate.setAttack (0.001f);
        gate.setRelease (0.001f);
        gate.setMix (1.0f);
        gate.reset();

        juce::AudioSampleBuffer buffer (2, 1000);
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < buffer.getNumSamples(); i++)
                buffer.setSample (ch, i, 1.0f);

        gate.process (buffer, 0, -1); // note on at sample 0, never off

        // Left channel gate is on: after the attack settles the signal passes through
        expectWithinAbsoluteError (buffer.getSample (0, buffer.getNumSamples() - 1), 1.0f, 0.05f, "Left passes when gated on");

        // Right channel gate is always off: output should be silent
        for (int i = 0; i < buffer.getNumSamples(); i++)
            expect (std::abs (buffer.getSample (1, i)) < 1.0e-4f, "Right silent when gated off");
    }

    void testNoStepsIsNoOp()
    {
        beginTest ("No Steps Leaves Buffer Unchanged");

        GateEffect gate;
        gate.setSampleRate (1000.0);
        gate.setFrequency (4.0f);
        gate.reset();

        juce::AudioSampleBuffer buffer (2, 32);
        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < buffer.getNumSamples(); i++)
                buffer.setSample (ch, i, 0.5f);

        gate.process (buffer, 0, -1);

        for (int ch = 0; ch < 2; ch++)
            for (int i = 0; i < buffer.getNumSamples(); i++)
                expectWithinAbsoluteError (buffer.getSample (ch, i), 0.5f, 1.0e-6f, "Unchanged with no steps");
    }
};

static GateEffectTests gateEffectTests;

#endif

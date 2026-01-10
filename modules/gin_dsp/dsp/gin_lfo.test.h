/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LFOTests : public juce::UnitTest
{
public:
    LFOTests() : juce::UnitTest ("LFO", "gin_dsp") {}

    void runTest() override
    {
        testSineWave();
        testTriangleWave();
        testSquareWave();
        testSawWave();
        testFrequency();
        testPhaseOffset();
        testDepthAndOffset();
        testFadeIn();
        testDelay();
    }

private:
    void testSineWave()
    {
        beginTest ("Sine Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::sine;
        params.frequency = 1.0f;  // 1 Hz = 1000 samples per cycle
        params.depth = 1.0f;
        params.offset = 0.0f;

        lfo.setParameters (params);
        lfo.noteOn();

        // At phase 0, sine should be 0
        float val = lfo.process (1);
        expectWithinAbsoluteError (val, 0.0f, 0.01f, "Sine at phase 0 should be ~0");

        // At quarter cycle (250 samples), sine should be 1
        lfo.noteOn();
        val = lfo.process (250);
        expectWithinAbsoluteError (val, 1.0f, 0.05f, "Sine at quarter cycle should be ~1");

        // At half cycle (500 samples), sine should be 0
        lfo.noteOn();
        val = lfo.process (500);
        expectWithinAbsoluteError (val, 0.0f, 0.05f, "Sine at half cycle should be ~0");

        // At three-quarter cycle (750 samples), sine should be -1
        lfo.noteOn();
        val = lfo.process (750);
        expectWithinAbsoluteError (val, -1.0f, 0.05f, "Sine at 3/4 cycle should be ~-1");
    }

    void testTriangleWave()
    {
        beginTest ("Triangle Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::triangle;
        params.frequency = 1.0f;
        params.depth = 1.0f;

        lfo.setParameters (params);
        lfo.noteOn();

        // Triangle should ramp from -1 to 1 in first half
        float val1 = lfo.process (1);
        lfo.noteOn();
        float val2 = lfo.process (250);

        expect (val2 > val1, "Triangle should be increasing in first quarter");
        expectWithinAbsoluteError (val2, 1.0f, 0.05f, "Triangle should be ~1 at quarter cycle");
    }

    void testSquareWave()
    {
        beginTest ("Square Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::square;
        params.frequency = 1.0f;
        params.depth = 1.0f;

        lfo.setParameters (params);
        lfo.noteOn();

        // First half of square wave should be high
        float val1 = lfo.process (100);
        expectWithinAbsoluteError (val1, 1.0f, 0.001f, "Square should be 1 in first half");

        // Second half should be low
        lfo.noteOn();
        float val2 = lfo.process (600);
        expectWithinAbsoluteError (val2, -1.0f, 0.001f, "Square should be -1 in second half");
    }

    void testSawWave()
    {
        beginTest ("Saw Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::sawUp;
        params.frequency = 1.0f;
        params.depth = 1.0f;

        lfo.setParameters (params);
        lfo.noteOn();

        // Saw should ramp from -1 to 1 over full cycle
        float val1 = lfo.process (1);
        lfo.noteOn();
        float val2 = lfo.process (500);

        expect (val2 > val1, "Saw should be increasing");
    }

    void testFrequency()
    {
        beginTest ("Frequency");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::sine;
        params.depth = 1.0f;

        // 2 Hz should complete cycle in 500 samples
        params.frequency = 2.0f;
        lfo.setParameters (params);
        lfo.noteOn();

        float val1 = lfo.process (125);  // Quarter of 500
        expectWithinAbsoluteError (val1, 1.0f, 0.1f, "2Hz sine should reach peak at 125 samples");

        // 0.5 Hz should complete cycle in 2000 samples
        params.frequency = 0.5f;
        lfo.setParameters (params);
        lfo.noteOn();

        float val2 = lfo.process (500);  // Quarter of 2000
        expectWithinAbsoluteError (val2, 1.0f, 0.1f, "0.5Hz sine should reach peak at 500 samples");
    }

    void testPhaseOffset()
    {
        beginTest ("Phase Offset");

        LFO lfo1, lfo2;
        lfo1.setSampleRate (1000.0);
        lfo2.setSampleRate (1000.0);

        LFO::Parameters params1, params2;
        params1.waveShape = LFO::WaveShape::sine;
        params1.frequency = 1.0f;
        params1.phase = 0.0f;
        params1.depth = 1.0f;

        params2.waveShape = LFO::WaveShape::sine;
        params2.frequency = 1.0f;
        params2.phase = 0.25f;  // 90 degrees offset
        params2.depth = 1.0f;

        lfo1.setParameters (params1);
        lfo2.setParameters (params2);

        lfo1.noteOn();
        lfo2.noteOn();

        float val1 = lfo1.process (1);
        float val2 = lfo2.process (1);

        // With 90 degree offset, when lfo1 is at 0, lfo2 should be at peak
        expect (std::abs (val2) > std::abs (val1), "Phase offset should create different values");
    }

    void testDepthAndOffset()
    {
        beginTest ("Depth and Offset");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::sine;
        params.frequency = 1.0f;
        params.depth = 0.5f;   // Half depth
        params.offset = 0.5f;  // Centered at 0.5

        lfo.setParameters (params);
        lfo.noteOn();

        float val = lfo.process (250);  // Peak of sine

        // With depth 0.5 and offset 0.5, peak should be around 0.5 + 0.5*1.0 = 1.0
        // and trough should be around 0.5 + 0.5*(-1.0) = 0.0
        expectWithinAbsoluteError (val, 1.0f, 0.1f, "Peak with depth 0.5, offset 0.5 should be ~1.0");

        lfo.noteOn();
        val = lfo.process (750);  // Trough of sine
        expectWithinAbsoluteError (val, 0.0f, 0.1f, "Trough with depth 0.5, offset 0.5 should be ~0.0");
    }

    void testFadeIn()
    {
        beginTest ("Fade In");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::square;
        params.frequency = 1.0f;
        params.depth = 1.0f;
        params.fade = 0.1f;  // 100ms fade-in

        lfo.setParameters (params);
        lfo.noteOn();

        // At start of fade, output should be low
        float val1 = lfo.process (1);
        expect (std::abs (val1) < 0.1f, "Output should be low at start of fade");

        // After fade duration, output should be at full level
        lfo.noteOn();
        float val2 = lfo.process (100);
        expect (std::abs (val2) > 0.8f, "Output should be at full level after fade");
    }

    void testDelay()
    {
        beginTest ("Delay");

        LFO lfo;
        lfo.setSampleRate (1000.0);

        LFO::Parameters params;
        params.waveShape = LFO::WaveShape::square;
        params.frequency = 1.0f;
        params.depth = 1.0f;
        params.delay = 0.1f;  // 100ms delay

        lfo.setParameters (params);
        lfo.noteOn();

        // During delay, output should be 0
        float val1 = lfo.process (50);
        expectWithinAbsoluteError (val1, 0.0f, 0.01f, "Output should be 0 during delay");

        // After delay, LFO should be running
        lfo.noteOn();
        float val2 = lfo.process (150);
        expect (std::abs (val2) > 0.5f, "Output should be active after delay");
    }
};

static LFOTests lfoTests;

#endif

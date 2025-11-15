/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

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
    }

private:
    void testSineWave()
    {
        beginTest ("Sine Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setFrequency (1.0f);  // 1 Hz = 1000 samples per cycle
        lfo.setWaveShape (LFO::WaveShape::sine);

        // At phase 0, sine should be 0
        lfo.reset();
        float val = lfo.process (1);
        expectWithinAbsoluteError (val, 0.0f, 0.01f, "Sine at phase 0 should be ~0");

        // At quarter cycle (250 samples), sine should be 1
        lfo.reset();
        val = lfo.process (250);
        expectWithinAbsoluteError (val, 1.0f, 0.05f, "Sine at quarter cycle should be ~1");

        // At half cycle (500 samples), sine should be 0
        lfo.reset();
        val = lfo.process (500);
        expectWithinAbsoluteError (val, 0.0f, 0.05f, "Sine at half cycle should be ~0");

        // At three-quarter cycle (750 samples), sine should be -1
        lfo.reset();
        val = lfo.process (750);
        expectWithinAbsoluteError (val, -1.0f, 0.05f, "Sine at 3/4 cycle should be ~-1");
    }

    void testTriangleWave()
    {
        beginTest ("Triangle Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setFrequency (1.0f);
        lfo.setWaveShape (LFO::WaveShape::triangle);

        lfo.reset();
        
        // Triangle should ramp from -1 to 1 in first half
        float val1 = lfo.process (1);
        float val2 = lfo.process (250);
        
        expect (val2 > val1, "Triangle should be increasing in first quarter");
        expectWithinAbsoluteError (val2, 1.0f, 0.05f, "Triangle should be ~1 at quarter cycle");
    }

    void testSquareWave()
    {
        beginTest ("Square Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setFrequency (1.0f);
        lfo.setWaveShape (LFO::WaveShape::square);

        lfo.reset();

        // First half of square wave should be high
        float val1 = lfo.process (100);
        expectWithinAbsoluteError (val1, 1.0f, 0.001f, "Square should be 1 in first half");

        // Second half should be low
        lfo.reset();
        float val2 = lfo.process (600);
        expectWithinAbsoluteError (val2, -1.0f, 0.001f, "Square should be -1 in second half");
    }

    void testSawWave()
    {
        beginTest ("Saw Wave");

        LFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setFrequency (1.0f);
        lfo.setWaveShape (LFO::WaveShape::sawUp);

        lfo.reset();

        // Saw should ramp from -1 to 1 over full cycle
        float val1 = lfo.process (1);
        float val2 = lfo.process (500);
        
        expect (val2 > val1, "Saw should be increasing");
    }

    void testFrequency()
    {
        beginTest ("Frequency");

        LFO lfo;
        lfo.setSampleRate (1000.0);
        lfo.setWaveShape (LFO::WaveShape::sine);

        // 2 Hz should complete cycle in 500 samples
        lfo.setFrequency (2.0f);
        lfo.reset();
        
        float val1 = lfo.process (125);  // Quarter of 500
        expectWithinAbsoluteError (val1, 1.0f, 0.1f, "2Hz sine should reach peak at 125 samples");

        // 0.5 Hz should complete cycle in 2000 samples
        lfo.setFrequency (0.5f);
        lfo.reset();
        
        float val2 = lfo.process (500);  // Quarter of 2000
        expectWithinAbsoluteError (val2, 1.0f, 0.1f, "0.5Hz sine should reach peak at 500 samples");
    }

    void testPhaseOffset()
    {
        beginTest ("Phase Offset");

        LFO lfo1, lfo2;
        lfo1.setSampleRate (1000.0);
        lfo2.setSampleRate (1000.0);
        lfo1.setFrequency (1.0f);
        lfo2.setFrequency (1.0f);
        lfo1.setWaveShape (LFO::WaveShape::sine);
        lfo2.setWaveShape (LFO::WaveShape::sine);

        lfo1.setPhase (0.0f);
        lfo2.setPhase (0.25f);  // 90 degrees offset

        lfo1.reset();
        lfo2.reset();

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
        lfo.setFrequency (1.0f);
        lfo.setWaveShape (LFO::WaveShape::sine);
        lfo.setDepth (0.5f);   // Half depth
        lfo.setOffset (0.5f);  // Centered at 0.5

        lfo.reset();
        float val = lfo.process (250);  // Peak of sine

        // With depth 0.5 and offset 0.5, peak should be around 0.5 + 0.5*1.0 = 1.0
        // and trough should be around 0.5 + 0.5*(-1.0) = 0.0
        expectWithinAbsoluteError (val, 1.0f, 0.1f, "Peak with depth 0.5, offset 0.5 should be ~1.0");

        lfo.reset();
        val = lfo.process (750);  // Trough of sine
        expectWithinAbsoluteError (val, 0.0f, 0.1f, "Trough with depth 0.5, offset 0.5 should be ~0.0");
    }
};

static LFOTests lfoTests;

#endif

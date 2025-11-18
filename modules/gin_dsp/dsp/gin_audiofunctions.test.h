/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class AudioFunctionsTests : public juce::UnitTest
{
public:
    AudioFunctionsTests() : juce::UnitTest ("Audio Functions", "gin_dsp") {}

    void runTest() override
    {
        testSoftClip();
        testHardClip();
        testDB2Gain();
        testGain2DB();
        testPitchToFrequency();
        testFrequencyToPitch();
    }

private:
    void testSoftClip()
    {
        beginTest ("Soft Clip");

        // Soft clip should leave small signals mostly unchanged
        float small = softClip (0.1f);
        expectWithinAbsoluteError (small, 0.1f, 0.01f, "Small signals should pass through mostly unchanged");

        // Should compress signals approaching 1.0
        float large = softClip (1.5f);
        expect (large < 1.5f && large > 0.0f, "Large signals should be compressed");
        expect (large < 1.0f, "Soft clip should keep signal below 1.0");

        // Should be symmetric
        float neg = softClip (-0.5f);
        float pos = softClip (0.5f);
        expectWithinAbsoluteError (neg, -pos, 0.001f, "Soft clip should be symmetric");
    }

    void testHardClip()
    {
        beginTest ("Hard Clip");

        expectWithinAbsoluteError (hardClip (0.5f), 0.5f, 0.001f, "Should pass through values in range");
        expectWithinAbsoluteError (hardClip (1.5f), 1.0f, 0.001f, "Should clip positive to 1.0");
        expectWithinAbsoluteError (hardClip (-1.5f), -1.0f, 0.001f, "Should clip negative to -1.0");
        expectWithinAbsoluteError (hardClip (0.0f), 0.0f, 0.001f, "Zero should remain zero");
    }

    void testDB2Gain()
    {
        beginTest ("dB to Gain");

        expectWithinAbsoluteError (dbToGain (0.0f), 1.0f, 0.001f, "0 dB should be gain 1.0");
        expectWithinAbsoluteError (dbToGain (6.0f), 2.0f, 0.1f, "6 dB should be ~gain 2.0");
        expectWithinAbsoluteError (dbToGain (-6.0f), 0.5f, 0.05f, "-6 dB should be ~gain 0.5");
        expectWithinAbsoluteError (dbToGain (-100.0f), 0.0f, 0.001f, "-100 dB should be ~0");

        // Test infinity
        float inf = dbToGain (-std::numeric_limits<float>::infinity());
        expectWithinAbsoluteError (inf, 0.0f, 0.001f, "-inf dB should be 0 gain");
    }

    void testGain2DB()
    {
        beginTest ("Gain to dB");

        expectWithinAbsoluteError (gainToDb (1.0f), 0.0f, 0.001f, "Gain 1.0 should be 0 dB");
        expectWithinAbsoluteError (gainToDb (2.0f), 6.0f, 0.1f, "Gain 2.0 should be ~6 dB");
        expectWithinAbsoluteError (gainToDb (0.5f), -6.0f, 0.1f, "Gain 0.5 should be ~-6 dB");

        // Zero gain should give very negative dB
        float db = gainToDb (0.0f);
        expect (db < -100.0f, "Gain 0.0 should give very negative dB");
    }

    void testPitchToFrequency()
    {
        beginTest ("Pitch to Frequency");

        // MIDI note 69 (A4) should be 440 Hz
        expectWithinAbsoluteError (getMidiNoteFromHertz (69), 440.0f, 0.1f, "MIDI 69 should be 440 Hz");

        // MIDI note 60 (C4) should be ~261.6 Hz
        expectWithinAbsoluteError (getMidiNoteFromHertz (60), 261.6f, 1.0f, "MIDI 60 should be ~261.6 Hz");

        // Octave up should double frequency
        float f1 = getMidiNoteFromHertz (60);
        float f2 = getMidiNoteFromHertz (72);
        expectWithinAbsoluteError (f2, f1 * 2.0f, 1.0f, "Octave up should double frequency");
    }

    void testFrequencyToPitch()
    {
        beginTest ("Frequency to Pitch");

        // 440 Hz should be MIDI note 69 (A4)
        expectWithinAbsoluteError (getHertzFromMidiNote (440.0f), 69.0f, 0.1f, "440 Hz should be MIDI 69");

        // ~261.6 Hz should be MIDI note 60 (C4)
        expectWithinAbsoluteError (getHertzFromMidiNote (261.6f), 60.0f, 0.1f, "261.6 Hz should be MIDI 60");

        // Doubling frequency should add 12 semitones
        float note1 = getHertzFromMidiNote (200.0f);
        float note2 = getHertzFromMidiNote (400.0f);
        expectWithinAbsoluteError (note2 - note1, 12.0f, 0.1f, "Doubling freq should add 12 semitones");
    }
};

static AudioFunctionsTests audioFunctionsTests;

#endif

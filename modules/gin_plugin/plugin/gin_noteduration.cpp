
//==============================================================================
NoteDuration::NoteDuration (const char* name_, float bars_, float note_)
  : name (name_), bars (bars_), note (note_)
{
}

float NoteDuration::toSeconds (juce::AudioPlayHead* playhead) const
{
    float timeSigNumerator = 4;
    float timeSigDenominator = 4;
    float bpm = 120.0f;

    if (playhead != nullptr)
    {
        if (auto position = playhead->getPosition(); position.hasValue())
        {
            if (auto tso = position->getTimeSignature(); tso.hasValue())
            {
                timeSigNumerator = float (tso->numerator);
                timeSigDenominator = float (tso->denominator);
            }
            if (auto bo = position->getBpm(); bo.hasValue())
                bpm = float (*bo);

            if (bpm == 0.0)
                bpm = 120;

            bpm = juce::jlimit (1.0f, 500.0f, bpm);
        }
    }

    float sPerBeat  = float (1.0f / (bpm / 60.0f));
    float sPerWhole = sPerBeat * timeSigDenominator;

    if (note > 0.0f)
    {
        return sPerWhole * note;
    }
    else if (bars > 0.0f)
    {
        float beatLen = sPerWhole * 1.0f / timeSigDenominator;
        return bars * timeSigNumerator * beatLen;
    }
    return 0.0f;
}

float NoteDuration::toSeconds (float bpm) const
{
    float timeSigNumerator = 4;
    float timeSigDenominator = 4;

    float sPerBeat  = float (1.0f / (bpm / 60.0f));
    float sPerWhole = sPerBeat * timeSigDenominator;

    if (note > 0.0f)
    {
        return sPerWhole * note;
    }
    else if (bars > 0.0f)
    {
        float beatLen = sPerWhole * 1.0f / timeSigDenominator;
        return bars * timeSigNumerator * beatLen;
    }
    return 0.0f;
}

float NoteDuration::toSeconds (juce::AudioPlayHead* playhead, float value)
{
    auto vi = juce::roundToInt (value);
    if (std::abs (vi - value) < 0.001)
        return getNoteDurations()[size_t (vi)].toSeconds (playhead);

    auto v1 = int (value);
    auto v2 = v1 + 1;

    auto d1 = getNoteDurations()[size_t (v1)].toSeconds (playhead);
    auto d2 = getNoteDurations()[size_t (v2)].toSeconds (playhead);

    auto frac = value - v1;

    return juce::jmap (frac, 0.0f, 1.0f, d1, d2);
}

const std::vector<NoteDuration>& NoteDuration::getNoteDurations()
{
    const float dot = 1.5f;
    const float triplet = 2.0f / 3.0f;

    static std::vector<NoteDuration> durations =
    {
        {"1/64t",   0.0f,  0.015625f * triplet},
        {"1/64",    0.0f,  0.015625f},
        {"1/64d",   0.0f,  0.015625f * dot},
        {"1/32t",   0.0f,  0.03125f * triplet},
        {"1/32",    0.0f,  0.03125f},
        {"1/32d",   0.0f,  0.03125f * dot},
        {"1/16t",   0.0f,  0.0625f * triplet},
        {"1/16",    0.0f,  0.0625f},
        {"1/16d",   0.0f,  0.0625f * dot},
        {"1/8t",    0.0f,  0.125f * triplet},
        {"1/8",     0.0f,  0.125f},
        {"1/8d",    0.0f,  0.125f * dot},
        {"1/4t",    0.0f,  0.25f * triplet},
        {"1/4",     0.0f,  0.25f},
        {"1/4d",    0.0f,  0.25f * dot},
        {"1/2t",    0.0f,  0.5f * triplet},
        {"1/2",     0.0f,  0.5f},
        {"1/2d",    0.0f,  0.5f * dot},
        {"1/1t",    0.0f,  1.0f * triplet},
        {"1/1",     0.0f,  1.0f},
        {"1/1d",    0.0f,  1.0f * dot},
        {"1 bar",   1.0f,  0.0f},
        {"2 bars",  2.0f,  0.0f},
        {"4 bars",  4.0f,  0.0f},
        {"6 bars",  6.0f,  0.0f},
        {"8 bars",  8.0f,  0.0f},
        {"10 bars", 10.0f, 0.0f},
        {"12 bars", 12.0f, 0.0f},
        {"16 bars", 16.0f, 0.0f},
        {"20 bars", 20.0f, 0.0f},
        {"24 bars", 24.0f, 0.0f},
        {"28 bars", 28.0f, 0.0f},
        {"32 bars", 32.0f, 0.0f},
    };

    return durations;
}

//==============================================================================
#if JUCE_UNIT_TESTS

class NoteDurationTests : public juce::UnitTest
{
public:
    NoteDurationTests() : juce::UnitTest ("Note Duration", "gin_plugin") {}

    void runTest() override
    {
        testBasicNoteValues();
        testDottedNotes();
        testTriplets();
        testBars();
        testBPMConversion();
        testInterpolation();
    }

private:
    void testBasicNoteValues()
    {
        beginTest ("Basic Note Values");

        float bpm = 120.0f;

        // At 120 BPM: 1 beat = 0.5 seconds
        // Whole note in 4/4 = 4 beats = 2.0 seconds
        // Half note = 2 beats = 1.0 second
        // Quarter note = 1 beat = 0.5 seconds
        // Eighth note = 0.5 beats = 0.25 seconds

        NoteDuration whole ("1/1", 0.0f, 1.0f);
        expectWithinAbsoluteError (whole.toSeconds (bpm), 2.0f, 0.001f, "Whole note should be 2 seconds at 120 BPM");

        NoteDuration half ("1/2", 0.0f, 0.5f);
        expectWithinAbsoluteError (half.toSeconds (bpm), 1.0f, 0.001f, "Half note should be 1 second at 120 BPM");

        NoteDuration quarter ("1/4", 0.0f, 0.25f);
        expectWithinAbsoluteError (quarter.toSeconds (bpm), 0.5f, 0.001f, "Quarter note should be 0.5 seconds at 120 BPM");

        NoteDuration eighth ("1/8", 0.0f, 0.125f);
        expectWithinAbsoluteError (eighth.toSeconds (bpm), 0.25f, 0.001f, "Eighth note should be 0.25 seconds at 120 BPM");

        NoteDuration sixteenth ("1/16", 0.0f, 0.0625f);
        expectWithinAbsoluteError (sixteenth.toSeconds (bpm), 0.125f, 0.001f, "16th note should be 0.125 seconds at 120 BPM");
    }

    void testDottedNotes()
    {
        beginTest ("Dotted Notes");

        float bpm = 120.0f;

        // Dotted notes are 1.5x the original duration
        NoteDuration quarterDotted ("1/4d", 0.0f, 0.25f * 1.5f);
        expectWithinAbsoluteError (quarterDotted.toSeconds (bpm), 0.75f, 0.001f,
                                   "Dotted quarter should be 0.75 seconds");

        NoteDuration halfDotted ("1/2d", 0.0f, 0.5f * 1.5f);
        expectWithinAbsoluteError (halfDotted.toSeconds (bpm), 1.5f, 0.001f,
                                   "Dotted half should be 1.5 seconds");

        NoteDuration eighthDotted ("1/8d", 0.0f, 0.125f * 1.5f);
        expectWithinAbsoluteError (eighthDotted.toSeconds (bpm), 0.375f, 0.001f,
                                   "Dotted eighth should be 0.375 seconds");
    }

    void testTriplets()
    {
        beginTest ("Triplets");

        float bpm = 120.0f;

        // Triplets are 2/3 of the original duration
        NoteDuration quarterTriplet ("1/4t", 0.0f, 0.25f * (2.0f / 3.0f));
        expectWithinAbsoluteError (quarterTriplet.toSeconds (bpm), 0.5f * (2.0f / 3.0f), 0.001f,
                                   "Quarter triplet should be 2/3 of quarter note");

        NoteDuration eighthTriplet ("1/8t", 0.0f, 0.125f * (2.0f / 3.0f));
        expectWithinAbsoluteError (eighthTriplet.toSeconds (bpm), 0.25f * (2.0f / 3.0f), 0.001f,
                                   "Eighth triplet should be 2/3 of eighth note");
    }

    void testBars()
    {
        beginTest ("Bars");

        float bpm = 120.0f;

        // At 120 BPM in 4/4: 1 bar = 4 beats = 2 seconds
        NoteDuration oneBar ("1 bar", 1.0f, 0.0f);
        expectWithinAbsoluteError (oneBar.toSeconds (bpm), 2.0f, 0.001f,
                                   "1 bar should be 2 seconds at 120 BPM in 4/4");

        NoteDuration twoBars ("2 bars", 2.0f, 0.0f);
        expectWithinAbsoluteError (twoBars.toSeconds (bpm), 4.0f, 0.001f,
                                   "2 bars should be 4 seconds");

        NoteDuration fourBars ("4 bars", 4.0f, 0.0f);
        expectWithinAbsoluteError (fourBars.toSeconds (bpm), 8.0f, 0.001f,
                                   "4 bars should be 8 seconds");
    }

    void testBPMConversion()
    {
        beginTest ("BPM Conversion");

        NoteDuration quarter ("1/4", 0.0f, 0.25f);

        // At 60 BPM: 1 beat = 1 second
        expectWithinAbsoluteError (quarter.toSeconds (60.0f), 1.0f, 0.001f,
                                   "Quarter note at 60 BPM should be 1 second");

        // At 120 BPM: 1 beat = 0.5 seconds
        expectWithinAbsoluteError (quarter.toSeconds (120.0f), 0.5f, 0.001f,
                                   "Quarter note at 120 BPM should be 0.5 seconds");

        // At 240 BPM: 1 beat = 0.25 seconds
        expectWithinAbsoluteError (quarter.toSeconds (240.0f), 0.25f, 0.001f,
                                   "Quarter note at 240 BPM should be 0.25 seconds");

        // At 90 BPM: 1 beat = 0.666... seconds
        expectWithinAbsoluteError (quarter.toSeconds (90.0f), 0.6667f, 0.001f,
                                   "Quarter note at 90 BPM should be ~0.667 seconds");
    }

    void testInterpolation()
    {
        beginTest ("Interpolation");

        // Test the static interpolation function
        // This should interpolate between predefined note durations

        auto& durations = NoteDuration::getNoteDurations();
        expect (durations.size() > 0, "Should have predefined durations");

        // Test that integer values return exact durations
        float dur0 = NoteDuration::toSeconds (nullptr, 0.0f);
        expect (dur0 > 0.0f, "Duration 0 should be valid");

        float dur1 = NoteDuration::toSeconds (nullptr, 1.0f);
        expect (dur1 > 0.0f, "Duration 1 should be valid");

        // Test interpolation between values
        float durMid = NoteDuration::toSeconds (nullptr, 0.5f);
        expect (durMid > dur0 && durMid < dur1, "Interpolated value should be between endpoints");
    }
};

static NoteDurationTests noteDurationTests;

#endif

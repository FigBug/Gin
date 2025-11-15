
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

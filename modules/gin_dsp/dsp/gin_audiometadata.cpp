namespace
{

juce::StringArray reverseTokens (juce::StringRef stringToTokenise, juce::StringRef breakCharacters, juce::StringRef quoteCharacters)
{
    auto tokens = juce::StringArray::fromTokens (stringToTokenise, breakCharacters, quoteCharacters);
    std::reverse (tokens.strings.begin(), tokens.strings.end());
    return tokens;
}

std::optional<float> getFileNameTempo (const juce::String& rawName)
{
    auto name = rawName.replace (" ", "_").replace ("-", "_");

    for (auto token : reverseTokens (name, "_", ""))
    {
        if (token.containsIgnoreCase ("bpm"))
        {
            token = token.replace ("bpm", "", true).trim();

            while (token.startsWith ("0"))
                token = token.substring (1);

            auto val = token.getIntValue();

            if (val > 50 && val < 250 && juce::String (val) == token)
                return float (val);
        }
    }

    for (auto token : reverseTokens (name, "_", ""))
    {
        auto val = token.getIntValue();

        while (token.startsWith ("0"))
            token = token.substring (1);

        if (val > 50 && val < 250 && juce::String (val) == token)
            return float (val);
    }

    return {};
}

std::optional<float> getCueTempo (const juce::StringPairArray& metadata)
{
    if (auto tempoStr = metadata["CueLabel0Text"]; tempoStr.isNotEmpty())
        if (tempoStr.contains ("Tempo:"))
            if (auto val = tempoStr.fromFirstOccurrenceOf ("Tempo: ", false, false).getFloatValue(); val >= 50 && val <= 250)
                return val;

    return {};
}

std::optional<float> deduceTempo (const juce::File& file, const juce::AudioFormatReader& afr)
{
    auto len = afr.lengthInSamples / afr.sampleRate;
    if (len <= 1.0 && len > 60.0)
        return {};

    auto fn = file.getFileNameWithoutExtension();

    auto tempo = getCueTempo (afr.metadataValues);
    if (! tempo.has_value())
        tempo = getFileNameTempo (fn);

    if (! tempo.has_value())
        return {};

    auto beats = *tempo / 60 * len;
    auto rem = std::fmod (beats, 4.0f);
    if (rem < 0.0f || (rem > 0.1f && rem < 3.9f) || rem > 4.0f)
        return {};

    return tempo;
}

std::optional<int> deduceRoot (const juce::File& f)
{
    auto noteValue = [] (const juce::String& noteName) -> std::optional<int>
    {
        auto nnl = noteName.toLowerCase();

        if (nnl == "c#") return 13;
        if (nnl == "d#") return 15;
        if (nnl == "f#") return 18;
        if (nnl == "g#") return 20;
        if (nnl == "a#") return 22;
        if (nnl == "db") return 13;
        if (nnl == "eb") return 15;
        if (nnl == "gb") return 18;
        if (nnl == "ab") return 20;
        if (nnl == "bb") return 22;
        if (nnl == "c")  return 12;
        if (nnl == "d")  return 14;
        if (nnl == "e")  return 16;
        if (nnl == "f")  return 17;
        if (nnl == "g")  return 19;
        if (nnl == "a")  return 21;
        if (nnl == "b")  return 23;

        for (int i = 0; i <= 127; i++)
            if (nnl == juce::MidiMessage::getMidiNoteName (i, true, true, 4).toLowerCase() || nnl == juce::MidiMessage::getMidiNoteName (i, false, true, 4).toLowerCase())
                return i;

        return {};
    };

    for (auto token : juce::StringArray::fromTokens (f.getFileNameWithoutExtension(), " _-[](){}", ""))
    {
        if (token.endsWith ("min"))       token = token.dropLastCharacters (3);
        else if (token.endsWith ("maj"))  token = token.dropLastCharacters (3);
        else if (token.endsWith ("m"))    token = token.dropLastCharacters (1);

        if (auto noteVal = noteValue (token))
            return noteVal;
    }

    return {};
}

void parseAiffMetadata (AudioMetadata& result, const juce::StringPairArray& metadata, int64_t lengthInSamples, double sampleRate)
{
    // Root note from Apple Loops basc chunk
    if (metadata[juce::AiffAudioFormat::appleRootSet] == "1")
        result.root = metadata[juce::AiffAudioFormat::appleRootNote].getIntValue();

    // BPM from Apple Loops basc chunk
    const double numBeats = metadata[juce::AiffAudioFormat::appleBeats].getDoubleValue();
    if (numBeats > 0)
        result.bpm = static_cast<float> ((numBeats * 60.0) / (lengthInSamples / sampleRate));

    // Time signature from Apple Loops basc chunk
    if (auto num = metadata[juce::AiffAudioFormat::appleNumerator].getIntValue())
        result.numerator = num;
    if (auto denom = metadata[juce::AiffAudioFormat::appleDenominator].getIntValue())
        result.denominator = denom;

    // Loop points from INST chunk via cue points
    int markers = metadata["NumCuePoints"].getIntValue();
    if (markers >= 2)
    {
        auto startId = metadata["Cue0Identifier"].getIntValue();
        auto endId = metadata["Cue1Identifier"].getIntValue();

        for (int marker = 0; marker <= markers; marker++)
        {
            auto name = "Cue" + juce::String (marker);
            auto id = metadata[name + "Identifier"].getIntValue();
            auto offset = metadata[name + "Offset"].getLargeIntValue();

            if (id == startId)
                result.loopStart = offset;
            if (id == endId && offset < lengthInSamples)
                result.loopEnd = offset;
        }

        if (result.loopStart.has_value() && *result.loopStart >= lengthInSamples)
            result.loopStart.reset();
    }
}

void parseWavMetadata (AudioMetadata& result, const juce::StringPairArray& metadata, int64_t lengthInSamples, double sampleRate)
{
    // Root note from ACID chunk or sampler chunk
    if (metadata[juce::WavAudioFormat::acidRootSet] == "1")
        result.root = metadata[juce::WavAudioFormat::acidRootNote].getIntValue();

    if (! result.root.has_value())
        if (auto smplNote = metadata["MidiUnityNote"].getIntValue())
            result.root = smplNote;

    // BPM from ACID chunk
    const double numBeats = metadata[juce::WavAudioFormat::acidBeats].getDoubleValue();
    if (numBeats > 0)
        result.bpm = static_cast<float> ((numBeats * 60.0) / (lengthInSamples / sampleRate));

    // Time signature from ACID chunk
    if (auto num = metadata[juce::WavAudioFormat::acidNumerator].getIntValue())
        result.numerator = num;
    if (auto denom = metadata[juce::WavAudioFormat::acidDenominator].getIntValue())
        result.denominator = denom;

    // Loop points from sampler chunk
    auto numLoops = metadata["NumSampleLoops"].getIntValue();
    if (numLoops > 0)
    {
        result.loopStart = metadata["Loop0Start"].getLargeIntValue();
        auto end = metadata["Loop0End"].getLargeIntValue() + 1;

        if (end < lengthInSamples)
            result.loopEnd = end;

        if (result.loopStart.has_value() && *result.loopStart >= lengthInSamples)
            result.loopStart.reset();
    }
}

} // namespace

AudioMetadata::AudioMetadata (const juce::File& f, juce::AudioFormatManager& mgr)
{
    auto reader = std::unique_ptr<juce::AudioFormatReader> (mgr.createReaderFor (f));
    if (reader == nullptr)
        return;

    if (reader->getFormatName() == juce::AiffAudioFormat().getFormatName())
        parseAiffMetadata (*this, reader->metadataValues, reader->lengthInSamples, reader->sampleRate);
    else
        parseWavMetadata (*this, reader->metadataValues, reader->lengthInSamples, reader->sampleRate);

    if (! root.has_value())
        root = deduceRoot (f);

    if (! bpm.has_value())
        bpm = deduceTempo (f, *reader);
}


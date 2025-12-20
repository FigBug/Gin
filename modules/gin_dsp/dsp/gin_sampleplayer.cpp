SamplePlayer::SamplePlayer()
{
}

SamplePlayer::SamplePlayer (const char* data, int dataLen)
    : SamplePlayer()
{
    load (data, dataLen);
}

SamplePlayer::SamplePlayer (const juce::File& f)
    : SamplePlayer()
{
    load (f);
}

void SamplePlayer::load (const char* data, int dataLen)
{
    juce::AudioFormatManager mgr;
    mgr.registerBasicFormats();

    if (auto reader = std::unique_ptr<juce::AudioFormatReader> (mgr.createReaderFor (std::make_unique<juce::MemoryInputStream> (data, dataLen, false))))
    {
        juce::AudioSampleBuffer newBuffer;
        newBuffer.setSize (reader->numChannels, int (reader->lengthInSamples));
        reader->read (&newBuffer, 0, newBuffer.getNumSamples(), 0, true, true);

        const juce::SpinLock::ScopedLockType sl (lock);
        loadedFilePath = juce::File();
        buffer = std::move (newBuffer);
        sourceSampleRate = reader->sampleRate;
        position.store (0.0);
        fileLoaded.store (true);
    }
}

void SamplePlayer::load (const juce::File& f)
{
    juce::AudioFormatManager mgr;
    mgr.registerBasicFormats();

    if (auto reader = std::unique_ptr<juce::AudioFormatReader> (mgr.createReaderFor (f)))
    {
        juce::AudioSampleBuffer newBuffer;
        newBuffer.setSize (reader->numChannels, int (reader->lengthInSamples));
        reader->read (&newBuffer, 0, newBuffer.getNumSamples(), 0, true, true);

        // Extract metadata if available
        AudioMetadata metadata (f, mgr);
        double fileBpm = metadata.bpm.value_or (120.0);
        int fileNumerator = metadata.numerator.value_or (4);
        int fileDenominator = metadata.denominator.value_or (4);

        const juce::SpinLock::ScopedLockType sl (lock);
        loadedFilePath = f;
        buffer = std::move (newBuffer);
        sourceSampleRate = reader->sampleRate;
        bpm = fileBpm;
        timeSigNumerator = fileNumerator;
        timeSigDenominator = fileDenominator;
        position.store (0.0);
        fileLoaded.store (true);
    }
}

void SamplePlayer::clear()
{
    playing.store (false);

    const juce::SpinLock::ScopedLockType sl (lock);
    loadedFilePath = juce::File();
    buffer.setSize (0, 0);
    sourceSampleRate = 44100.0;
    position.store (0.0);
    fileLoaded.store (false);
}

void SamplePlayer::setBuffer (const juce::AudioSampleBuffer& newBuffer, double sampleRate)
{
    const juce::SpinLock::ScopedLockType sl (lock);
    buffer = newBuffer;
    sourceSampleRate = sampleRate;
    position.store (0.0);
}

void SamplePlayer::setPlaybackSampleRate (double sampleRate)
{
    playbackSampleRate = sampleRate;
}

void SamplePlayer::reset()
{
    position.store (0.0);
}

void SamplePlayer::setCrossfadeSamples (int samples)
{
    crossfadeSamples = juce::jmax (0, samples);
}

void SamplePlayer::play()
{
    playing.store (true);
}

void SamplePlayer::stop()
{
    playing.store (false);
}

void SamplePlayer::processBlock (juce::AudioSampleBuffer& output)
{
    if (! playing.load())
        return;

    const juce::SpinLock::ScopedTryLockType sl (lock);
    if (! sl.isLocked())
        return;

    if (buffer.getNumSamples() == 0)
        return;

    const auto numSamples = buffer.getNumSamples();
    const auto srcChannels = buffer.getNumChannels();
    const auto outputChannels = output.getNumChannels();
    const auto outputSamples = output.getNumSamples();
    const auto ratio = sourceSampleRate / playbackSampleRate;
    const bool shouldLoop = looping.load();

    double pos = position.load();

    for (int i = 0; i < outputSamples; ++i)
    {
        // Get samples from source (mono or stereo)
        float srcSamples[2];
        srcSamples[0] = interpolateSampleWithCrossfade (0, pos);
        srcSamples[1] = (srcChannels > 1) ? interpolateSampleWithCrossfade (1, pos) : srcSamples[0];

        // Mix down to mono if needed, or output to all channels
        if (outputChannels == 1)
        {
            // Stereo to mono: sum and halve
            output.addSample (0, i, (srcSamples[0] + srcSamples[1]) * 0.5f);
        }
        else
        {
            // Output to all channels (duplicate stereo pair if more than 2 output channels)
            for (int ch = 0; ch < outputChannels; ++ch)
                output.addSample (ch, i, srcSamples[ch % 2]);
        }

        pos += ratio;

        if (pos >= numSamples)
        {
            if (shouldLoop)
                pos = std::fmod (pos, static_cast<double> (numSamples));
            else
            {
                playing.store (false);
                position.store (pos);
                return;
            }
        }
    }

    position.store (pos);
}

double SamplePlayer::getPosition() const
{
    return position.load();
}

void SamplePlayer::setPosition (double newPosition)
{
    const int numSamples = buffer.getNumSamples();

    if (numSamples > 0)
        position.store (std::fmod (newPosition, static_cast<double> (numSamples)));
}

float SamplePlayer::interpolateSampleWithCrossfade (int channel, double pos)
{
    if (channel >= buffer.getNumChannels())
        return 0.0f;

    const int numSamples = buffer.getNumSamples();
    const auto mainSample = interpolateSample (channel, pos);
    const double crossfadeStart = numSamples - crossfadeSamples;

    if (pos >= crossfadeStart && crossfadeSamples > 0)
    {
        const double wrappedPosition = pos - numSamples;
        const auto beginSample = interpolateSample (channel, wrappedPosition + numSamples);

        auto fadeAmount = static_cast<float> ((pos - crossfadeStart) / crossfadeSamples);
        fadeAmount = juce::jlimit (0.0f, 1.0f, fadeAmount);

        return mainSample * (1.0f - fadeAmount) + beginSample * fadeAmount;
    }

    return mainSample;
}

float SamplePlayer::interpolateSample (int channel, double samplePosition)
{
    if (channel >= buffer.getNumChannels())
        return 0.0f;

    const float* channelData = buffer.getReadPointer (channel);
    const int numSamples = buffer.getNumSamples();

    const int index = static_cast<int> (samplePosition);
    const float frac = static_cast<float> (samplePosition - index);

    const float y0 = channelData[(index - 1 + numSamples) % numSamples];
    const float y1 = channelData[index % numSamples];
    const float y2 = channelData[(index + 1) % numSamples];
    const float y3 = channelData[(index + 2) % numSamples];

    const float c0 = y1;
    const float c1 = -y0 / 3.0f - y1 / 2.0f + y2 - y3 / 6.0f;
    const float c2 = y0 / 2.0f - y1 + y2 / 2.0f;
    const float c3 = -y0 / 6.0f + y1 / 2.0f - y2 / 2.0f + y3 / 6.0f;

    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

juce::AudioPlayHead::PositionInfo SamplePlayer::populatePositionInfo()
{
    juce::AudioPlayHead::PositionInfo info;

    info.setIsPlaying (playing.load());
    info.setIsRecording (false);
    info.setIsLooping (looping.load());

    const double pos = position.load();
    const double positionSeconds = pos / sourceSampleRate;

    info.setTimeInSeconds (positionSeconds);
    info.setTimeInSamples (static_cast<int64_t> (pos * playbackSampleRate / sourceSampleRate));

    info.setBpm (bpm);

    // Calculate PPQ position (quarter notes from start)
    const double beatsPerSecond = bpm / 60.0;
    const double ppq = positionSeconds * beatsPerSecond;
    info.setPpqPosition (ppq);

    // Calculate bar start position using actual time signature
    const double beatsPerBar = static_cast<double> (timeSigNumerator);
    const double barNumber = std::floor (ppq / beatsPerBar);
    info.setPpqPositionOfLastBarStart (barNumber * beatsPerBar);

    // Set time signature from file metadata
    info.setTimeSignature (juce::AudioPlayHead::TimeSignature { timeSigNumerator, timeSigDenominator });

    if (looping.load())
    {
        const int numSamples = buffer.getNumSamples();
        const double lengthSeconds = static_cast<double> (numSamples) / sourceSampleRate;
        const double lengthPpq = lengthSeconds * beatsPerSecond;

        juce::AudioPlayHead::LoopPoints loopPoints;
        loopPoints.ppqStart = 0.0;
        loopPoints.ppqEnd = lengthPpq;
        info.setLoopPoints (loopPoints);
    }

    return info;
}

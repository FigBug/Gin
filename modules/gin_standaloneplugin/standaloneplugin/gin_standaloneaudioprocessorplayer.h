#pragma once

/** A concrete implementation of AudioPlayHead for standalone plugin use.
    Allows external code to set the position info which is then returned
    when the processor queries the playhead.
*/
class StandalonePlayhead : public juce::AudioPlayHead
{
public:
    StandalonePlayhead() = default;

    /** Returns the current position info.
        Called by the AudioProcessor during processBlock.
    */
    juce::Optional<PositionInfo> getPosition() const
    {
        return positionInfo;
    }

    /** Sets the position info to be returned by getPosition().
        Call this before the processor's processBlock to provide timing info.
    */
    void setPositionInfo (const PositionInfo& info)
    {
        positionInfo = info;
    }

private:
    PositionInfo positionInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StandalonePlayhead)
};

class AudioRecorder : public juce::Thread
{
public:
    enum class RecordMode
    {
        idle,
        normal,
        triggered,
        waitingForTrigger
    };

    AudioRecorder (AudioFifo& f)
        : juce::Thread ("AudioRecorder"), fifo (f)
    {
        startThread();
    }

    ~AudioRecorder() override
    {
        stopRecording();
        stopThread (1000);
    }

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        retrospectiveBuffer.setSize (fifo.getNumChannels(), int (sr * 30));
    }

    void setDestFolder (const juce::File& folder)
    {
        destFolder = folder;
    }

    juce::File getDestFolder() const
    {
        return destFolder;
    }

    void startRecording()
    {
        if (recordMode.load() != RecordMode::idle)
            return;

        createNewFile();
        recordMode.store (RecordMode::normal);
    }

    void startTriggeredRecording()
    {
        if (recordMode.load() != RecordMode::idle)
            return;

        recordMode.store (RecordMode::waitingForTrigger);
    }

    void stopRecording()
    {
        recordMode.store (RecordMode::idle);

        const juce::ScopedLock sl (writerLock);
        if (writer != nullptr)
        {
            writer->flush();
            writer.reset();
        }
    }

    bool isRecording() const
    {
        auto mode = recordMode.load();
        return mode == RecordMode::normal || mode == RecordMode::triggered;
    }

    bool isWaitingForTrigger() const
    {
        return recordMode.load() == RecordMode::waitingForTrigger;
    }

    RecordMode getRecordMode() const
    {
        return recordMode.load();
    }

    void saveRetrospective()
    {
        const juce::SpinLock::ScopedLockType sl (retroLock);

        auto numReady = retrospectiveBuffer.getNumReady();
        if (numReady == 0)
            return;

        createNewFile();

        ScratchBuffer buffer (retrospectiveBuffer.getNumChannels(), numReady);
        retrospectiveBuffer.peek (buffer);

        const juce::ScopedLock wl (writerLock);
        if (writer != nullptr)
        {
            writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
            writer->flush();
            writer.reset();
        }
    }

    int getRetrospectiveNumReady() const
    {
        return retrospectiveBuffer.getNumReady();
    }

    int getRetrospectiveNumChannels() const
    {
        return retrospectiveBuffer.getNumChannels();
    }

    bool peekRetrospective (juce::AudioSampleBuffer& dest)
    {
        const juce::SpinLock::ScopedTryLockType sl (retroLock);
        if (! sl.isLocked())
            return false;

        return retrospectiveBuffer.peek (dest);
    }

    void clearRetrospective()
    {
        const juce::SpinLock::ScopedLockType sl (retroLock);
        retrospectiveBuffer.reset();
    }

    std::function<void()> onRecordingStarted;
    std::function<void()> onRecordingStopped;

    void run() override
    {
        while (! threadShouldExit())
        {
            auto ready = fifo.getNumReady();
            if (ready > 0)
            {
                ScratchBuffer buffer (fifo.getNumChannels(), ready);
                fifo.read (buffer);

                // Always write to retrospective buffer
                {
                    const juce::SpinLock::ScopedLockType sl (retroLock);
                    retrospectiveBuffer.ensureFreeSpace (ready);
                    retrospectiveBuffer.write (buffer);
                }

                // Handle recording
                auto mode = recordMode.load();

                if (mode == RecordMode::waitingForTrigger)
                {
                    // Check if audio level exceeds threshold (-80 dB)
                    float maxLevel = 0.0f;
                    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                        maxLevel = std::max (maxLevel, buffer.getMagnitude (ch, 0, buffer.getNumSamples()));

                    constexpr float triggerThreshold = 0.0001f; // -80 dB
                    if (maxLevel >= triggerThreshold)
                    {
                        createNewFile();

                        // Write pre-roll from retrospective buffer (0.1 seconds before trigger)
                        {
                            const juce::SpinLock::ScopedLockType sl (retroLock);

                            const int availableSamples = retrospectiveBuffer.getNumReady();

                            if (availableSamples > 0)
                            {
                                // Peek entire buffer
                                ScratchBuffer fullBuffer (retrospectiveBuffer.getNumChannels(), availableSamples);
                                retrospectiveBuffer.peek (fullBuffer);

                                // Only write the last 0.1 seconds
                                const int preRollSamples = std::min (int (sampleRate * 0.1), availableSamples);
                                const int startSample = availableSamples - preRollSamples;

                                const juce::ScopedLock wl (writerLock);
                                if (writer != nullptr)
                                    writer->writeFromAudioSampleBuffer (fullBuffer, startSample, preRollSamples);
                            }
                        }

                        recordMode.store (RecordMode::triggered);
                        mode = RecordMode::triggered;

                        if (onRecordingStarted)
                            juce::MessageManager::callAsync (onRecordingStarted);
                    }
                }

                if (mode == RecordMode::normal || mode == RecordMode::triggered)
                {
                    const juce::ScopedLock sl (writerLock);
                    if (writer != nullptr)
                        writer->writeFromAudioSampleBuffer (buffer, 0, buffer.getNumSamples());
                }
            }

            if (fifo.getNumReady() == 0)
                juce::Thread::sleep (10);
        }
    }

private:
    void createNewFile()
    {
        auto time = juce::Time::getCurrentTime();
        auto filename = time.formatted ("%Y-%m-%d_%H-%M-%S") + ".wav";
        auto file = destFolder.getChildFile (filename);

        juce::WavAudioFormat wavFormat;
        std::unique_ptr<juce::OutputStream> stream (file.createOutputStream());

        if (stream != nullptr)
        {
            auto options = juce::AudioFormatWriterOptions()
                .withSampleRate (sampleRate)
                .withChannelLayout (juce::AudioChannelSet::canonicalChannelSet (fifo.getNumChannels()))
                .withBitsPerSample (16);

            const juce::ScopedLock sl (writerLock);
            writer = wavFormat.createWriterFor (stream, options);
        }
    }

    AudioFifo& fifo;
    AudioFifo retrospectiveBuffer;
    juce::SpinLock retroLock;
    juce::File destFolder { juce::File::getSpecialLocation (juce::File::userDesktopDirectory) };

    std::atomic<RecordMode> recordMode { RecordMode::idle };
    double sampleRate = 44100.0;
    juce::CriticalSection writerLock;
    std::unique_ptr<juce::AudioFormatWriter> writer;
};

class StandaloneAudioProcessorPlayer : public gin::AudioProcessorPlayer
{
public:
    StandaloneAudioProcessorPlayer (bool doDoublePrecisionProcessing = false)
        : gin::AudioProcessorPlayer (doDoublePrecisionProcessing)
	{
        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);
        xyFifo.setSize (2, 44100);
        recordFifo.setSize (2, 44100);

        midiPlayer.setFallbackBpm (120.0);
        midiPlayer.setLooping (true);
	}

    void audioDeviceAboutToStart (juce::AudioIODevice* ioDevice) override
    {
        gin::AudioProcessorPlayer::audioDeviceAboutToStart (ioDevice);

        auto sr = ioDevice->getCurrentSampleRate();
        auto numChannels = ioDevice->getActiveOutputChannels().countNumberOfSetBits();
        if (numChannels == 0)
            numChannels = 2;

        scopeFifo.setSize (numChannels, int (sr));
        spectrumFifo.setSize (numChannels, int (sr));
        xyFifo.setSize (2, int (sr));  // XY scope always uses 2 channels (L/R)
        recordFifo.setSize (numChannels, int (sr));

        midiPlayer.setSampleRate (sr);
        samplePlayer.setPlaybackSampleRate (sr);
        audioRecorder.setSampleRate (sr);
    }

    void preProcessBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override
    {
        if (midiPlayer.hasFileLoaded())
        {
            playhead.setPositionInfo (midiPlayer.populatePositionInfo());
            processor->setPlayHead (&playhead);
        }
        midiPlayer.processBlock (buffer.getNumSamples(), midi);

        if (samplePlayer.hasFileLoaded())
        {
            playhead.setPositionInfo (samplePlayer.populatePositionInfo());
            processor->setPlayHead (&playhead);
        }
        samplePlayer.processBlock (buffer);
    }

    void postProcessBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
        if (scopeFifo.getFreeSpace() >= buffer.getNumSamples())
            scopeFifo.write (buffer);

        if (spectrumFifo.getFreeSpace() >= buffer.getNumSamples())
            spectrumFifo.write (buffer);

        // Write first 2 channels to XY fifo
        if (buffer.getNumChannels() >= 2 && xyFifo.getFreeSpace() >= buffer.getNumSamples())
        {
            ScratchBuffer stereoBuffer (2, buffer.getNumSamples());
            stereoBuffer.copyFrom (0, 0, buffer, 0, 0, buffer.getNumSamples());
            stereoBuffer.copyFrom (1, 0, buffer, 1, 0, buffer.getNumSamples());
            xyFifo.write (stereoBuffer);
        }

        if (recordFifo.getFreeSpace() >= buffer.getNumSamples())
            recordFifo.write (buffer);

        // Apply output gain last - only affects audio output, not visualizations
        float gain = outputGain.load();
        if (gain != 1.0f)
            buffer.applyGain (gain);

        processor->setPlayHead (nullptr);
	}

    void setOutputGain (float gain) { outputGain.store (gain); }
    float getOutputGain() const { return outputGain.load(); }

    AudioFifo           scopeFifo;
    AudioFifo           spectrumFifo;
    AudioFifo           xyFifo;
    AudioFifo           recordFifo;
    MidiFilePlayer      midiPlayer;
    SamplePlayer        samplePlayer;
    AudioRecorder       audioRecorder { recordFifo };
    StandalonePlayhead  playhead;

private:
    std::atomic<float>  outputGain { 1.0f };
};

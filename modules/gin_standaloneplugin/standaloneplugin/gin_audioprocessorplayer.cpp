/** Sets up `channels` so that it contains channel pointers suitable for passing to
    an AudioProcessor's processBlock.

    On return, `channels` will hold `max (processorIns, processorOuts)` entries.
    The first `processorIns` entries will point to buffers holding input data.
    Any entries after the first `processorIns` entries will point to zeroed buffers.

    In the case that the system only provides a single input channel, but the processor
    has been initialised with multiple input channels, the system input will be copied
    to all processor inputs.

    In the case that the system provides no input channels, but the processor has
    been initialised with multiple input channels, the processor's input channels will
    all be zeroed.

    @param ins            the system inputs.
    @param outs           the system outputs.
    @param numSamples     the number of samples in the system buffers.
    @param processorIns   the number of input channels requested by the processor.
    @param processorOuts  the number of output channels requested by the processor.
    @param tempBuffer     temporary storage for inputs that don't have a corresponding output.
    @param channels       holds pointers to each of the processor's audio channels.
*/
static void initialiseIoBuffers (juce::Span<const float* const> ins,
                                 juce::Span<float* const> outs,
                                 const int numSamples,
                                 size_t processorIns,
                                 size_t processorOuts,
                                 juce::AudioBuffer<float>& tempBuffer,
                                 std::vector<float*>& channels)
{
    const auto totalNumChannels = juce::jmax (processorIns, processorOuts);

    jassert (channels.capacity() >= totalNumChannels);
    jassert ((size_t) tempBuffer.getNumChannels() >= totalNumChannels);
    jassert (tempBuffer.getNumSamples() >= numSamples);

    channels.resize (totalNumChannels);

    const auto numBytes = (size_t) numSamples * sizeof (float);

    size_t tempBufferIndex = 0;

    for (size_t i = 0; i < totalNumChannels; ++i)
    {
        auto*& channelPtr = channels[i];
        channelPtr = i < outs.size()
                   ? outs[i]
                   : tempBuffer.getWritePointer ((int) tempBufferIndex++);

        // If there's a single input channel, route it to all inputs on the processor
        if (ins.size() == 1 && i < processorIns)
            memcpy (channelPtr, ins.front(), numBytes);

        // Otherwise, if there's a system input corresponding to this channel, use that
        else if (i < ins.size())
            memcpy (channelPtr, ins[i], numBytes);

        // Otherwise, silence the channel
        else
            juce::zeromem (channelPtr, numBytes);
    }

    // Zero any output channels that won't be written by the processor
    for (size_t i = totalNumChannels; i < outs.size(); ++i)
        juce::zeromem (outs[i], numBytes);
}

//==============================================================================
AudioProcessorPlayer::AudioProcessorPlayer (bool doDoublePrecisionProcessing)
    : isDoublePrecision (doDoublePrecisionProcessing)
{
}

AudioProcessorPlayer::~AudioProcessorPlayer()
{
    setProcessor (nullptr);
}

//==============================================================================
AudioProcessorPlayer::NumChannels AudioProcessorPlayer::findMostSuitableLayout (const juce::AudioProcessor& proc) const
{
    if (proc.isMidiEffect())
        return {};

    std::vector<NumChannels> layouts { deviceChannels };

    if (deviceChannels.ins == 0 || deviceChannels.ins == 1)
    {
        layouts.emplace_back (defaultProcessorChannels.ins, deviceChannels.outs);
        layouts.emplace_back (deviceChannels.outs, deviceChannels.outs);
    }

    const auto it = std::find_if (layouts.begin(), layouts.end(), [&] (const NumChannels& chans)
    {
        return proc.checkBusesLayoutSupported (chans.toLayout());
    });

    if (it == layouts.end())
        return defaultProcessorChannels;

    return *it;
}

void AudioProcessorPlayer::resizeChannels()
{
    const auto maxChannels = juce::jmax (deviceChannels.ins,
                                   deviceChannels.outs,
                                   actualProcessorChannels.ins,
                                   actualProcessorChannels.outs);
    channels.resize ((size_t) maxChannels);
    tempBuffer.setSize (maxChannels, blockSize);
}

void AudioProcessorPlayer::setProcessor (juce::AudioProcessor* const processorToPlay)
{
    const juce::ScopedLock sl (lock);

    if (processor == processorToPlay)
        return;

    sampleCount = 0;
    currentWorkgroup.reset();

    if (processorToPlay != nullptr && sampleRate > 0 && blockSize > 0)
    {
        defaultProcessorChannels = NumChannels { processorToPlay->getBusesLayout() };
        actualProcessorChannels  = findMostSuitableLayout (*processorToPlay);

        if (processorToPlay->isMidiEffect())
            processorToPlay->setRateAndBufferSizeDetails (sampleRate, blockSize);
        else
            processorToPlay->setPlayConfigDetails (actualProcessorChannels.ins,
                                                   actualProcessorChannels.outs,
                                                   sampleRate,
                                                   blockSize);

        auto supportsDouble = processorToPlay->supportsDoublePrecisionProcessing() && isDoublePrecision;

        processorToPlay->setProcessingPrecision (supportsDouble ? juce::AudioProcessor::doublePrecision
                                                                : juce::AudioProcessor::singlePrecision);

        processorToPlay->prepareToPlay (sampleRate, blockSize);
    }

    juce::AudioProcessor* oldOne = nullptr;

    oldOne = isPrepared ? processor : nullptr;
    processor = processorToPlay;
    isPrepared = true;
    resizeChannels();

    if (oldOne != nullptr)
        oldOne->releaseResources();
}

void AudioProcessorPlayer::setDoublePrecisionProcessing (bool doublePrecision)
{
    if (doublePrecision != isDoublePrecision)
    {
        const juce::ScopedLock sl (lock);

        currentWorkgroup.reset();

        if (processor != nullptr)
        {
            processor->releaseResources();

            auto supportsDouble = processor->supportsDoublePrecisionProcessing() && doublePrecision;

            processor->setProcessingPrecision (supportsDouble ? juce::AudioProcessor::doublePrecision
                                                              : juce::AudioProcessor::singlePrecision);

            processor->prepareToPlay (sampleRate, blockSize);
        }

        isDoublePrecision = doublePrecision;
    }
}

void AudioProcessorPlayer::setMidiOutput (juce::MidiOutput* midiOutputToUse)
{
    if (midiOutput != midiOutputToUse)
    {
        const juce::ScopedLock sl (lock);
        midiOutput = midiOutputToUse;
    }
}

//==============================================================================
void AudioProcessorPlayer::audioDeviceIOCallbackWithContext (const float* const* const inputChannelData,
                                                             const int numInputChannels,
                                                             float* const* const outputChannelData,
                                                             const int numOutputChannels,
                                                             const int numSamples,
                                                             const juce::AudioIODeviceCallbackContext& context)
{
    const juce::ScopedLock sl (lock);

    jassert (currentDevice != nullptr);

    // These should have been prepared by audioDeviceAboutToStart()...
    jassert (sampleRate > 0 && blockSize > 0);

    incomingMidi.clear();
    messageCollector.removeNextBlockOfMessages (incomingMidi, numSamples);

    initialiseIoBuffers ({ inputChannelData,  (size_t) numInputChannels },
                         { outputChannelData, (size_t) numOutputChannels },
                         numSamples,
                         (size_t) actualProcessorChannels.ins,
                         (size_t) actualProcessorChannels.outs,
                         tempBuffer,
                         channels);

    const auto totalNumChannels = juce::jmax (actualProcessorChannels.ins, actualProcessorChannels.outs);
    juce::AudioBuffer<float> buffer (channels.data(), (int) totalNumChannels, numSamples);

    if (processor != nullptr)
    {
        const juce::ScopedLock sl2 (processor->getCallbackLock());

        if (std::exchange (currentWorkgroup, currentDevice->getWorkgroup()) != currentDevice->getWorkgroup())
            processor->audioWorkgroupContextChanged (currentWorkgroup);

        class PlayHead final : private juce::AudioPlayHead
        {
        public:
            PlayHead (juce::AudioProcessor& proc,
                      juce::Optional<uint64_t> hostTimeIn,
                      uint64_t sampleCountIn,
                      double sampleRateIn)
                : processor (proc),
                  hostTimeNs (hostTimeIn),
                  sampleCount (sampleCountIn),
                  seconds ((double) sampleCountIn / sampleRateIn)
            {
                if (useThisPlayhead)
                    processor.setPlayHead (this);
            }

            ~PlayHead() override
            {
                if (useThisPlayhead)
                    processor.setPlayHead (nullptr);
            }

        private:
            juce::Optional<PositionInfo> getPosition() const override
            {
                PositionInfo info;
                info.setHostTimeNs (hostTimeNs);
                info.setTimeInSamples ((int64_t) sampleCount);
                info.setTimeInSeconds (seconds);
                return info;
            }

            juce::AudioProcessor& processor;
            juce::Optional<uint64_t> hostTimeNs;
            uint64_t sampleCount;
            double seconds;
            bool useThisPlayhead = processor.getPlayHead() == nullptr;
        };

        PlayHead playHead { *processor,
                            context.hostTimeNs != nullptr ? juce::makeOptional (*context.hostTimeNs) : juce::nullopt,
                            sampleCount,
                            sampleRate };

        sampleCount += (uint64_t) numSamples;

        if (! processor->isSuspended())
        {
            if (processor->isUsingDoublePrecision())
            {
                conversionBuffer.makeCopyOf (buffer, true);
                preProcessBlock (conversionBuffer, incomingMidi);
                processor->processBlock (conversionBuffer, incomingMidi);
                postProcessBlock (conversionBuffer, incomingMidi);
                buffer.makeCopyOf (conversionBuffer, true);
            }
            else
            {
                preProcessBlock (buffer, incomingMidi);
                processor->processBlock (buffer, incomingMidi);
                postProcessBlock (buffer, incomingMidi);
            }

            if (midiOutput != nullptr)
            {
                if (midiOutput->isBackgroundThreadRunning())
                {
                    midiOutput->sendBlockOfMessages (incomingMidi,
                                                     juce::Time::getMillisecondCounterHiRes(),
                                                     sampleRate);
                }
                else
                {
                    midiOutput->sendBlockOfMessagesNow (incomingMidi);
                }
            }

            return;
        }
    }

    for (int i = 0; i < numOutputChannels; ++i)
        juce::FloatVectorOperations::clear (outputChannelData[i], numSamples);
}

void AudioProcessorPlayer::audioDeviceAboutToStart (juce::AudioIODevice* const device)
{
    currentDevice = device;
    auto newSampleRate = device->getCurrentSampleRate();
    auto newBlockSize  = device->getCurrentBufferSizeSamples();
    auto numChansIn    = device->getActiveInputChannels().countNumberOfSetBits();
    auto numChansOut   = device->getActiveOutputChannels().countNumberOfSetBits();

    const juce::ScopedLock sl (lock);

    sampleRate = newSampleRate;
    blockSize  = newBlockSize;
    deviceChannels = { numChansIn, numChansOut };

    resizeChannels();

    messageCollector.reset (sampleRate);

    currentWorkgroup.reset();

    if (processor != nullptr)
    {
        if (isPrepared)
            processor->releaseResources();

        auto* oldProcessor = processor;
        setProcessor (nullptr);
        setProcessor (oldProcessor);
    }
}

void AudioProcessorPlayer::audioDeviceStopped()
{
    const juce::ScopedLock sl (lock);

    if (processor != nullptr && isPrepared)
        processor->releaseResources();

    sampleRate = 0.0;
    blockSize = 0;
    isPrepared = false;
    tempBuffer.setSize (1, 1);

    currentDevice = nullptr;
    currentWorkgroup.reset();
}

void AudioProcessorPlayer::handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage& message)
{
    messageCollector.addMessageToQueue (message);
}

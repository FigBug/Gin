/**
    Wrapper for JUCE AudioProcessor that forwards all calls to a wrapped instance.

    AudioProcessorWrapper provides a complete pass-through wrapper for any JUCE
    AudioProcessor. All virtual methods are forwarded to the wrapped processor,
    allowing for interception, monitoring, or modification of processor behavior
    without modifying the original processor code.

    Key Features:
    - Complete forwarding of all AudioProcessor methods
    - Ownership management of wrapped processor
    - No overhead when not overriding methods
    - Useful for debugging, testing, or adding cross-cutting concerns
    - VST2/VST3 extension support

    Use Cases:
    - Adding logging/debugging to existing processors
    - Implementing cross-cutting features (metering, analysis)
    - Testing and validation wrappers
    - Plugin format adapter patterns

    Usage:
    @code
    // Wrap an existing processor
    auto myProcessor = std::make_unique<MyAudioProcessor>();
    auto wrapper = std::make_unique<AudioProcessorWrapper>(std::move(myProcessor));

    // Can also subclass to intercept specific methods
    class LoggingWrapper : public AudioProcessorWrapper
    {
        void processBlock(juce::AudioBuffer<float>& buffer,
                         juce::MidiBuffer& midi) override
        {
            DBG("Processing " + String(buffer.getNumSamples()) + " samples");
            AudioProcessorWrapper::processBlock(buffer, midi);
        }
    };
    @endcode

    @see juce::AudioProcessor
*/
class AudioProcessorWrapper : public juce::AudioProcessor
{
public:
	AudioProcessorWrapper (std::unique_ptr<juce::AudioProcessor> p)
		: proc (std::move (p))
	{
	}

	~AudioProcessorWrapper() override {}

    //==============================================================================
	const juce::String getName() const override { return proc->getName(); }
	juce::StringArray getAlternateDisplayNames() const override { return proc->getAlternateDisplayNames(); }

    //==============================================================================
    void setRateAndBufferSizeDetails (double newSampleRate, int newBlockSize) noexcept
    {
        juce::AudioProcessor::setRateAndBufferSizeDetails (newSampleRate, newBlockSize);
        proc->setRateAndBufferSizeDetails (newSampleRate, newBlockSize);
    }

	void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override
	{
		proc->prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
	}

	void releaseResources() override { proc->releaseResources(); }
	void memoryWarningReceived() override { proc->memoryWarningReceived(); }

  
	void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		proc->processBlock (buffer, midiMessages);
	}

	void processBlock (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override
	{
		proc->processBlock (buffer, midiMessages);
	}

	void processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		proc->processBlock (buffer, midiMessages);
	}

	void processBlockBypassed (juce::AudioBuffer<double>& buffer, juce::MidiBuffer& midiMessages) override
	{
		proc->processBlock (buffer, midiMessages);
	}

	bool canAddBus (bool isInput) const override { return proc->canAddBus (isInput); }
	bool canRemoveBus (bool isInput) const override { return proc->canRemoveBus (isInput); }
	bool supportsDoublePrecisionProcessing() const override { return proc->supportsDoublePrecisionProcessing(); }
	double getTailLengthSeconds() const override { return proc->getTailLengthSeconds(); }
	bool acceptsMidi() const override { return proc->acceptsMidi(); }
	bool producesMidi() const override { return proc->producesMidi(); }
	bool supportsMPE() const override { return proc->supportsMPE(); }
	bool isMidiEffect() const override { return proc->isMidiEffect(); }
	void reset() override { proc->reset(); }
	juce::AudioProcessorParameter* getBypassParameter() const override { return proc->getBypassParameter(); }
	void setNonRealtime (bool isNonRealtime) noexcept override { proc->setNonRealtime (isNonRealtime); }
	juce::AudioProcessorEditor* createEditor() override { return proc->createEditor(); }
	bool hasEditor() const override { return proc->hasEditor(); }
	void refreshParameterList() override { return proc->refreshParameterList(); }
	int getNumPrograms() override { return proc->getNumPrograms(); }
	int getCurrentProgram() override { return proc->getCurrentProgram(); }
	void setCurrentProgram (int index) override { proc->setCurrentProgram (index); }
	const juce::String getProgramName (int index) override { return proc->getProgramName (index); }
	void changeProgramName (int index, const juce::String& newName) override { proc->changeProgramName (index, newName); }
	void getStateInformation (juce::MemoryBlock& destData) override { proc->getStateInformation (destData); }
	void getCurrentProgramStateInformation (juce::MemoryBlock& destData) override { proc->getCurrentProgramStateInformation (destData); }
	void setStateInformation (const void* data, int sizeInBytes) override { return proc->setStateInformation (data, sizeInBytes); }
	void setCurrentProgramStateInformation (const void* data, int sizeInBytes) override { proc->setCurrentProgramStateInformation (data, sizeInBytes); }
	void numChannelsChanged() override { proc->numChannelsChanged(); }
	void numBusesChanged() override { proc->numBusesChanged(); }
	void processorLayoutsChanged() override { proc->processorLayoutsChanged(); }

	void addListener (juce::AudioProcessorListener* newListener) override
	{
		proc->addListener (newListener);
	}

	void removeListener (juce::AudioProcessorListener* listenerToRemove) override
	{
		proc->removeListener (listenerToRemove);
	}

    //==============================================================================
	void setPlayHead (juce::AudioPlayHead* newPlayHead) override
	{
		proc->setPlayHead (newPlayHead);
	}

	void audioWorkgroupContextChanged (const juce::AudioWorkgroup& workgroup) override
	{
		proc->audioWorkgroupContextChanged (workgroup);
	}

	juce::VST2ClientExtensions* getVST2ClientExtensions() override
	{
		return proc->getVST2ClientExtensions();
	}

	juce::VST3ClientExtensions* getVST3ClientExtensions() override
	{
		return proc->getVST3ClientExtensions();
	}

	CurveData getResponseCurve (CurveData::Type curveType) const override
	{
		return proc->getResponseCurve (curveType);
	}

	void updateTrackProperties (const TrackProperties& properties) override
	{
		return proc->updateTrackProperties (properties);
	}

   #if JUCE_MAJOR_VERSION >= 8
	std::optional<juce::String> getNameForMidiNoteNumber (int note, int midiChannel) override
	{
		return proc->getNameForMidiNoteNumber (note, midiChannel);
	}
   #endif

	std::unique_ptr<juce::AudioProcessor> proc;
};

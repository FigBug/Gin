
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

	std::optional<juce::String> getNameForMidiNoteNumber (int note, int midiChannel) override
	{
		return proc->getNameForMidiNoteNumber (note, midiChannel);
	}
	
	std::unique_ptr<juce::AudioProcessor> proc;
};

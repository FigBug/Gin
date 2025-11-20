#pragma once

class PluginWrapper : public AudioProcessorWrapper
{
public:
	PluginWrapper (std::unique_ptr<juce::AudioProcessor> p) : AudioProcessorWrapper (std::move (p))
	{
        scopeFifo.setSize (2, 44100);
	}
	
	void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override
	{
		AudioProcessorWrapper::prepareToPlay (sampleRate, maximumExpectedSamplesPerBlock);
	}
	
	void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
	{
		AudioProcessorWrapper::processBlock (buffer, midiMessages);

        if (buffer.getNumChannels() != scopeFifo.getNumChannels())
            scopeFifo.setSize (2, 44100);

        if (scopeFifo.getFreeSpace() >= buffer.getNumSamples())
            scopeFifo.write (buffer);
	}

    AudioFifo   scopeFifo;
};

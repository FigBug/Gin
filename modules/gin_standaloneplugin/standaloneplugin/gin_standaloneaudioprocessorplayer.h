#pragma once

class StandaloneAudioProcessorPlayer : public gin::AudioProcessorPlayer
{
public:
    StandaloneAudioProcessorPlayer (bool doDoublePrecisionProcessing = false)
        : gin::AudioProcessorPlayer (doDoublePrecisionProcessing)
	{
        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);
	}

    void postProcessBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
	{
        {
            if (buffer.getNumChannels() != scopeFifo.getNumChannels())
                scopeFifo.setSize (buffer.getNumChannels(), 44100);

            if (scopeFifo.getFreeSpace() >= buffer.getNumSamples())
                scopeFifo.write (buffer);
        }

        {
            if (buffer.getNumChannels() != spectrumFifo.getNumChannels())
                spectrumFifo.setSize (buffer.getNumChannels(), 44100);

            if (spectrumFifo.getFreeSpace() >= buffer.getNumSamples())
                spectrumFifo.write (buffer);
        }
	}

    AudioFifo   scopeFifo;
    AudioFifo   spectrumFifo;
};

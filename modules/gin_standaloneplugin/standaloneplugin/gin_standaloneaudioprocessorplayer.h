#pragma once

class StandaloneAudioProcessorPlayer : public gin::AudioProcessorPlayer
{
public:
    StandaloneAudioProcessorPlayer (bool doDoublePrecisionProcessing = false)
        : gin::AudioProcessorPlayer (doDoublePrecisionProcessing)
	{
        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);

        midiPlayer.setBpm (120.0);
        midiPlayer.setLooping (true);
	}

    void audioDeviceAboutToStart (juce::AudioIODevice* ioDevice) override
    {
        gin::AudioProcessorPlayer::audioDeviceAboutToStart (ioDevice);

        midiPlayer.setSampleRate (ioDevice->getCurrentSampleRate());
    }

    void preProcessBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi) override
    {
        midiPlayer.processBlock (buffer.getNumSamples(), midi);
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

    AudioFifo       scopeFifo;
    AudioFifo       spectrumFifo;
    MidiFilePlayer  midiPlayer;
};

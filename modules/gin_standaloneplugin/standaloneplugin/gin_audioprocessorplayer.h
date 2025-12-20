#pragma once

//==============================================================================
/**
    An AudioIODeviceCallback object which streams audio through an AudioProcessor.

    To use one of these, just make it the callback used by your AudioIODevice, and
    give it a processor to use by calling setProcessor().

    It's also a MidiInputCallback, so you can connect it to both an audio and midi
    input to send both streams through the processor. To set a MidiOutput for the processor,
    use the setMidiOutput() method.

    This is an improvement on the juce version as it allows pre/post processing the
    audio and midi data

    @see AudioProcessor, AudioProcessorGraph

    @tags{Audio}
*/
class AudioProcessorPlayer : public juce::AudioIODeviceCallback,
                             public juce::MidiInputCallback
{
public:
    //==============================================================================
    AudioProcessorPlayer (bool doDoublePrecisionProcessing = false);

    /** Destructor. */
    ~AudioProcessorPlayer() override;

    //==============================================================================
    /** Sets the processor that should be played.

        The processor that is passed in will not be deleted or owned by this object.
        To stop anything playing, pass a nullptr to this method.
    */
    virtual void setProcessor (juce::AudioProcessor* processorToPlay);

    /** Returns the current audio processor that is being played. */
    juce::AudioProcessor* getCurrentProcessor() const noexcept            { return processor; }

    /** Returns a midi message collector that you can pass midi messages to if you
        want them to be injected into the midi stream that is being sent to the
        processor.
    */
    juce::MidiMessageCollector& getMidiMessageCollector() noexcept        { return messageCollector; }

    /** Sets the MIDI output that should be used, if required.

        The MIDI output will not be deleted or owned by this object. If the MIDI output is
        deleted, pass a nullptr to this method.
    */
    void setMidiOutput (juce::MidiOutput* midiOutputToUse);

    /** Switch between double and single floating point precisions processing.

        The audio IO callbacks will still operate in single floating point precision,
        however, all internal processing including the AudioProcessor will be processed in
        double floating point precision if the AudioProcessor supports it (see
        AudioProcessor::supportsDoublePrecisionProcessing()). Otherwise, the processing will
        remain single precision irrespective of the parameter doublePrecision.
    */
    void setDoublePrecisionProcessing (bool doublePrecision);

    /** Returns true if this player processes internally processes the samples with
        double floating point precision.
    */
    inline bool getDoublePrecisionProcessing() { return isDoublePrecision; }

    virtual void preProcessBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) {}
    virtual void postProcessBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) {}

    virtual void preProcessBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) {}
    virtual void postProcessBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) {}

    //==============================================================================
    /** @internal */
    void audioDeviceIOCallbackWithContext (const float* const*, int, float* const*, int, int, const juce::AudioIODeviceCallbackContext&) override;
    /** @internal */
    void audioDeviceAboutToStart (juce::AudioIODevice*) override;
    /** @internal */
    void audioDeviceStopped() override;
    /** @internal */
    void handleIncomingMidiMessage (juce::MidiInput*, const juce::MidiMessage&) override;

protected:
    struct NumChannels
    {
        NumChannels() = default;
        NumChannels (int numIns, int numOuts) : ins (numIns), outs (numOuts) {}

        explicit NumChannels (const juce::AudioProcessor::BusesLayout& layout)
            : ins (layout.getNumChannels (true, 0)), outs (layout.getNumChannels (false, 0)) {}

        juce::AudioProcessor::BusesLayout toLayout() const
        {
            return { { juce::AudioChannelSet::canonicalChannelSet (ins) },
                     { juce::AudioChannelSet::canonicalChannelSet (outs) } };
        }

        int ins = 0, outs = 0;
    };

    //==============================================================================
    NumChannels findMostSuitableLayout (const juce::AudioProcessor&) const;
    void resizeChannels();

    //==============================================================================
    juce::AudioProcessor* processor = nullptr;
    juce::CriticalSection lock;
    double sampleRate = 0;
    int blockSize = 0;
    bool isPrepared = false, isDoublePrecision = false;

    NumChannels deviceChannels, defaultProcessorChannels, actualProcessorChannels;
    std::vector<float*> channels;
    juce::AudioBuffer<float> tempBuffer;
    juce::AudioBuffer<double> conversionBuffer;

    juce::MidiBuffer incomingMidi;
    juce::MidiMessageCollector messageCollector;
    juce::MidiOutput* midiOutput = nullptr;
    uint64_t sampleCount = 0;

    juce::AudioIODevice* currentDevice = nullptr;
    juce::AudioWorkgroup currentWorkgroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioProcessorPlayer)
};

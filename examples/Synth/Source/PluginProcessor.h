#pragma once

#include <JuceHeader.h>

#include "SynthVoice.h"

//==============================================================================
class SynthAudioProcessor : public gin::Processor,
                            public gin::Synthesiser
{
public:
    //==============================================================================
    SynthAudioProcessor();
    ~SynthAudioProcessor() override;

    void stateUpdated() override;
    void updateState() override;

    //==============================================================================
    void reset() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    gin::BandLimitedLookupTables bandLimitedLookupTables;

    //==============================================================================
    void handleMidiEvent (const juce::MidiMessage& m) override;
    void handleController (int ch, int num, int val) override;
    //==============================================================================
    juce::Array<float> getLiveFilterCutoff();

    // Voice Params
    struct OSCParams
    {
        OSCParams() = default;

        gin::Parameter::Ptr wave, voices, voicesTrns, detune, spread;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (OSCParams)
    };

    struct FilterParams
    {
        FilterParams() = default;

        gin::Parameter::Ptr type, keyTracking, velocityTracking,
                            frequency, resonance, amount,
                            attack, decay, sustain, release;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (FilterParams)
    };

    struct ADSRParams
    {
        ADSRParams() = default;

        gin::Parameter::Ptr attack, decay, sustain, release, velocityTracking;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (ADSRParams)
    };

    //==============================================================================
    OSCParams oscParams;
    FilterParams filterParams;
    ADSRParams adsrParams;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthAudioProcessor)
};

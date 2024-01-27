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

    bool supportsMPE() const override { return true; }

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

    void updateParams (int blockSize);
    void setupModMatrix();

    //==============================================================================
    void handleMidiEvent (const juce::MidiMessage& m) override;
    //==============================================================================
    juce::Array<float> getLiveFilterCutoff();

    void applyEffects (juce::AudioSampleBuffer& buffer);

    // Voice Params
    struct OSCParams
    {
        OSCParams() = default;

        gin::Parameter::Ptr enable, wave, voices, tune, finetune, level, detune, spread, pan;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (OSCParams)
    };

    struct FilterParams
    {
        FilterParams() = default;

        gin::Parameter::Ptr enable, type, keyTracking, frequency, resonance;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (FilterParams)
    };

    struct LFOParams
    {
        LFOParams() = default;

        gin::Parameter::Ptr enable, sync, wave, rate, beat, depth, phase, offset, fade, delay;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (LFOParams)
    };

    struct ADSRParams
    {
        ADSRParams() = default;

        gin::Parameter::Ptr attack, decay, sustain, release, velocityTracking;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (ADSRParams)
    };

    // Global Params
    struct GlobalParams
    {
        GlobalParams() = default;

        gin::Parameter::Ptr mono, glideMode, glideRate, legato, level, voices, mpe;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (GlobalParams)
    };

    struct DelayParams
    {
        DelayParams() = default;

        gin::Parameter::Ptr enable, sync, time, beat, fb, cf, mix, delay;

        void setup (SynthAudioProcessor& p);

        JUCE_DECLARE_NON_COPYABLE (DelayParams)
    };

    //==============================================================================
    gin::ModSrcId modSrcPressure, modSrcTimbre, modScrPitchBend, modSrcLFO,
                  modSrcNote, modSrcVelocity, modSrcMonoLFO;

    //==============================================================================

    OSCParams oscParams;
    FilterParams filterParams;
    LFOParams lfoParams;

    ADSRParams adsrParams;

    GlobalParams globalParams;
    DelayParams delayParams;

    //==============================================================================
    gin::StereoDelay stereoDelay { 120.1 };
    gin::GainProcessor outputGain;
    gin::AudioFifo scopeFifo { 2, 44100 };

    gin::BandLimitedLookupTables analogTables;

    //==============================================================================
    gin::ModMatrix modMatrix;

    gin::LFO modLFO;

    juce::AudioPlayHead* playhead = nullptr;
    bool presetLoaded = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SynthAudioProcessor)
};

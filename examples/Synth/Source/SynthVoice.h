#pragma once

#include <JuceHeader.h>

class SynthAudioProcessor;

//==============================================================================
class SynthVoice : public gin::SynthesiserVoice
{
public:
    SynthVoice (SynthAudioProcessor& p, gin::BandLimitedLookupTables& bandLimitedLookupTables);

    void noteStarted() override;
    void noteRetriggered() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override;
    void noteTimbreChanged() override;
    void notePitchbendChanged() override    {}
    void noteKeyStateChanged() override     {}

    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    float getFilterCutoffNormalized();

private:
    void updateParams (int blockSize);
    float getValue (gin::Parameter* p);

    SynthAudioProcessor& proc;
    gin::BandLimitedLookupTables& bandLimitedLookupTables;
    gin::BLLTVoicedStereoOscillator oscillators { bandLimitedLookupTables };

    gin::Filter filters;
    gin::ADSR filterADSRs;
    gin::AnalogADSR adsr;

    float currentMidiNotes = 0.0f;
    gin::VoicedStereoOscillatorParams oscParams;

    gin::EasedValueSmoother<float> noteSmoother;

    float ampKeyTrack = 1.0f;
};

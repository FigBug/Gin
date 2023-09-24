#pragma once

#include <JuceHeader.h>

class SynthAudioProcessor;

//==============================================================================
class SynthVoice : public gin::SynthesiserVoice,
                   public gin::ModVoice
{
public:
    SynthVoice (SynthAudioProcessor& p);
    
    void noteStarted() override;
    void noteRetriggered() override;
    void noteStopped (bool allowTailOff) override;

    void notePressureChanged() override;
    void noteTimbreChanged() override;
    void notePitchbendChanged() override    {}
    void noteKeyStateChanged() override     {}
    
    void setCurrentSampleRate (double newRate) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    bool isVoiceActive() override;

    float getFilterCutoffNormalized();
  
private:
    void updateParams (int blockSize);

    SynthAudioProcessor& proc;

    gin::BLLTVoicedStereoOscillator oscillator;

    gin::Filter filter;
    
    gin::LFO modLFO;

    gin::AnalogADSR adsr;

    float currentMidiNote;
    gin::VoicedStereoOscillatorParams oscParams;
    
    gin::EasedValueSmoother<float> noteSmoother;
    
    float ampKeyTrack = 1.0f;
};

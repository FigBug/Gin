/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


//==============================================================================
/** WT Oscillator.
*/
class WTOscillator
{
public:
    WTOscillator() = default;

    struct Params
    {
        Wave wave = Wave::wavetable;
        float leftGain = 1.0;
        float rightGain = 1.0;
        float pw = 0.0;
    };

    void setSampleRate (double sr)  { sampleRate = sr; }
    void noteOn (float p = -1);

    void process (float note, const Params& params, juce::AudioSampleBuffer& buffer);
    void process (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer);

    void processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer);
    void processAdding (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer);

    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>& table);

private:
    juce::Array<BandLimitedLookupTable*> bllt;
    double sampleRate = 44100.0;
    float phaseL = 0.0f, phaseR = 0.0f;
};

//==============================================================================
/** Stereo Oscillator with multiples voices, pan, spread, detune, etc
 */
class WTVoicedStereoOscillator : public VoicedStereoOscillator<WTOscillator>
{
public:
    WTVoicedStereoOscillator (int maxVoices = 8)
    {
        for (int i = 0; i < maxVoices; i++)
            oscillators.add (new WTOscillator());
    }

    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>& table)
    {
        for (auto o : oscillators)
            o->setWavetable (table);
    }
};

bool loadWavetables (juce::OwnedArray<BandLimitedLookupTable>& bllt, juce::AudioSampleBuffer& buffer, double sampleRate, int tableSize);

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
        float leftGain = 1.0f;
        float rightGain = 1.0f;
        float position = 0.0f;
        float bend = 0.0f;
        float formant = 0.0f;
    };

    void setSampleRate (double sr)  { sampleRate = sr; }
    void noteOn (float p = -1);

    void process (float note, const Params& params, juce::AudioSampleBuffer& buffer);
    void process (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer);

    void processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer);
    void processAdding (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer);

    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table);

private:
    inline float bendDistortion (float phase, float bend)
    {
        const auto addDist = std::clamp (bend, 0.0f, 1.0f);
        const auto subDist = std::clamp (bend, -1.0f, 0.0f);

        const auto sub = std::pow (phase, 8.0f);
        const auto add = std::pow (1.0f - phase, 8.0f);

        const auto addMix = std::lerp (phase, 1.0f - add, addDist);
        const auto subMix = std::lerp (phase, sub, -subDist);

        return std::min (1.0f - std::numeric_limits<float>::epsilon(), addMix + subMix - phase);
    }

    inline float formantDistortion (float phase, float formant)
    {
        return std::min (1.0f - std::numeric_limits<float>::epsilon(), phase * std::exp (formant * 1.60943791243f));
    }

    inline float phaseDistortion (float phase, float bend, float formant)
    {
        if (! juce::approximatelyEqual (bend, 0.0f))
            phase = bendDistortion (phase, bend);

        if (! juce::approximatelyEqual (formant, 0.0f))
            phase = formantDistortion (phase, formant);

        return phase;
    }

    juce::OwnedArray<BandLimitedLookupTable>* bllt = nullptr;
    double sampleRate = 44100.0;
    float phaseL = 0.0f, phaseR = 0.0f;
    int tableIndexL = 0, tableIndexR = 0;
};

struct WTVoicedStereoOscillatorParams : public VoicedOscillatorParams
{
    float position = 0.5;
    float bend = 0.0f;
    float formant = 0.0f;

    inline void init (WTOscillator::Params& p) const
    {
        p.position = position;
        p.bend = bend;
        p.formant = formant;
    }
};

//==============================================================================
/** Stereo Wavetable Oscillator with multiples voices, pan, spread, detune, etc
 */
class WTVoicedStereoOscillator : public VoicedStereoOscillator<WTOscillator, WTVoicedStereoOscillatorParams>
{
public:
    WTVoicedStereoOscillator (int maxVoices = 8)
    {
        for (int i = 0; i < maxVoices; i++)
            oscillators.add (new WTOscillator());
    }

    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table)
    {
        for (auto o : oscillators)
            o->setWavetable (table);
    }
};

bool loadWavetables (juce::OwnedArray<BandLimitedLookupTable>& bllt, double playbackSampleRate, juce::AudioSampleBuffer& buffer, double fileSampleRate, int tableSize);

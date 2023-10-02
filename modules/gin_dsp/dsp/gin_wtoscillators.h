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

    void process (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        buffer.clear();
        processAdding (note, params, buffer);
    }

    void processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr && bllt->size() == 0) return;

        if (tableIndex == -1 || tableIndex >= bllt->size())
            tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

        float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
        float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

        int samps = buffer.getNumSamples();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);
        
        auto table = bllt->getUnchecked (tableIndex);

        for (int i = 0; i < samps; i++)
        {
            auto s = table->process (note, phaseDistortion (phase, params.bend, params.formant));
            *l++ += s * params.leftGain;
            *r++ += s * params.rightGain;

            phase += delta;
            while (phase >= 1.0f)
            {
                phase -= 1.0f;
                tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
                table = bllt->getUnchecked (tableIndex);
            }
        }
    }
    
    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table);

private:
    inline float bendDistortion (float phaseIn, float bend)
    {
        const auto addDist = std::clamp (bend, 0.0f, 1.0f);
        const auto subDist = std::clamp (bend, -1.0f, 0.0f);

        const auto sub = std::pow (phaseIn, 8.0f);
        const auto add = std::pow (1.0f - phaseIn, 8.0f);

        const auto addMix = std::lerp (phaseIn, 1.0f - add, addDist);
        const auto subMix = std::lerp (phaseIn, sub, -subDist);

        auto res = std::min (1.0f - std::numeric_limits<float>::epsilon(), addMix + subMix - phaseIn);
        jassert (res >= 0.0f && res < 1.0f);
        return res;
    }

    inline float formantDistortion (float phaseIn, float formant)
    {
        auto res = std::min (1.0f - std::numeric_limits<float>::epsilon(), phaseIn * std::exp (formant * 1.60943791243f));
        jassert (res >= 0.0f && res < 1.0f);
        return res;
    }

    inline float phaseDistortion (float phaseIn, float bend, float formant)
    {
        if (! juce::exactlyEqual (bend, 0.0f))
            phaseIn = bendDistortion (phaseIn, bend);

        if (! juce::exactlyEqual (formant, 0.0f))
            phaseIn = formantDistortion (phaseIn, formant);

        return phaseIn;
    }

    juce::OwnedArray<BandLimitedLookupTable>* bllt = nullptr;
    double sampleRate = 44100.0;
    float phase = 0.0f;
    int tableIndex = 0;
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

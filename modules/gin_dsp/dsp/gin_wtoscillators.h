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

    void processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr && bllt->size() == 0) return;

        if (tableIndexL == -1 || tableIndexL >= bllt->size())
            tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

        float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
        float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

        int samps = buffer.getNumSamples();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);
        
        auto table = bllt->getUnchecked (tableIndexL);

        for (int i = 0; i < samps; i++)
        {
            auto s = table->process (note, phaseDistortion (phaseL, params.bend, params.formant));
            *l++ += s * params.leftGain;
            *r++ += s * params.rightGain;

            phaseL += delta;
            while (phaseL >= 1.0f)
            {
                phaseL -= 1.0f;
                tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
                table = bllt->getUnchecked (tableIndexL);
            }
        }
        phaseR = phaseL;
    }
    
    void processAdding (float noteL, float noteR, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr && bllt->size() == 0) return;

        if (tableIndexL == -1 || tableIndexL >= bllt->size() || tableIndexR >= bllt->size())
            tableIndexL = tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

        float freqL = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteL - 69.0) / 12.0)));
        float freqR = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (noteR - 69.0) / 12.0)));
        float deltaL = 1.0f / (float ((1.0f / freqL) * sampleRate));
        float deltaR = 1.0f / (float ((1.0f / freqR) * sampleRate));

        int samps = buffer.getNumSamples();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        for (int i = 0; i < samps; i++)
        {
            auto sL = bllt->getUnchecked (tableIndexL)->process (noteL, phaseDistortion (phaseL, params.bend, params.formant));
            auto sR = bllt->getUnchecked (tableIndexR)->process (noteR, phaseDistortion (phaseR, params.bend, params.formant));

            *l++ += sL * params.leftGain;
            *r++ += sR * params.rightGain;

            phaseL += deltaL;
            phaseR += deltaR;
            while (phaseL >= 1.0f)
            {
                phaseL -= 1.0f;
                tableIndexL = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
            }
            while (phaseR >= 1.0f)
            {
                phaseR -= 1.0f;
                tableIndexR = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
            }
        }
    }
    
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
        if (! juce::exactlyEqual (bend, 0.0f))
            phase = bendDistortion (phase, bend);

        if (! juce::exactlyEqual (formant, 0.0f))
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

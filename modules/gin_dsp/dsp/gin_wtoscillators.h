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
      
        while (samps > 0)
        {
            auto todo = std::min (samps, int ((1.0f - phase) / delta) + 1);
            samps -= todo;

            for (; todo >= 4; todo -= 4)
            {
                mipp::Reg<float> phaseVec = {phase, phase + delta, phase + 2 * delta, phase + 3 * delta};
                mipp::Reg<float> lVec { l };
                mipp::Reg<float> rVec { r };

                auto s = table->process (note, phaseDistortion (phaseVec, params.bend, params.formant));

                lVec += s * params.leftGain;
                rVec += s * params.rightGain;

                lVec.store (l); l += 4;
                lVec.store (r); r += 4;

                phase += delta * 4;
            }

            for (; todo > 0; todo--)
            {
                auto s = table->process (note, phaseDistortion (phase, params.bend, params.formant));
                *l++ += s * params.leftGain;
                *r++ += s * params.rightGain;

                phase += delta;
            }

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
    template<typename T>
    T bendDistortion (T phaseIn, float bend)
    {
        const auto addDist = std::clamp (bend, 0.0f, 1.0f);
        const auto subDist = std::clamp (bend, -1.0f, 0.0f);

        const auto sub = math::pow8 (phaseIn);
        const auto add = math::pow8 (T (1.0f) - phaseIn);

        const auto addMix = math::lerp (phaseIn, T (1.0f) - add, addDist);
        const auto subMix = math::lerp (phaseIn, sub, -subDist);

        return math::min (T (1.0f - std::numeric_limits<float>::epsilon()), addMix + subMix - phaseIn);
    }

    template<typename T>
    T formantDistortion (T phaseIn, float formant)
    {
        return math::min (T (1.0f - std::numeric_limits<float>::epsilon()), phaseIn * std::exp (formant * 1.60943791243f));
    }

    template<typename T>
    T phaseDistortion (T phaseIn, float bend, float formant)
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

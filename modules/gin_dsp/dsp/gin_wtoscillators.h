/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


/**
    DC blocking filter (high-pass filter) to remove DC offset.

    DCBlocker implements a simple first-order high-pass filter to remove DC
    offset from audio signals. This is useful after certain DSP operations
    (like waveshaping or oscillators) that may introduce DC bias.

    Key Features:
    - Configurable cutoff frequency (default 10 Hz)
    - First-order IIR filter
    - Very low CPU usage
    - Reset for clearing filter state

    Usage:
    @code
    DCBlocker blocker;
    blocker.setSampleRate(44100.0f);
    blocker.setCutoff(10.0f); // Block below 10 Hz

    // Per-sample processing
    float output = blocker.process(input);
    @endcode

    Note: This is a per-sample filter. For stereo, use two instances.

    @see WTOscillator
*/
class DCBlocker
{
public:
    DCBlocker() {}
    ~DCBlocker() {}

    void setSampleRate (float sampleRate_)
    {
        sampleRate = sampleRate_;
        recalc();
    }

    void setCutoff (float cutoff_ /* Hz */)
    {
        cutoff = cutoff_;
        recalc();
    }

    float process (float x)
    {
        z = x * a + z * b;
        return z;
    }

    void reset()
    {
        a = 0;
        b = 0;
        z = 0;
    }

private:
    float sampleRate = 1;
    float cutoff = 10.0f;

    float a = 0;
    float b = 0;
    float z = 0;

    void recalc()
    {
        b = std::exp (-2.0f * juce::MathConstants<float>::pi * cutoff / sampleRate);
        a = 1.0f - b;
    }
};

//==============================================================================
/**
    Wavetable oscillator with interpolation and shaping options.

    WTOscillator provides high-quality wavetable synthesis with support for
    table morphing, formant shifting, phase distortion (bend), and post-processing
    effects like asymmetry and wavefolding. It uses band-limited wavetables to
    avoid aliasing.

    Key Features:
    - Wavetable synthesis with linear interpolation
    - Table position morphing for timbral variation
    - Formant shifting (pitch-independent spectral shift)
    - Phase distortion (bend parameter)
    - Wavefolding and asymmetry effects
    - Optional DC blocking
    - SIMD optimization where available

    Parameters (via Params struct):
    - position: Wavetable position (0.0 to 1.0)
    - bend: Phase distortion amount
    - formant: Formant shift (pitch-independent frequency shift)
    - asym: Asymmetry/waveshaping amount
    - fold: Wavefolding amount
    - leftGain/rightGain: Stereo output gains

    Usage:
    @code
    WTOscillator osc;
    osc.setSampleRate(44100.0);
    osc.setWaveTable(&myBandLimitedTables);
    osc.noteOn(); // Initialize phase

    WTOscillator::Params params;
    params.position = 0.3f;
    params.fold = 0.2f;

    osc.process(60.0f, params, audioBuffer); // MIDI note 60 (C4)
    @endcode

    @see BandLimitedLookupTable, DCBlocker
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
        float asym = 0.0f;
        float fold = 0.0f;
    };

    void setSampleRate (double sr) 
    { 
        sampleRate = sr;
        blockerL.setSampleRate (float (sr));
        blockerR.setSampleRate (float (sr));
        blockerL.setCutoff (10.0f);
        blockerR.setCutoff (10.0f);
    }

    void noteOn (float p = -1);

    void process (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        buffer.clear();
        processAdding (note, params, buffer);
    }

    void processAdding (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (tableIndex != lastTableIndex && lastTableIndex >= 0)
            processAddingCrossfadeComplex (note, params, buffer);
        else if (juce::exactlyEqual (params.bend, 0.0f) && juce::exactlyEqual (params.formant, 0.0f))
            processAddingSimple (note, params, buffer);
        else
            processAddingComplex (note, params, buffer);

        if (blockDC)
        {
            int samps = buffer.getNumSamples();
            auto l = buffer.getWritePointer (0);
            auto r = buffer.getWritePointer (1);

            while (samps > 0)
            {
                *l -= blockerL.process (*l);
                *r -= blockerR.process (*r);

                l++;
                r++;
                samps--;
            }
        }
    }

    void processAddingSimple (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr || bllt->size() == 0) return;

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

           #if GIN_HAS_SIMD
            for (; todo >= 4; todo -= 4)
            {
                mipp::Reg<float> phaseVec = {phase, phase + delta, phase + 2 * delta, phase + 3 * delta};
                mipp::Reg<float> lVec { l };
                mipp::Reg<float> rVec { r };

                auto s = table->processLinear (note, math::min (almostOne, phaseVec));
                postProcess (params, s);

                lVec += s * params.leftGain;
                rVec += s * params.rightGain;

                lVec.store (l); l += 4;
                rVec.store (r); r += 4;

                phase += delta * 4;
            }
           #endif

            for (; todo > 0; todo--)
            {
                auto s = table->processLinear (note, std::min (almostOne, phase));
                postProcess (params, s);

                *l++ += s * params.leftGain;
                *r++ += s * params.rightGain;

                phase += delta;
            }

            while (phase >= 1.0f)
            {
                phase -= 1.0f;
                lastTableIndex = tableIndex;
                tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
                table = bllt->getUnchecked (tableIndex);
            }
        }
    }

    void processAddingComplex (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr || bllt->size() == 0) return;

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

           #if GIN_HAS_SIMD
            for (; todo >= 4; todo -= 4)
            {
                mipp::Reg<float> phaseVec = {phase, phase + delta, phase + 2 * delta, phase + 3 * delta};
                mipp::Reg<float> lVec { l };
                mipp::Reg<float> rVec { r };

                auto s = table->processLinear (note, phaseDistortion (math::min (almostOne, phaseVec), params.bend, params.formant));
                postProcess (params, s);

                lVec += s * params.leftGain;
                rVec += s * params.rightGain;

                lVec.store (l); l += 4;
                rVec.store (r); r += 4;

                phase += delta * 4;
            }
           #endif

            for (; todo > 0; todo--)
            {
                auto s = table->processLinear (note, phaseDistortion (std::min (almostOne, phase), params.bend, params.formant));
                postProcess (params, s);

                *l++ += s * params.leftGain;
                *r++ += s * params.rightGain;

                phase += delta;
            }

            while (phase >= 1.0f)
            {
                phase -= 1.0f;
                lastTableIndex = tableIndex;
                tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
                table = bllt->getUnchecked (tableIndex);
            }
        }
    }

    void processAddingCrossfadeComplex (float note, const Params& params, juce::AudioSampleBuffer& buffer)
    {
        if (bllt == nullptr || bllt->size() == 0) return;

        if (tableIndex == -1 || tableIndex >= bllt->size())
            tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

        if (lastTableIndex == -1 || lastTableIndex >= bllt->size())
            lastTableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));

        float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
        float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

        int samps = buffer.getNumSamples();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        auto table1 = bllt->getUnchecked (tableIndex);
        auto table2 = bllt->getUnchecked (lastTableIndex);

        while (samps > 0)
        {
            auto todo = std::min (samps, int ((1.0f - phase) / delta) + 1);
            samps -= todo;

           #if GIN_HAS_SIMD
            for (; todo >= 4; todo -= 4)
            {
                mipp::Reg<float> phaseVec = {phase, phase + delta, phase + 2 * delta, phase + 3 * delta};
                mipp::Reg<float> lVec { l };
                mipp::Reg<float> rVec { r };

                auto s1 = table1->processLinear (note, phaseDistortion (math::min (almostOne, phaseVec), params.bend, params.formant));
                auto s2 = table2->processLinear (note, phaseDistortion (math::min (almostOne, phaseVec), params.bend, params.formant));

                auto s = s1 * phase + s2 * (1.0f - phase);
                postProcess (params, s);

                lVec += s * params.leftGain;
                rVec += s * params.rightGain;

                lVec.store (l); l += 4;
                rVec.store (r); r += 4;

                phase += delta * 4;
            }
           #endif

            for (; todo > 0; todo--)
            {
                auto s1 = table1->processLinear (note, phaseDistortion (std::min (almostOne, phase), params.bend, params.formant));
                auto s2 = table2->processLinear (note, phaseDistortion (std::min (almostOne, phase), params.bend, params.formant));

                auto s = s1 * phase + s2 * (1.0f - phase);
                postProcess (params, s);

                *l++ += s * params.leftGain;
                *r++ += s * params.rightGain;

                phase += delta;
            }

            while (phase >= 1.0f)
            {
                phase -= 1.0f;
                lastTableIndex = tableIndex;
                tableIndex = std::min (bllt->size() - 1, int (float (bllt->size()) * params.position));
                table1 = table2 = bllt->getUnchecked (tableIndex);
            }
        }
    }

    template<typename T>
    void postProcess (const Params& params, T& v)
    {
        if (params.asym > 0)
            v = math::lerp (v, math::pow4 (v - 1.0f) * -1.0f + 1.0f, math::pow2 (params.asym));

        if (params.fold > 0)
        {
            const auto fold = math::pow2 (math::pow2 (1.0f - params.fold)) * 1.5f;
            v = (v - ((math::max (v, fold) - fold) * T(2.0f)) - ((math::min (v, -fold) + fold) * T(2.0f)));
        }
    }

    void setWavetable (juce::OwnedArray<BandLimitedLookupTable>* table);

    void setBlockDC (bool b) { blockDC = b; }

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

        auto res = math::min (T (almostOne), addMix + subMix - phaseIn);
        jassert (math::minVal (res) >= 0.0f && math::maxVal (res) < 1.0f);
        return res;
    }

    template<typename T>
    T formantDistortion (T phaseIn, float formant)
    {
        auto res = math::min (T (almostOne), phaseIn * std::exp (formant * 1.60943791243f));
        jassert (math::minVal (res) >= 0.0f && math::maxVal (res) < 1.0f);
        return res;
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
    int lastTableIndex = -1;
    bool blockDC = true;

    DCBlocker blockerL;
    DCBlocker blockerR;

    static constexpr float almostOne = { 1.0f - std::numeric_limits<float>::epsilon() };
};

/**
    Extended parameters for wavetable voiced stereo oscillator.

    Extends VoicedOscillatorParams with wavetable-specific parameters including
    table position morphing, phase distortion, formant shifting, and waveshaping.

    Additional Parameters:
    - position: Wavetable position (0.0 to 1.0) for morphing between tables
    - bend: Phase distortion amount (0.0 to 1.0)
    - formant: Formant shift - pitch-independent spectral shifting
    - asym: Asymmetry/waveshaping amount (0.0 to 1.0)
    - fold: Wavefolding amount (0.0 to 1.0)

    @see VoicedOscillatorParams, WTVoicedStereoOscillator
*/
struct WTVoicedStereoOscillatorParams : public VoicedOscillatorParams
{
    float position  = 0.5;
    float bend      = 0.0f;
    float formant   = 0.0f;
    float asym      = 0.0f;
    float fold      = 0.0f;

    inline void init (WTOscillator::Params& p) const
    {
        p.position = position;
        p.bend     = bend;
        p.formant  = formant;
        p.asym     = asym;
        p.fold     = fold;
    }
};

//==============================================================================
/**
    Stereo wavetable oscillator with multiple voices for unison/detuning effects.

    WTVoicedStereoOscillator provides a complete wavetable-based virtual analog
    oscillator with support for multiple voices (unison), stereo spread, detuning,
    and wavetable-specific features like morphing, formant shifting, and phase
    distortion.

    Key Features:
    - Multiple voices for thick unison sounds
    - Per-voice detuning with spread
    - Stereo panning and spread
    - Wavetable position morphing
    - Formant shifting (pitch-independent)
    - Phase distortion (bend)
    - Wavefolding and asymmetry
    - Band-limited to avoid aliasing

    Usage:
    @code
    WTVoicedStereoOscillator osc(8); // Up to 8 voices
    osc.setSampleRate(44100.0);
    osc.setWavetable(&myWavetables);
    osc.noteOn(); // Initialize all voices

    WTVoicedStereoOscillatorParams params;
    params.voices = 4;
    params.detune = 0.1f;
    params.spread = 0.5f;
    params.position = 0.3f;
    params.fold = 0.2f;

    osc.process(60.0f, params, audioBuffer); // MIDI note 60 (C4)
    @endcode

    @see WTOscillator, VoicedStereoOscillator, WTVoicedStereoOscillatorParams
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

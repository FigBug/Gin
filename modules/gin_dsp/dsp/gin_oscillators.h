/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Virtual Analog Stereo oscillator.
*/
class StereoOscillator
{
public:
    StereoOscillator (BandLimitedLookupTables& bllt_) : bllt (bllt_) {}

    struct Params
    {
        Wave wave = Wave::sawUp;
        float leftGain = 1.0;
        float rightGain = 1.0;
        float pw = 0.5;
        float fold = 0.0f;
        float asym = 0.0f;
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
        float freq = float (std::min (sampleRate / 2.0, 440.0 * std::pow (2.0, (note - 69.0) / 12.0)));
        float delta = 1.0f / (float ((1.0f / freq) * sampleRate));

        int samps = buffer.getNumSamples();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        for (int i = 0; i < samps; i++)
        {
            auto s = bllt.process (params.wave, note, phase, params.pw);
            postProcess (params, s);

            *l++ += s * params.leftGain;
            *r++ += s * params.rightGain;

            phase += delta;
            while (phase >= 1.0f)
                phase -= 1.0f;
        }
    }

private:
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

    BandLimitedLookupTables& bllt;
    double sampleRate = 44100.0;
    float phase = 0.0f;
};

/**
    Parameters for VoicedStereoOscillator configuration.

    Defines the multi-voice settings for a voiced stereo oscillator including
    the number of voices, stereo panning, voice spread, detuning amount, and
    overall gain.

    @see VoicedStereoOscillator
*/
struct VoicedOscillatorParams
{
    int voices = 1;         ///< Number of voices (1-N for unison)
    float pan = 0.0f;       ///< Stereo pan position (-1.0 = left, 0.0 = center, 1.0 = right)
    float spread = 0.0f;    ///< Stereo spread amount (0.0 = mono, 1.0 = full stereo)
    float detune = 0.0f;    ///< Detune amount between voices (in semitones or cents)
    float gain = 1.0f;      ///< Overall gain/volume (0.0 to 1.0+)
};

//==============================================================================
/** Stereo Oscillator with multiples voices, pan, spread, detune, etc
*/
template<typename O, typename P>
class VoicedStereoOscillator
{
public:
    VoicedStereoOscillator() = default;

    void setSampleRate (double sr)
    {
        for (auto o : oscillators)
            o->setSampleRate (sr);
    }

    void noteOn (float phase = -1)
    {
        for (auto o : oscillators)
            o->noteOn (phase);
    }

    void noteOn (float phases[])
    {
        for (auto idx = 0; auto o : oscillators)
            o->noteOn (phases[idx++]);
    }

    void process (float note, const P& params, juce::AudioSampleBuffer& buffer)
    {
        buffer.clear();
        processAdding (note, params, buffer);
    }

    void processAdding (float note, const P& params, juce::AudioSampleBuffer& buffer)
    {
        typename O::Params p;
        params.init (p);

        if (params.voices == 1)
        {
            p.leftGain  = params.gain * (1.0f - params.pan);
            p.rightGain = params.gain * (1.0f + params.pan);

            oscillators[0]->processAdding (note, p, buffer);
        }
        else
        {
            float baseNote  = note - params.detune / 2;
            float noteDelta = params.detune / (params.voices - 1);

            float basePan = params.pan - params.spread;
            float panDelta = (params.spread * 2) / (params.voices - 1);

            for (int i = 0; i < params.voices; i++)
            {
                float pan = juce::jlimit (-1.0f, 1.0f, basePan + panDelta * i);

                p.leftGain  = params.gain * (1.0f - pan) / float (std::sqrt (params.voices));
                p.rightGain = params.gain * (1.0f + pan) / float (std::sqrt (params.voices));

                oscillators[i]->processAdding (baseNote + noteDelta * i, p, buffer);
            }
        }
    }

protected:
    juce::OwnedArray<O> oscillators;
};

/**
    Extended parameters for band-limited voiced stereo oscillator.

    Extends VoicedOscillatorParams with waveform-specific parameters for
    band-limited oscillators including waveform type, pulse width, and
    waveshaping effects.

    Additional Parameters:
    - wave: Waveform type (saw, square, triangle, etc.)
    - pw: Pulse width for square/pulse waves (0.0 to 1.0)
    - fold: Wavefolding amount (0.0 to 1.0)
    - asym: Asymmetry/waveshaping amount (0.0 to 1.0)

    @see VoicedOscillatorParams, BLLTVoicedStereoOscillator
*/
struct VoicedStereoOscillatorParams : public VoicedOscillatorParams
{
    Wave wave   = Wave::sawUp;
    float pw    = 0.5;
    float fold  = 0.0f;
    float asym  = 0.0f;

    inline void init (StereoOscillator::Params& p) const
    {
        p.wave  = wave;
        p.pw    = pw;
        p.asym  = asym;
        p.fold  = fold;
    }
};

//==============================================================================
/** Stereo Oscillator with multiple voices, pan, spread, detune, etc
 */
class BLLTVoicedStereoOscillator : public VoicedStereoOscillator<StereoOscillator, VoicedStereoOscillatorParams>
{
public:
    BLLTVoicedStereoOscillator (BandLimitedLookupTables& bllt, int maxVoices = 8)
    {
        for (int i = 0; i < maxVoices; i++)
            oscillators.add (new StereoOscillator (bllt));
    }
};

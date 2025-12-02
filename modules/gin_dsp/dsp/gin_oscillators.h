/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Virtual analog stereo oscillator with band-limited waveforms.

    StereoOscillator generates classic analog waveforms (saw, square, triangle, etc.)
    using band-limited lookup tables to avoid aliasing. It includes post-processing
    effects like wavefolding and asymmetry for additional timbral variation.

    Key Features:
    - Band-limited waveforms (no aliasing)
    - Multiple classic waveforms (saw, square, triangle, sine, etc.)
    - Pulse width modulation for square/pulse waves
    - Wavefolding for harmonic distortion
    - Asymmetry/waveshaping for timbral variation
    - Independent left/right gain control

    Parameters (via Params struct):
    - wave: Waveform type (saw, square, triangle, etc.)
    - leftGain/rightGain: Independent stereo output gains
    - pw: Pulse width (0.0 to 1.0) for square/pulse waves
    - fold: Wavefolding amount (0.0 to 1.0)
    - asym: Asymmetry/waveshaping amount (0.0 to 1.0)

    Usage:
    @code
    BandLimitedLookupTables bllt;
    StereoOscillator osc(bllt);
    osc.setSampleRate(44100.0);
    osc.noteOn(); // Initialize phase

    StereoOscillator::Params params;
    params.wave = Wave::sawUp;
    params.pw = 0.5f;
    params.fold = 0.3f;

    osc.process(60.0f, params, audioBuffer); // MIDI note 60 (C4)
    @endcode

    @see BandLimitedLookupTables, VoicedStereoOscillator, Wave
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

    void setSampleRate (double sr)  
    {
        sampleRate = sr;
        
        if (sampleRate > 0)
        {
            leftGain.reset(sampleRate, 0.05);
            rightGain.reset(sampleRate, 0.05);
        }
    }
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
            leftGain.setTargetValue(params.leftGain);
            rightGain.setTargetValue(params.rightGain);
            
            auto s = bllt.process (params.wave, note, phase, params.pw);
            postProcess (params, s);

            *l++ += s * leftGain.getNextValue();
            *r++ += s * rightGain.getNextValue();

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
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> leftGain, rightGain;
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
/**
    Stereo oscillator with multiple voices for unison/detuning effects.

    VoicedStereoOscillator is a template class that wraps any oscillator type (O)
    and adds multi-voice capabilities with stereo spread, detuning, and panning.
    This creates rich, thick unison sounds commonly used in synthesizers.

    Key Features:
    - Multiple voices (1 to N) for unison effects
    - Per-voice detuning with adjustable spread
    - Stereo panning and spread controls
    - Automatic gain compensation for multiple voices
    - Works with any oscillator type (template-based)

    The voices are evenly distributed across:
    - Pitch range: controlled by detune parameter
    - Stereo field: controlled by spread parameter
    - Base position: controlled by pan parameter

    Template Parameters:
    - O: Oscillator type (e.g., StereoOscillator, WTOscillator)
    - P: Parameter struct type that extends VoicedOscillatorParams

    Usage:
    @code
    // Typically used via derived classes like BLLTVoicedStereoOscillator
    // or WTVoicedStereoOscillator, but can be used directly:

    VoicedStereoOscillator<StereoOscillator, VoicedStereoOscillatorParams> osc;
    osc.setSampleRate(44100.0);
    osc.noteOn(); // Initialize all voices

    VoicedStereoOscillatorParams params;
    params.voices = 4;      // 4 voices for thick sound
    params.detune = 0.15f;  // Detune range in semitones
    params.spread = 0.7f;   // Stereo spread amount
    params.pan = 0.0f;      // Center pan
    params.gain = 1.0f;

    osc.process(60.0f, params, audioBuffer);
    @endcode

    @see VoicedOscillatorParams, BLLTVoicedStereoOscillator, WTVoicedStereoOscillator
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
/**
    Band-limited stereo oscillator with multiple voices for unison effects.

    BLLTVoicedStereoOscillator combines the band-limited StereoOscillator with
    the multi-voice capabilities of VoicedStereoOscillator. It's ideal for creating
    rich, detuned analog-style sounds without aliasing.

    Key Features:
    - Multiple voices (configurable up to maxVoices)
    - Band-limited waveforms (no aliasing)
    - Classic analog waveforms (saw, square, triangle, etc.)
    - Per-voice detuning and stereo spread
    - Pulse width modulation
    - Wavefolding and asymmetry effects
    - Stereo panning

    Usage:
    @code
    BandLimitedLookupTables bllt;
    BLLTVoicedStereoOscillator osc(bllt, 8); // Up to 8 voices
    osc.setSampleRate(44100.0);
    osc.noteOn();

    VoicedStereoOscillatorParams params;
    params.wave = Wave::sawUp;
    params.voices = 5;
    params.detune = 0.12f; // Semitones
    params.spread = 0.6f;  // Stereo spread
    params.pw = 0.5f;
    params.fold = 0.2f;

    osc.process(60.0f, params, audioBuffer);
    @endcode

    @see VoicedStereoOscillator, StereoOscillator, VoicedStereoOscillatorParams
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

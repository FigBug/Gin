/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/** Stereo oscillator. L & R can be at different pitches
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
            *l++ += s * params.leftGain;
            *r++ += s * params.rightGain;

            phase += delta;
            while (phase >= 1.0f)
                phase -= 1.0f;
        }
    }

private:
    BandLimitedLookupTables& bllt;
    double sampleRate = 44100.0;
    float phase = 0.0f;
};

struct VoicedOscillatorParams
{
    int voices = 1;
    float pan = 0.0f;
    float spread = 0.0f;
    float detune = 0.0f;
    float gain = 1.0f;
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

struct VoicedStereoOscillatorParams : public VoicedOscillatorParams
{
    Wave wave = Wave::sawUp;
    float pw = 0.5;

    inline void init (StereoOscillator::Params& p) const
    {
        p.wave = wave;
        p.pw = pw;
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

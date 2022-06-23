#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class OscillatorBox : public gin::ParamBox
{
public:
    OscillatorBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        auto& osc = proc.oscParams;

        addControl (new gin::Select (osc.wave), 0, 0);
        addControl (new gin::Select (osc.voices), 2, 0);
        addControl (detune = new gin::Knob (osc.detune), 3, 0);

        addControl (spread = new gin::Knob (osc.spread), 2, 1);
        addControl (trans = new gin::Knob (osc.voicesTrns, true), 3, 1);

        watchParam (osc.wave);
        watchParam (osc.voices);
    }

    void paramChanged() override
    {
        gin::ParamBox::paramChanged();

        auto& osc = proc.oscParams;

        trans->setEnabled (osc.voices->getProcValue() > 1);
        detune->setEnabled (osc.voices->getProcValue() > 1);
        spread->setEnabled (osc.voices->getProcValue() > 1);
    }

    SynthAudioProcessor& proc;
    int idx = 0;
    gin::ParamComponent::Ptr trans, detune, spread;
};

//==============================================================================
class FilterBox : public gin::ParamBox
{
public:
    FilterBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        auto& flt = proc.filterParams;

        auto freq = new gin::Knob (flt.frequency);
        addControl (freq, 0, 0);
        addControl (new gin::Knob (flt.resonance), 1, 0);
        addControl (new gin::Knob (flt.amount, true), 2, 0);

        addControl (new gin::Knob (flt.keyTracking), 0, 1);
        addControl (new gin::Select (flt.type), 1, 1);
        addControl (v = new gin::Knob (flt.velocityTracking), 2, 1);

        freq->setLiveValuesCallback ([this] ()
        {
            if (proc.filterParams.amount->getUserValue() != 0.0f || proc.filterParams.keyTracking->getUserValue() != 0.0f)
                return proc.getLiveFilterCutoff();
            return juce::Array<float>();
        });
    }

    void paramChanged () override
    {
        gin::ParamBox::paramChanged ();

        auto& flt = proc.filterParams;
        v->setEnabled (flt.amount->getUserValue() != 0.0f);
    }

    SynthAudioProcessor& proc;
    gin::ParamComponent::Ptr v;
};

//==============================================================================
class FilterADSRArea : public gin::ParamArea
{
public:
    FilterADSRArea (SynthAudioProcessor& proc_)
        : proc (proc_)
    {
        auto& flt = proc.filterParams;

        adsr = new gin::ADSRComponent ();
        adsr->setParams (flt.attack, flt.decay, flt.sustain, flt.release);
        addControl (adsr);

        addControl (a = new gin::Knob (flt.attack));
        addControl (d = new gin::Knob (flt.decay));
        addControl (s = new gin::Knob (flt.sustain));
        addControl (r = new gin::Knob (flt.release));

        watchParam (flt.amount);
    }

    void paramChanged() override
    {
        gin::ParamArea::paramChanged ();

        auto& flt = proc.filterParams;
        a->setEnabled (flt.amount->getUserValue() != 0.0f);
        d->setEnabled (flt.amount->getUserValue() != 0.0f);
        s->setEnabled (flt.amount->getUserValue() != 0.0f);
        r->setEnabled (flt.amount->getUserValue() != 0.0f);
        adsr->setEnabled (flt.amount->getUserValue() != 0.0f);
    }

    SynthAudioProcessor& proc;
    int idx;
    gin::ParamComponent::Ptr a, d, s, r;
    gin::ADSRComponent* adsr;
};

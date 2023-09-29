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
        setName ( "osc" );

        auto& osc = proc.oscParams;

        addEnable (osc.enable);

        addControl (new gin::Select (osc.wave), 1, 1);
        addControl (new gin::Knob (osc.tune, true), 1, 0);
        addControl (new gin::Knob (osc.finetune, true), 2, 0);
        addControl (new gin::Knob (osc.level), 3, 0);
        addControl (new gin::Knob (osc.pan, true), 4, 0);

        addControl (new gin::Select (osc.voices), 0, 1);
        addControl (detune = new gin::Knob (osc.detune), 1, 1);
        addControl (spread = new gin::Knob (osc.spread), 2, 1);

        watchParam (osc.voices);
    }

    ~OscillatorBox() override
    {
    }

    void paramChanged() override
    {
        gin::ParamBox::paramChanged();

        auto& osc = proc.oscParams;

        detune->setEnabled (osc.voices->getProcValue() > 1);
        spread->setEnabled (osc.voices->getProcValue() > 1);
    }

    SynthAudioProcessor& proc;
    gin::ParamComponent::Ptr detune, spread;
};

//==============================================================================
class ADSRBox : public gin::ParamBox
{
public:
    ADSRBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ("adsr");

        auto& preset = proc.adsrParams;

        adsr = new gin::ADSRComponent ();
        adsr->setParams (preset.attack, preset.decay, preset.sustain, preset.release);
        addControl (adsr, 0, 0, 4, 1);

        addControl (new gin::Knob (preset.attack), 0, 1);
        addControl (new gin::Knob (preset.decay), 1, 1);
        addControl (new gin::Knob (preset.sustain), 2, 1);
        addControl (new gin::Knob (preset.release), 3, 1);
        addControl (new gin::Knob (preset.velocityTracking), 4, 1);
    }

    SynthAudioProcessor& proc;
    gin::ParamComponent::Ptr a, d, s, r;
    gin::ADSRComponent* adsr;
};

//==============================================================================
class FilterBox : public gin::ParamBox
{
public:
    FilterBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ( "flt" );

        auto& flt = proc.filterParams;

        addEnable (flt.enable);

        auto freq = new gin::Knob (flt.frequency);
        addControl (freq, 0, 0);
        addControl (new gin::Knob (flt.resonance), 1, 0);

        addControl (new gin::Knob (flt.keyTracking), 0, 1);
        addControl (new gin::Select (flt.type), 1, 1);

        freq->setLiveValuesCallback ([this] ()
        {
            if (proc.filterParams.keyTracking->getUserValue() != 0.0f ||
                proc.modMatrix.isModulated (gin::ModDstId (proc.filterParams.frequency->getModIndex())))
                return proc.getLiveFilterCutoff();
            return juce::Array<float>();
        });
    }

    SynthAudioProcessor& proc;
};

//==============================================================================
class LFOBox : public gin::ParamBox
{
public:
    LFOBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ("lfo");

        auto& lfo = proc.lfoParams;

        addEnable (lfo.enable);

        addModSource (new gin::ModulationSourceButton (proc.modMatrix, proc.modSrcLFO, true));
        addModSource (new gin::ModulationSourceButton (proc.modMatrix, proc.modSrcMonoLFO, false));

        addControl (r = new gin::Knob (lfo.rate), 0, 0);
        addControl (b = new gin::Select (lfo.beat), 0, 0);
        addControl (new gin::Knob (lfo.depth, true), 1, 0);
        addControl (new gin::Knob (lfo.fade, true), 0, 1);
        addControl (new gin::Knob (lfo.delay), 1, 1);

        addControl (new gin::Select (lfo.wave), 2, 1);
        addControl (new gin::Switch (lfo.sync), 3, 1);
        addControl (new gin::Knob (lfo.phase, true), 4, 1);
        addControl (new gin::Knob (lfo.offset, true), 5, 1);

        auto l = new gin::LFOComponent();
        l->phaseCallback = [this]
        {
            std::vector<float> res;
            res.push_back (proc.modLFO.getCurrentPhase());
            return res;
        };
        l->setParams (lfo.wave, lfo.sync, lfo.rate, lfo.beat, lfo.depth, lfo.offset, lfo.phase, lfo.enable);
        addControl (l, 2, 0, 4, 1);

        watchParam (lfo.sync);

        setSize (112, 163);
    }

    void paramChanged () override
    {
        gin::ParamBox::paramChanged ();

        if (r && b)
        {
            auto& lfo = proc.lfoParams;
            r->setVisible (! lfo.sync->isOn());
            b->setVisible (lfo.sync->isOn());
        }
    }

    SynthAudioProcessor& proc;
    int idx;
    gin::ParamComponent::Ptr r = nullptr;
    gin::ParamComponent::Ptr b = nullptr;
};

//==============================================================================
class ModBox : public gin::ParamBox
{
public:
    ModBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ("mod");
        
        addControl (new gin::ModSrcListBox (proc.modMatrix), 0, 0, 3, 2);
    }

    SynthAudioProcessor& proc;
};

//==============================================================================
class MatrixBox : public gin::ParamBox
{
public:
    MatrixBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ("mtx");

        addControl (new gin::ModMatrixBox (proc, proc.modMatrix), 0, 0, 3, 2);
    }

    SynthAudioProcessor& proc;
};

//==============================================================================
class GlobalBox : public gin::ParamBox
{
public:
    GlobalBox (const juce::String& name, SynthAudioProcessor& proc_)
        : gin::ParamBox (name), proc (proc_)
    {
        setName ("global");

        addControl (new gin::Knob (proc.globalParams.level), 0, 0);
        addControl (new gin::Select (proc.globalParams.glideMode), 1, 0);
        addControl (new gin::Knob (proc.globalParams.glideRate), 2, 0);

        addControl (new gin::Knob (proc.globalParams.voices), 0, 1);
        addControl (new gin::Switch (proc.globalParams.legato), 1, 1);
        addControl (new gin::Switch (proc.globalParams.mono), 2, 1);

        setSize (168, 163);
    }

    SynthAudioProcessor& proc;
};

//==============================================================================
class DelayBox : public gin::ParamBox
{
public:
    DelayBox (SynthAudioProcessor& proc_)
        : gin::ParamBox ("Delay"), proc (proc_)
    {
        setName ("delay");

        addEnable (proc.delayParams.enable);

        addControl (t = new gin::Knob (proc.delayParams.time), 0, 0);
        addControl (b = new gin::Select (proc.delayParams.beat), 0, 0);
        addControl (new gin::Knob (proc.delayParams.fb), 1, 0);
        addControl (new gin::Knob (proc.delayParams.cf), 2, 0);

        addControl (new gin::Switch (proc.delayParams.sync), 0, 1);
        addControl (new gin::Knob (proc.delayParams.mix), 1.5f, 1.0f);

        t->setName ("Delay1");
        b->setName ("Delay2");

        watchParam (proc.delayParams.sync);

        setSize (168, 163);
    }

    void paramChanged () override
    {
        gin::ParamBox::paramChanged();

        t->setVisible (! proc.delayParams.sync->isOn());
        b->setVisible (proc.delayParams.sync->isOn());
    }

    SynthAudioProcessor& proc;
    gin::ParamComponent::Ptr t, b;
};


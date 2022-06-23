#include "SynthVoice.h"
#include "PluginProcessor.h"

//==============================================================================
SynthVoice::SynthVoice (SynthAudioProcessor& p, gin::BandLimitedLookupTables& bllt)
    : proc (p)
    , bandLimitedLookupTables (bllt)
{
    filters.setNumChannels (2);
}

void SynthVoice::noteStarted()
{
    fastKill = false;

    auto note = getCurrentlyPlayingNote();
    if (glideInfo.fromNote != -1 && (glideInfo.glissando || glideInfo.portamento))
    {
        noteSmoother.setTime (glideInfo.rate);
        noteSmoother.setValueUnsmoothed (glideInfo.fromNote / 127.0f);
        noteSmoother.setValue (note.initialNote / 127.0f);
    }
    else
    {
        noteSmoother.setValueUnsmoothed (note.initialNote / 127.0f);
    }

    filters.reset();
    filterADSRs.reset();

    updateParams (0);
    updateParams (0);

    oscillators.noteOn();
    filterADSRs.noteOn();

    adsr.reset();
    adsr.noteOn();
}

void SynthVoice::noteRetriggered()
{
    auto note = getCurrentlyPlayingNote();

    if (glideInfo.fromNote != -1 && (glideInfo.glissando || glideInfo.portamento))
    {
        noteSmoother.setTime (glideInfo.rate);
        noteSmoother.setValue (note.initialNote / 127.0f);
    }
    else
    {
        noteSmoother.setValueUnsmoothed (note.initialNote / 127.0f);
    }

    updateParams (0);

    oscillators.noteOn();
    filterADSRs.noteOn();
    adsr.noteOn();
}

void SynthVoice::noteStopped (bool allowTailOff)
{
    adsr.noteOff();
    filterADSRs.noteOff();

    if (! allowTailOff)
        clearCurrentNote();
}

void SynthVoice::notePressureChanged()
{
}

void SynthVoice::noteTimbreChanged()
{
}

void SynthVoice::setCurrentSampleRate (double newRate)
{
    MPESynthesiserVoice::setCurrentSampleRate (newRate);

    oscillators.setSampleRate (newRate);
    filters.setSampleRate (newRate);
    filterADSRs.setSampleRate (newRate);

    noteSmoother.setSampleRate (newRate);
    adsr.setSampleRate (newRate);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    updateParams (numSamples);

    // Run OSC
    gin::ScratchBuffer buffer (2, numSamples);

    oscillators.processAdding (currentMidiNotes, oscParams, buffer);

    // Apply velocity
    float velocity = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    buffer.applyGain (gin::velocityToGain (velocity, ampKeyTrack));

    // Apply filters
    filters.process (buffer);

    // Run ADSR
    adsr.processMultiplying (buffer);

    if (adsr.getState() == gin::AnalogADSR::State::idle)
    {
        clearCurrentNote();
    }

    // Copy output to synth
    outputBuffer.addFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.addFrom (1, startSample, buffer, 1, 0, numSamples);
}

void SynthVoice::updateParams (int blockSize)
{
    auto note = getCurrentlyPlayingNote();

    {
        currentMidiNotes = noteSmoother.getCurrentValue() * 127.0f;
        if (glideInfo.glissando) currentMidiNotes = (float) juce::roundToInt (currentMidiNotes);
        currentMidiNotes += float (note.totalPitchbendInSemitones);

        oscParams.wave   = (gin::Wave) int (proc.oscParams.wave->getProcValue());
        oscParams.voices = int (proc.oscParams.voices->getProcValue());
        oscParams.vcTrns = int (proc.oscParams.voicesTrns->getProcValue());
        oscParams.pw     = 0.5f;
        oscParams.pan    = 0.5f;
        oscParams.spread = getValue (proc.oscParams.spread) / 100.0f;
        oscParams.detune = getValue (proc.oscParams.detune);
        oscParams.gain   = 1.0f;
    }

    ampKeyTrack = getValue (proc.adsrParams.velocityTracking);

    {
        filterADSRs.setAttack (getValue (proc.filterParams.attack));
        filterADSRs.setSustainLevel (getValue (proc.filterParams.sustain));
        filterADSRs.setDecay (getValue (proc.filterParams.decay));
        filterADSRs.setRelease (getValue (proc.filterParams.release));

        filterADSRs.process (blockSize);

        float filterWidth = float (gin::getMidiNoteFromHertz (20000.0));
        float filterEnv   = filterADSRs.getOutput();
        float filterSens = getValue (proc.filterParams.velocityTracking);
        filterSens = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat() * filterSens + 1.0f - filterSens;

        float n = getValue (proc.filterParams.frequency);
        n += (currentlyPlayingNote.initialNote - 60) * getValue (proc.filterParams.keyTracking);
        n += filterEnv * filterSens * getValue (proc.filterParams.amount) * filterWidth;

        float f = gin::getMidiNoteInHertz (n);
        float maxFreq = std::min (20000.0f, float (getSampleRate() / 2));
        f = juce::jlimit (4.0f, maxFreq, f);

        float q = gin::Q / (1.0f - (getValue (proc.filterParams.resonance) / 100.0f) * 0.99f);

        switch (int (proc.filterParams.type->getProcValue()))
        {
            case 0:
                filters.setType (gin::Filter::lowpass);
                filters.setSlope (gin::Filter::db12);
                break;
            case 1:
                filters.setType (gin::Filter::lowpass);
                filters.setSlope (gin::Filter::db24);
                break;
            case 2:
                filters.setType (gin::Filter::highpass);
                filters.setSlope (gin::Filter::db12);
                break;
            case 3:
                filters.setType (gin::Filter::highpass);
                filters.setSlope (gin::Filter::db24);
                break;
            case 4:
                filters.setType (gin::Filter::bandpass);
                filters.setSlope (gin::Filter::db12);
                break;
            case 5:
                filters.setType (gin::Filter::bandpass);
                filters.setSlope (gin::Filter::db24);
                break;
            case 6:
                filters.setType (gin::Filter::notch);
                filters.setSlope (gin::Filter::db12);
                break;
            case 7:
                filters.setType (gin::Filter::notch);
                filters.setSlope (gin::Filter::db24);
                break;
        }

        filters.setParams (f, q);
    }

    adsr.setAttack (getValue (proc.adsrParams.attack));
    adsr.setDecay (getValue (proc.adsrParams.decay));
    adsr.setSustainLevel (getValue (proc.adsrParams.sustain));
    adsr.setRelease (fastKill ? 0.01f : getValue (proc.adsrParams.release));

    noteSmoother.process (blockSize);
}

float SynthVoice::getFilterCutoffNormalized()
{
    float freq = filters.getFrequency();
    return proc.filterParams.frequency->getUserRange().convertTo0to1 (gin::getMidiNoteFromHertz (freq));
}

float SynthVoice::getValue (gin::Parameter* p)
{
    float base = p->getValue();

    base = juce::jlimit (0.0f, 1.0f, base);
    auto v = p->getUserRange().convertFrom0to1 (base);

    if (p->conversionFunction)
        v = p->conversionFunction (v);

    return v;
}

#include "SynthVoice.h"
#include "PluginProcessor.h"

//==============================================================================
SynthVoice::SynthVoice (SynthAudioProcessor& p)
    : proc (p)
    , oscillator (proc.analogTables, 8)
{
    filter.setNumChannels (2);
}

void SynthVoice::noteStarted()
{
    fastKill = false;
    startVoice();

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

    proc.modMatrix.setPolyValue (*this, proc.modSrcVelocity, note.noteOnVelocity.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());

    juce::ScopedValueSetter<bool> svs (disableSmoothing, true);

    filter.reset();

    modLFO.reset();

    updateParams (0);
    snapParams();
    updateParams (0);
    snapParams();
    
    oscillator.noteOn();

    modLFO.noteOn();

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
    
    proc.modMatrix.setPolyValue (*this, proc.modSrcVelocity, note.noteOnVelocity.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());
    
    updateParams (0);

    oscillator.noteOn();
    adsr.noteOn();
}

void SynthVoice::noteStopped (bool allowTailOff)
{
    adsr.noteOff();

    if (! allowTailOff)
    {
        clearCurrentNote();
        stopVoice();
    }
}

void SynthVoice::notePressureChanged()
{
    auto note = getCurrentlyPlayingNote();
    proc.modMatrix.setPolyValue (*this, proc.modSrcPressure, note.pressure.asUnsignedFloat());
}

void SynthVoice::noteTimbreChanged()
{
    auto note = getCurrentlyPlayingNote();
    proc.modMatrix.setPolyValue (*this, proc.modSrcTimbre, note.initialTimbre.asUnsignedFloat());
}

void SynthVoice::setCurrentSampleRate (double newRate)
{
    MPESynthesiserVoice::setCurrentSampleRate (newRate);

    oscillator.setSampleRate (newRate);
    filter.setSampleRate (newRate);

    modLFO.setSampleRate (newRate);

    noteSmoother.setSampleRate (newRate);
    adsr.setSampleRate (newRate);
}

void SynthVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    updateParams (numSamples);

    // Run OSC
    gin::ScratchBuffer buffer (2, numSamples);

    if (proc.oscParams.enable->isOn())
        oscillator.processAdding (currentMidiNote, oscParams, buffer);

    // Apply velocity
    float velocity = currentlyPlayingNote.noteOnVelocity.asUnsignedFloat();
    buffer.applyGain (gin::velocityToGain (velocity, ampKeyTrack));

    // Apply filter
    if (proc.filterParams.enable->isOn())
        filter.process (buffer);
    
    // Run ADSR
    adsr.processMultiplying (buffer);
    
    if (adsr.getState() == gin::AnalogADSR::State::idle)
    {
        clearCurrentNote();
        stopVoice();
    }

    // Copy output to synth
    outputBuffer.addFrom (0, startSample, buffer, 0, 0, numSamples);
    outputBuffer.addFrom (1, startSample, buffer, 1, 0, numSamples);
    
    finishBlock (numSamples);
}

void SynthVoice::updateParams (int blockSize)
{
    auto note = getCurrentlyPlayingNote();
    
    proc.modMatrix.setPolyValue (*this, proc.modSrcNote, note.initialNote / 127.0f);

    if (proc.oscParams.enable->isOn())
    {
        currentMidiNote = noteSmoother.getCurrentValue() * 127.0f;
        if (glideInfo.glissando) currentMidiNote = (float) juce::roundToInt (currentMidiNote);
        currentMidiNote += float (note.totalPitchbendInSemitones);
        currentMidiNote += getValue (proc.oscParams.tune) + getValue (proc.oscParams.finetune) / 100.0f;

        switch (proc.oscParams.wave->getUserValueInt())
        {
            case 0: oscParams.wave = gin::Wave::sine;       break;
            case 1: oscParams.wave = gin::Wave::triangle;   break;
            case 2: oscParams.wave = gin::Wave::sawUp;      break;
            case 3: oscParams.wave = gin::Wave::pulse;      break;;
        }
        
        oscParams.voices     = int (proc.oscParams.voices->getProcValue());
        oscParams.pan        = getValue (proc.oscParams.pan);
        oscParams.spread     = getValue (proc.oscParams.spread) / 100.0f;
        oscParams.detune     = getValue (proc.oscParams.detune);
        oscParams.gain       = getValue (proc.oscParams.level);
    }
    
    ampKeyTrack = getValue (proc.adsrParams.velocityTracking);

    if (proc.filterParams.enable->isOn())
    {
        float n = getValue (proc.filterParams.frequency);
        n += (currentlyPlayingNote.initialNote - 60) * getValue (proc.filterParams.keyTracking);

        float f = gin::getMidiNoteInHertz (n);
        float maxFreq = std::min (20000.0f, float (getSampleRate() / 2));
        f = juce::jlimit (4.0f, maxFreq, f);

        float q = gin::Q / (1.0f - (getValue (proc.filterParams.resonance) / 100.0f) * 0.99f);

        switch (int (proc.filterParams.type->getProcValue()))
        {
            case 0:
                filter.setType (gin::Filter::lowpass);
                filter.setSlope (gin::Filter::db12);
                break;
            case 1:
                filter.setType (gin::Filter::lowpass);
                filter.setSlope (gin::Filter::db24);
                break;
            case 2:
                filter.setType (gin::Filter::highpass);
                filter.setSlope (gin::Filter::db12);
                break;
            case 3:
                filter.setType (gin::Filter::highpass);
                filter.setSlope (gin::Filter::db24);
                break;
            case 4:
                filter.setType (gin::Filter::bandpass);
                filter.setSlope (gin::Filter::db12);
                break;
            case 5:
                filter.setType (gin::Filter::bandpass);
                filter.setSlope (gin::Filter::db24);
                break;
            case 6:
                filter.setType (gin::Filter::notch);
                filter.setSlope (gin::Filter::db12);
                break;
            case 7:
                filter.setType (gin::Filter::notch);
                filter.setSlope (gin::Filter::db24);
                break;
        }

        filter.setParams (f, q);
    }
    
    if (proc.lfoParams.enable->isOn())
    {
        gin::LFO::Parameters params;

        float freq = 0;
        if (proc.lfoParams.sync->getProcValue() > 0.0f)
            freq = 1.0f / gin::NoteDuration::getNoteDurations()[size_t (proc.lfoParams.beat->getProcValue())].toSeconds (proc.playhead);
        else
            freq = getValue (proc.lfoParams.rate);

        params.waveShape = (gin::LFO::WaveShape) int (proc.lfoParams.wave->getProcValue());
        params.frequency = freq;
        params.phase     = getValue (proc.lfoParams.phase);
        params.offset    = getValue (proc.lfoParams.offset);
        params.depth     = getValue (proc.lfoParams.depth);
        params.delay     = getValue (proc.lfoParams.delay);
        params.fade      = getValue (proc.lfoParams.fade);

        modLFO.setParameters (params);
        modLFO.process (blockSize);

        proc.modMatrix.setPolyValue (*this, proc.modSrcLFO, modLFO.getOutput());
    }
    else
    {
        proc.modMatrix.setPolyValue (*this, proc.modSrcLFO, 0);
    }

    adsr.setAttack (getValue (proc.adsrParams.attack));
    adsr.setDecay (getValue (proc.adsrParams.decay));
    adsr.setSustainLevel (getValue (proc.adsrParams.sustain));
    adsr.setRelease (fastKill ? 0.01f : getValue (proc.adsrParams.release));
    
    noteSmoother.process (blockSize);
}

bool SynthVoice::isVoiceActive()
{
    return isActive();
}

float SynthVoice::getFilterCutoffNormalized()
{
    float freq = filter.getFrequency();
    auto range = proc.filterParams.frequency->getUserRange();
    return range.convertTo0to1 (juce::jlimit (range.start, range.end, gin::getMidiNoteFromHertz (freq)));
}

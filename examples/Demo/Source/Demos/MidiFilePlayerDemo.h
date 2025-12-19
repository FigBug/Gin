/*
  ==============================================================================

    MidiFilePlayerDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
// Simple synth voice for MIDI playback - a stripped down version of the Synth example
class SimpleSynthVoice : public gin::SynthesiserVoice
{
public:
    SimpleSynthVoice (gin::BandLimitedLookupTables& tables)
        : oscillator (tables, 1)
    {
    }

    float getCurrentNote() override { return currentMidiNote; }

    void noteStarted() override
    {
        fastKill = false;
        auto note = getCurrentlyPlayingNote();
        currentMidiNote = note.initialNote;
        velocity = note.noteOnVelocity.asUnsignedFloat();

        oscillator.noteOn();
        adsr.reset();
        adsr.noteOn();
    }

    void noteRetriggered() override
    {
        auto note = getCurrentlyPlayingNote();
        currentMidiNote = note.initialNote;
        velocity = note.noteOnVelocity.asUnsignedFloat();

        oscillator.noteOn();
        adsr.noteOn();
    }

    void noteStopped (bool allowTailOff) override
    {
        adsr.noteOff();

        if (! allowTailOff)
            clearCurrentNote();
    }

    void notePressureChanged() override {}
    void noteTimbreChanged() override {}
    void notePitchbendChanged() override {}
    void noteKeyStateChanged() override {}

    void setCurrentSampleRate (double newRate) override
    {
        juce::MPESynthesiserVoice::setCurrentSampleRate (newRate);

        oscillator.setSampleRate (newRate);
        adsr.setSampleRate (newRate);
    }

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        gin::ScratchBuffer buffer (2, numSamples);

        // Configure oscillator
        gin::VoicedStereoOscillatorParams oscParams;
        oscParams.wave = gin::Wave::sawUp;
        oscParams.voices = 1;
        oscParams.gain = 0.3f;
        oscParams.pan = 0.0f;
        oscParams.spread = 0.0f;
        oscParams.detune = 0.0f;

        auto note = getCurrentlyPlayingNote();
        float noteToPlay = currentMidiNote + float (note.totalPitchbendInSemitones);

        oscillator.processAdding (noteToPlay, oscParams, buffer);

        // Apply velocity
        buffer.applyGain (velocity * 0.8f);

        // Configure and run ADSR
        adsr.setAttack (0.01f);
        adsr.setDecay (0.1f);
        adsr.setSustainLevel (0.7f);
        adsr.setRelease (fastKill ? 0.01f : 0.3f);

        adsr.processMultiplying (buffer);

        if (adsr.getState() == gin::AnalogADSR::State::idle)
            clearCurrentNote();

        // Copy to output
        outputBuffer.addFrom (0, startSample, buffer, 0, 0, numSamples);
        outputBuffer.addFrom (1, startSample, buffer, 1, 0, numSamples);
    }

    bool isVoiceActive()
    {
        return isActive();
    }

private:
    gin::BLLTVoicedStereoOscillator oscillator;
    gin::AnalogADSR adsr;
    float currentMidiNote = 60.0f;
    float velocity = 1.0f;
};

//==============================================================================
struct MidiFilePlayerDemo : public DemoComponent,
                            public juce::FileDragAndDropTarget,
                            public juce::Timer
{
    MidiFilePlayerDemo()
    {
        setName ("MIDI File Player");

        // Set up synth with voices
        for (int i = 0; i < 16; ++i)
            synth.addVoice (new SimpleSynthVoice (bllt));

        synth.setNumVoices (16);

        addAndMakeVisible (midiDisplay);
        midiDisplay.setColour (gin::MidiFileComponent::backgroundColourId, juce::Colours::black);
        midiDisplay.setColour (gin::MidiFileComponent::noteColourId, juce::Colours::cyan);
        midiDisplay.setColour (gin::MidiFileComponent::playheadColourId, juce::Colours::yellow);
        midiDisplay.setColour (gin::MidiFileComponent::outlineColourId, juce::Colours::grey);
        midiDisplay.setColour (gin::MidiFileComponent::dropHighlightColourId, juce::Colours::cyan.withAlpha (0.3f));

        midiDisplay.onFileDropped = [this]
        {
            updateButtons();
        };

        midiDisplay.onPlaybackStateChanged = [this]
        {
            updateButtons();
        };

        addAndMakeVisible (playButton);
        addAndMakeVisible (stopButton);

        playButton.onClick = [this]
        {
            midiPlayer.play();
            updateButtons();
        };

        stopButton.onClick = [this]
        {
            midiPlayer.stop();
            midiPlayer.setPlayheadPosition (0.0);
            updateButtons();
        };

        addAndMakeVisible (scope);
        scope.setColour (gin::TriggeredScope::backgroundColourId, juce::Colours::black);
        scope.setColour (gin::TriggeredScope::lineColourId, juce::Colours::grey);
        scope.setColour (gin::TriggeredScope::traceColourId, juce::Colours::cyan);
        scope.setColour (gin::TriggeredScope::envelopeColourId, juce::Colours::cyan.withAlpha (0.5f));

        addAndMakeVisible (spectrum);
        spectrum.setColour (gin::SpectrumAnalyzer::backgroundColourId, juce::Colours::black);
        spectrum.setColour (gin::SpectrumAnalyzer::lineColourId, juce::Colours::grey);
        spectrum.setColour (gin::SpectrumAnalyzer::traceColourId, juce::Colours::orange);
        spectrum.setColour (gin::SpectrumAnalyzer::envelopeColourId, juce::Colours::orange.withAlpha (0.5f));

        addAndMakeVisible (xyScope);
        xyScope.setColour (gin::XYScope::backgroundColourId, juce::Colours::black);
        xyScope.setColour (gin::XYScope::lineColourId, juce::Colours::grey);
        xyScope.setColour (gin::XYScope::traceColourId, juce::Colours::limegreen);

        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);
        xyFifo.setSize (2, 44100);

        midiPlayer.setLooping (true);

        startTimerHz (30);
        updateButtons();
    }

    ~MidiFilePlayerDemo() override
    {
        stopTimer();
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused (samplesPerBlock);
        midiPlayer.setSampleRate (sampleRate);
        synth.setCurrentPlaybackSampleRate (sampleRate);
        spectrum.setSampleRate (sampleRate);

        scopeFifo.setSize (2, int (sampleRate));
        spectrumFifo.setSize (2, int (sampleRate));
        xyFifo.setSize (2, int (sampleRate));
    }

    void processBlock (juce::AudioBuffer<float>& buffer) override
    {
        // Get MIDI events from the player
        juce::MidiBuffer midiBuffer;
        midiPlayer.processBlock (buffer.getNumSamples(), midiBuffer);

        // Render synth audio from MIDI
        synth.renderNextBlock (buffer, midiBuffer, 0, buffer.getNumSamples());

        // Feed visualizers
        if (scopeFifo.getFreeSpace() >= buffer.getNumSamples())
            scopeFifo.write (buffer);

        if (spectrumFifo.getFreeSpace() >= buffer.getNumSamples())
            spectrumFifo.write (buffer);

        if (buffer.getNumChannels() >= 2 && xyFifo.getFreeSpace() >= buffer.getNumSamples())
            xyFifo.write (buffer);
    }

    void timerCallback() override
    {
        if (wasPlaying && ! midiPlayer.isPlaying())
            updateButtons();

        wasPlaying = midiPlayer.isPlaying();
    }

    void updateButtons()
    {
        playButton.setEnabled (midiPlayer.hasFileLoaded() && ! midiPlayer.isPlaying());
        stopButton.setEnabled (midiPlayer.isPlaying() || midiPlayer.getPlayheadPosition() > 0.0);
    }

    // FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override
    {
        for (const auto& file : files)
            if (file.endsWithIgnoreCase (".mid") || file.endsWithIgnoreCase (".midi"))
                return true;

        return false;
    }

    void fileDragEnter (const juce::StringArray&, int, int) override
    {
        isDragOver = true;
        repaint();
    }

    void fileDragExit (const juce::StringArray&) override
    {
        isDragOver = false;
        repaint();
    }

    void filesDropped (const juce::StringArray& files, int, int) override
    {
        isDragOver = false;

        for (const auto& file : files)
        {
            if (file.endsWithIgnoreCase (".mid") || file.endsWithIgnoreCase (".midi"))
            {
                if (midiPlayer.load (juce::File (file)))
                    updateButtons();
                break;
            }
        }

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black.brighter (0.1f));

        if (isDragOver)
        {
            g.setColour (juce::Colours::cyan.withAlpha (0.2f));
            g.fillRect (midiDisplay.getBounds());
        }
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        auto topRow = rc.removeFromTop (80);
        midiDisplay.setBounds (topRow);

        rc.removeFromTop (8);

        auto buttonRow = rc.removeFromTop (24);
        playButton.setBounds (buttonRow.removeFromLeft (60));
        buttonRow.removeFromLeft (8);
        stopButton.setBounds (buttonRow.removeFromLeft (60));

        rc.removeFromTop (8);

        // XY scope should be square - calculate size first
        int xySize = std::min (rc.getWidth() / 2, rc.getHeight());

        // Top row: spectrum on left, xyScope on right
        auto topVisRow = rc.removeFromTop (xySize);
        spectrum.setBounds (topVisRow.removeFromLeft (topVisRow.getWidth() - xySize - 8));
        topVisRow.removeFromLeft (8);
        xyScope.setBounds (topVisRow.removeFromLeft (xySize));

        rc.removeFromTop (8);

        // Bottom row: scope takes remaining space
        scope.setBounds (rc);
    }

    gin::BandLimitedLookupTables bllt { 44100, 12, 2048 };
    gin::Synthesiser synth;
    gin::MidiFilePlayer midiPlayer;
    gin::MidiFileComponent midiDisplay { midiPlayer };

    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };

    gin::AudioFifo scopeFifo { 2, 44100 };
    gin::AudioFifo spectrumFifo { 2, 44100 };
    gin::AudioFifo xyFifo { 2, 44100 };

    gin::TriggeredScope scope { scopeFifo };
    gin::SpectrumAnalyzer spectrum { spectrumFifo };
    gin::XYScope xyScope { xyFifo };

    bool isDragOver = false;
    bool wasPlaying = false;
};

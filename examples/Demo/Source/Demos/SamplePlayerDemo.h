/*
  ==============================================================================

    SamplePlayerDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
struct SamplePlayerDemo : public DemoComponent,
                          public juce::FileDragAndDropTarget,
                          public juce::Timer
{
    SamplePlayerDemo()
    {
        setName ("Sample Player");

        addAndMakeVisible (waveform);
        waveform.setBackgroundColour (juce::Colours::black);
        waveform.setLineColour (juce::Colours::cyan);

        addAndMakeVisible (playButton);
        addAndMakeVisible (stopButton);

        playButton.onClick = [this]
        {
            samplePlayer.play();
            updateButtons();
        };

        stopButton.onClick = [this]
        {
            samplePlayer.stop();
            samplePlayer.setPosition (0.0);
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

        addAndMakeVisible (dropLabel);
        dropLabel.setText ("Drop audio file here", juce::dontSendNotification);
        dropLabel.setJustificationType (juce::Justification::centred);
        dropLabel.setColour (juce::Label::textColourId, juce::Colours::grey);

        scopeFifo.setSize (2, 44100);
        spectrumFifo.setSize (2, 44100);
        xyFifo.setSize (2, 44100);

        samplePlayer.setLooping (true);

        startTimerHz (30);
        updateButtons();
    }

    ~SamplePlayerDemo() override
    {
        stopTimer();
    }

    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused (samplesPerBlock);
        samplePlayer.setPlaybackSampleRate (sampleRate);
        spectrum.setSampleRate (sampleRate);

        scopeFifo.setSize (2, int (sampleRate));
        spectrumFifo.setSize (2, int (sampleRate));
        xyFifo.setSize (2, int (sampleRate));
    }

    void processBlock (juce::AudioBuffer<float>& buffer) override
    {
        samplePlayer.processBlock (buffer);

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
        if (wasPlaying && ! samplePlayer.isPlaying())
            updateButtons();

        wasPlaying = samplePlayer.isPlaying();

        if (samplePlayer.hasFileLoaded())
            waveform.setPlayheads ({ static_cast<int> (samplePlayer.getPosition()) });
    }

    void updateButtons()
    {
        playButton.setEnabled (samplePlayer.hasFileLoaded() && ! samplePlayer.isPlaying());
        stopButton.setEnabled (samplePlayer.isPlaying() || samplePlayer.getPosition() > 0.0);
        dropLabel.setVisible (! samplePlayer.hasFileLoaded());
    }

    // FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override
    {
        juce::AudioFormatManager mgr;
        mgr.registerBasicFormats();

        for (const auto& file : files)
            if (mgr.findFormatForFileExtension (juce::File (file).getFileExtension()) != nullptr)
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

        juce::AudioFormatManager mgr;
        mgr.registerBasicFormats();

        for (const auto& file : files)
        {
            juce::File f (file);
            if (mgr.findFormatForFileExtension (f.getFileExtension()) != nullptr)
            {
                samplePlayer.load (f);
                waveform.setBuffer (samplePlayer.getBuffer());
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
            g.fillRect (waveform.getBounds());
        }
    }

    void resized() override
    {
        auto rc = getLocalBounds().reduced (8);

        auto topRow = rc.removeFromTop (80);
        waveform.setBounds (topRow);
        dropLabel.setBounds (topRow);

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

    gin::SamplePlayer samplePlayer;
    gin::AudioSamplerBufferComponent waveform;
    juce::TextButton playButton { "Play" };
    juce::TextButton stopButton { "Stop" };
    juce::Label dropLabel;

    gin::AudioFifo scopeFifo { 2, 44100 };
    gin::AudioFifo spectrumFifo { 2, 44100 };
    gin::AudioFifo xyFifo { 2, 44100 };

    gin::TriggeredScope scope { scopeFifo };
    gin::SpectrumAnalyzer spectrum { spectrumFifo };
    gin::XYScope xyScope { xyFifo };

    bool isDragOver = false;
    bool wasPlaying = false;
};

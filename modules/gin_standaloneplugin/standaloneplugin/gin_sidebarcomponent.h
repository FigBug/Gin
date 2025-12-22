#pragma once

class StandaloneFilterWindow;

//==============================================================================
class HeaderComponent : public juce::Component
{
public:
    HeaderComponent (const juce::String& text) : title (text) {}

    void paint (juce::Graphics& g) override
    {
        g.setColour (findColour (GinLookAndFeel::accentColourId).withAlpha (0.6f));
        g.fillAll();

        g.setColour (findColour (GinLookAndFeel::matte1ColourId));
        g.setFont (12.0f);
        g.drawText (title, getLocalBounds().reduced (4, 0), juce::Justification::centredLeft);
    }

private:
    juce::String title;
};

//==============================================================================
class SampleFileComponent : public AudioSamplerBufferComponent,
                            public juce::FileDragAndDropTarget,
                            public juce::Timer
{
public:
    SampleFileComponent (SamplePlayer& player) : samplePlayer (player)
    {
        startTimerHz (30);
    }

    ~SampleFileComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        const bool isPlaying = samplePlayer.isPlaying();

        if (wasPlaying && ! isPlaying)
        {
            if (onPlaybackStateChanged)
                onPlaybackStateChanged();
        }

        if (samplePlayer.hasFileLoaded())
        {
            const double pos = samplePlayer.getPosition();
            setPlayheads ({ static_cast<int> (pos) });
        }

        if (isPlaying || wasPlaying != isPlaying)
            repaint();

        wasPlaying = isPlaying;
    }

    bool isInterestedInFileDrag (const juce::StringArray& files) override
    {
        juce::AudioFormatManager mgr;
        mgr.registerBasicFormats();

        for (const auto& file : files)
        {
            if (mgr.findFormatForFileExtension (juce::File (file).getFileExtension()) != nullptr)
                return true;
        }
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
                setBuffer (samplePlayer.getBuffer());
                if (onFileDropped)
                    onFileDropped();
                break;
            }
        }

        repaint();
    }

    void paint (juce::Graphics& g) override
    {
        AudioSamplerBufferComponent::paint (g);

        if (isDragOver)
        {
            g.setColour (findColour (GinLookAndFeel::accentColourId).withAlpha (0.2f));
            g.fillAll();
        }

        g.setColour (findColour (GinLookAndFeel::grey45ColourId));
        g.drawRect (getLocalBounds());
    }

    std::function<void()> onFileDropped;
    std::function<void()> onPlaybackStateChanged;

private:
    SamplePlayer& samplePlayer;
    bool isDragOver = false;
    bool wasPlaying = false;
};

//==============================================================================
class RetrospectiveBufferComponent : public AudioSamplerBufferComponent,
                                      public juce::Timer
{
public:
    RetrospectiveBufferComponent (AudioRecorder& recorder) : audioRecorder (recorder)
    {
        startTimerHz (10);
    }

    ~RetrospectiveBufferComponent() override
    {
        stopTimer();
    }

    void timerCallback() override
    {
        auto numReady = audioRecorder.getRetrospectiveNumReady();

        if (numReady > 0 && audioRecorder.getRetrospectiveNumChannels() > 0)
        {
            ScratchBuffer displayBuffer (audioRecorder.getRetrospectiveNumChannels(), numReady);
            if (audioRecorder.peekRetrospective (displayBuffer))
                setBuffer (displayBuffer);
        }
    }

    void paint (juce::Graphics& g) override
    {
        AudioSamplerBufferComponent::paint (g);

        g.setColour (findColour (GinLookAndFeel::grey45ColourId));
        g.drawRect (getLocalBounds());
    }

private:
    AudioRecorder& audioRecorder;
};

//==============================================================================
class SideBarComponent : public juce::Component,
                         public juce::Timer
{
public:
    SideBarComponent (StandaloneFilterWindow& filterWindow);
    ~SideBarComponent() override;

    void resized() override;
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

    static constexpr int width = 200;

private:
    void updatePlayStopButtons();
    void saveMidiFilePath();
    void restoreMidiFilePath();
    void showMidiMenu();
    void addToMidiMRU (const juce::File& file);
    juce::StringArray getMidiMRU();

    void updateSamplePlayStopButtons();
    void saveSampleFilePath();
    void restoreSampleFilePath();
    void showSampleMenu();
    void addToSampleMRU (const juce::File& file);
    juce::StringArray getSampleMRU();

    void updateRecordButtons();
    void updateRecordButtonColours();
    void showRecordMenu();

    StandaloneFilterWindow&         filterWindow;
    StandaloneAudioProcessorPlayer& player;

    CopperLookAndFeel   lf;

    HeaderComponent     midiHeader { "Midi" };
    MidiFileComponent   midi { player.midiPlayer };
    SVGButton           midiPlay;
    SVGButton           midiStop;
    SVGButton           midiMenu;

    HeaderComponent     sampleHeader { "Sample" };
    SampleFileComponent sample { player.samplePlayer };
    SVGButton           samplePlay;
    SVGButton           sampleStop;
    SVGButton           sampleMenu;

    HeaderComponent               recordHeader { "Record" };
    RetrospectiveBufferComponent  recordBuffer { player.audioRecorder };
    SVGButton                     recordNormal;
    SVGButton                     recordTriggered;
    SVGButton                     recordRetrospective;
    SVGButton                     recordStop;
    SVGButton                     recordMenu;
    bool                          flashState = false;

    HeaderComponent     outputHeader { "Output" };
    juce::Slider        outputGain;

    HeaderComponent     scopeHeader { "Scope" };
    TriggeredScope      scope { player.scopeFifo };

    HeaderComponent     spectrumHeader { "Spectrum" };
    SpectrumAnalyzer    spectrum { player.spectrumFifo };

    HeaderComponent     xyHeader { "XY" };
    XYScope             xyScope { player.xyFifo };
};

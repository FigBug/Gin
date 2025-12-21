/*
  ==============================================================================

  This file is based on part of the dRowAudio JUCE module
  Copyright 2004-13 by dRowAudio.
  dRowAudio is provided under the terms of The MIT License (MIT):

  ==============================================================================
*/


#pragma once

/** Triggered Scope that it can be set to start on a rising or falling signal.
    This makes it extremely useful for very zoomed-in waveform viewing.
*/
class TriggeredScope : public juce::Component,
                       public juce::Timer
{
public:

    /** Creates a Triggered scope.

        Pass a fifo to read from
    */
    TriggeredScope (AudioFifo&);

    /** Destructor. */
    ~TriggeredScope() override;

    enum ColourIds
    {
        lineColourId             = 0x1231e10,
        backgroundColourId       = 0x1231e11,
        traceColourId            = 0x1231e12,
        envelopeColourId         = 0x1231f13,
    };

    void setNumChannels (int num);

    //==============================================================================
    /** Sets the number of samples represented by each pixel on the scope.
        Setting this to a low number will give a very zoomed in display, a high
        number zoom out.
     */
    void setNumSamplesPerPixel (float newNumSamplesPerPixel);

    /** Sets the vertical zoom factor of the display. */
    void setVerticalZoomFactor (float newVerticalZoomFactor);

    /** Sets the vertical zoom offset of the display. */
    void setVerticalZoomOffset (float newVerticalZoomOffset, int ch);

    /** Freeze scope once triggered */
    void setSingleTrigger (bool singleTrigger_) { singleTrigger = singleTrigger_; }

    /** Start running again once triggered */
    void resetTrigger();

    //==============================================================================
    /** The enum to use when setting the trace trigger mode. */
    enum TriggerMode
    {
        None = 0,   //< The trace will just refresh at a constant rate.
        Up,         //< The start of the trace will be a rising edge.
        Down,       //< The start of the trace will be a falling edge.
        Auto        //< Automatically find the best trigger point using autocorrelation.
    };

    /** Sets the type of change that will trigger a trace. */
    void setTriggerMode (TriggerMode newTriggerMode);

    /** Set channel to trigger on, -1 ave of all channels */
    void setTriggerChannel (int ch) { triggerChannel = ch; }

    /** Set level to trigger on */
    void setTriggerLevel (float l)  { triggerLevel = l;     }

    void setTriggerPos (float l)    { triggerPos = l;       }

    void setDrawTriggerPos (bool d) { drawTriggerPos = d;   }

    /** Enable/disable cursor readout showing level and time at mouse position */
    void setDrawCursorInfo (bool d) { drawCursorInfo = d; }

    /** Set sample rate for time display */
    void setSampleRate (double sr)  { sampleRate = sr; }

    /** Set beat sync mode - 0 = off, otherwise number of beats to sync to */
    void setBeatSync (int beats)    { if (beats != beatSyncBeats) { beatSyncBeats = beats; lastBeatCycle = -1; } }

    /** Set a function to retrieve playhead info - called from timer */
    void setPlayheadSource (std::function<std::tuple<double, double, bool>()> source) { playheadSource = std::move (source); }

    /** Call this from processBlock to update playhead position */
    void updatePlayhead (double ppqPosition, double bpm, bool isPlaying);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void timerCallback() override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

private:
    //==============================================================================
    AudioFifo& fifo;
    TriggerMode triggerMode = Up;
    float numSamplesPerPixel = 4.0f;
    float verticalZoomFactor = 1.0f;
    juce::Array<float> verticalZoomOffset;
    float triggerLevel = 0.0f;
    float triggerPos = 0.0f;
    int triggerChannel = -1;
    bool drawTriggerPos = false;
    bool singleTrigger = false;
    int triggerPoint = -1;
    int samplesSinceTrigger = 0;

    struct Channel
    {
        Channel() :
          numLeftToAverage (4),
          bufferSize (4096),
          bufferWritePos (0),
          numAveraged (0),
          posBuffer ((size_t) bufferSize),
          minBuffer ((size_t) bufferSize),
          maxBuffer ((size_t) bufferSize),
          currentAve (0.0f),
          currentMax (-1.0f),
          currentMin (1.0f),
          samplesToProcess (1, 32768),
          tempProcessingBlock (32768)
        {}

        int numLeftToAverage;
        int bufferSize, bufferWritePos, numAveraged;

        juce::HeapBlock<float> posBuffer, minBuffer, maxBuffer;

        float currentAve, currentMax, currentMin;
        AudioFifo samplesToProcess;
        juce::HeapBlock<float> tempProcessingBlock;
    };

    juce::OwnedArray<Channel> channels;

    bool needToUpdate = false;
    bool drawCursorInfo = false;
    double sampleRate = 44100.0;
    std::optional<juce::Point<int>> mousePos;

    // Beat sync state
    int beatSyncBeats = 0;
    std::atomic<double> currentBpm { 120.0 };
    std::atomic<bool> hostIsPlaying { false };
    std::atomic<int> beatSyncCycleStartPos { -1 };  // Buffer position where current cycle started
    std::atomic<int> beatSyncTotalSamples { 0 };    // Total buffer samples in one cycle
    std::atomic<float> beatSyncSamplesPerPixel { 1.0f };
    int lastBeatCycle { -1 };
    std::function<std::tuple<double, double, bool>()> playheadSource;

    // Auto trigger mode state
    std::vector<int> autoCandidates;
    std::vector<int> autoRatings;
    std::vector<uint8_t> lastMatchMask;
    int lowFreqDelay = 0;
    static constexpr int s_lowFreqThreshold = 3;
    static constexpr int s_maxLowFreqDelay = 10;
    int autoTriggerPos = 0;

    //==============================================================================
    void addSamples (const juce::AudioSampleBuffer& buffer);
    void processPendingSamples();
    void render (juce::Graphics& g);
    std::pair<int, bool> getTriggerPos();
    void updateAutoTrigger();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TriggeredScope)
};

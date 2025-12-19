#pragma once

/** Draws an XY Scope with history-based fading
*/
class XYScope : public juce::Component,
                public juce::Timer
{
public:

    /** Creates a XY scope.

        Pass a fifo to read from
    */
    XYScope (AudioFifo&);

    /** Destructor. */
    ~XYScope() override;

    enum ColourIds
    {
        lineColourId             = 0x1291e10,
        backgroundColourId       = 0x1291e11,
        traceColourId            = 0x1291e12
    };

    //==============================================================================
    /** Sets the zoom factor of the display. */
    void setZoomFactor (float newZoomFactor);

    /** Sets the number of history frames to display with fading. */
    void setHistorySize (int numFrames);

    /** Sets the number of points per frame. */
    void setBlockSize (int size);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

private:
    //==============================================================================
    AudioFifo& fifo;
    float zoomFactor = 1.0f;
    int historySize = 32;
    int blockSize = 32;

    // History of point arrays for fading effect
    std::vector<std::vector<juce::Point<float>>> history;
    int currentHistoryIndex = 0;

    struct Channel
    {
        Channel() :
          bufferSize (4096),
          bufferWritePos (0),
          xBuffer ((size_t) bufferSize),
          yBuffer ((size_t) bufferSize),
          samplesToProcess (2, 32768)
        {}

        int bufferSize, bufferWritePos;

        juce::HeapBlock<float> xBuffer, yBuffer;

        AudioFifo samplesToProcess;
    };

    Channel channel;

    bool needToUpdate = false;
    int samplessinceLastFrame = 0;

    //==============================================================================
    void addSamples (const juce::AudioSampleBuffer& buffer);
    void processPendingSamples();
    void render (juce::Graphics& g);
    void buildCurrentFrame();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYScope)
};

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
    /** Sets the number of samples represented by each pixel on the scope.
        Setting this to a low number will give a very zoomed in display, a high
        number zoom out.
     */
    void setNumSamplesPerPixel (float newNumSamplesPerPixel);

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
    float numSamplesPerPixel = 4.0f;
    float zoomFactor = 1.0f;
    int historySize = 32;
    int blockSize = 32;

    // History of point arrays for fading effect
    std::vector<std::vector<juce::Point<float>>> history;
    int currentHistoryIndex = 0;

    struct Channel
    {
        Channel() :
          numLeftToAverage (4.0f),
          bufferSize (4096),
          bufferWritePos (0),
          xBuffer ((size_t) bufferSize),
          yBuffer ((size_t) bufferSize),
          currentX (0.0f),
          currentY (0.0f),
          samplesToProcess (2, 32768)
        {}

        int numAveraged = 0;
        float numLeftToAverage;
        int bufferSize, bufferWritePos;

        juce::HeapBlock<float> xBuffer, yBuffer;

        float currentX, currentY;
        AudioFifo samplesToProcess;
    };

    Channel channel;

    bool needToUpdate = false;

    //==============================================================================
    void addSamples (const juce::AudioSampleBuffer& buffer);
    void processPendingSamples();
    void render (juce::Graphics& g);
    void buildCurrentFrame();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYScope)
};

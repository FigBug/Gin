#pragma once

/** Draws an XY Scope
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

    /** Sets the vertical zoom factor of the display. */
    void setZoomFactor (float newZoomFactor);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

private:
    //==============================================================================
    AudioFifo& fifo;
    float numSamplesPerPixel = 4.0f;
    float zoomFactor = 1.0f;
    
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
        juce::HeapBlock<float> tempProcessingBlock;
    };
    
    Channel channel;
    
    bool needToUpdate = false;

    //==============================================================================
    void addSamples (const juce::AudioSampleBuffer& buffer);
    void processPendingSamples();
    void render (juce::Graphics& g);
  
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYScope)
};

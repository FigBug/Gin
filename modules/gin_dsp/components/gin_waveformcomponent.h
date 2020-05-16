#pragma once

/** Draws a waveform
*/
class WaveformComponent : public Component
{
public:
    WaveformComponent();

    /** Destructor. */
    ~WaveformComponent() override;
    
    enum ColourIds
    {
        lineColourId             = 0x1231e10,
        backgroundColourId       = 0x1231e11, 
        traceColourId            = 0x1231e12,
        envelopeColourId         = 0x1231f13,
    };
    
    void setHorizontalZoom (float zoom);
    void setHorizontalOffset (float offset);

    void setBuffer (AudioSampleBuffer& buffer);
    //==============================================================================
    void paint (Graphics& g) override;
    void resized() override             { needsUpdate = true; }

private:
    //==============================================================================
    float zoom = 1.0f;
    float offset = 0.0f;
    bool needsUpdate = true;
    AudioSampleBuffer* buffer = nullptr;
    
    struct Channel
    {
        Channel()
        {
            setSize (0);
        }
        
        void setSize (int sz)
        {
            if (sz != bufferSize)
            {
                bufferSize = 4096;
                posBuffer.resize (bufferSize);
                minBuffer.resize (bufferSize);
                maxBuffer.resize (bufferSize);
            }
        }
        
        int bufferSize = 0;
        Array<float> posBuffer, minBuffer, maxBuffer;
    };
    
    OwnedArray<Channel> channels;

    //==============================================================================
    void render (Graphics& g);
    void processPendingSamples();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

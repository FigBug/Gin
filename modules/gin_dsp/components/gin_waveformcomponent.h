#pragma once

/**
    Visual component for displaying audio waveforms with zoom and scroll.

    WaveformComponent renders multi-channel audio waveforms with support for
    horizontal zooming and panning. It efficiently displays large audio buffers
    by downsampling to min/max envelopes at the current zoom level, providing
    smooth visualization even for long audio files.

    Key Features:
    - Multi-channel waveform display
    - Horizontal zoom and scroll
    - Efficient downsampling for display
    - Min/max envelope rendering
    - Customizable colors via ColourIds
    - Automatic updates on resize
    - Optimized for real-time display

    Color Customization:
    - lineColourId: Color for zero-crossing line
    - backgroundColourId: Background color
    - traceColourId: Waveform trace color
    - envelopeColourId: Waveform envelope fill color

    The component automatically processes the audio buffer into min/max pairs
    for efficient rendering at any zoom level, avoiding the need to draw
    individual samples when zoomed out.

    Usage:
    @code
    WaveformComponent waveform;
    waveform.setBounds(0, 0, 800, 200);

    // Set audio to display
    AudioBuffer<float> audioData(2, 44100); // 2 channels, 1 second
    // ... fill with audio data ...
    waveform.setBuffer(audioData);

    // Customize colors
    waveform.setColour(WaveformComponent::traceColourId, Colours::blue);
    waveform.setColour(WaveformComponent::envelopeColourId,
                      Colours::lightblue.withAlpha(0.3f));

    // Zoom and scroll
    waveform.setHorizontalZoom(2.0f);  // 2x zoom
    waveform.setHorizontalOffset(0.25f); // Start at 25% through buffer
    @endcode

    @see juce::AudioSampleBuffer, juce::Component
*/
class WaveformComponent : public juce::Component
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

    void setBuffer (juce::AudioSampleBuffer& buffer);
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override             { needsUpdate = true; }

private:
    //==============================================================================
    float zoom = 1.0f;
    float offset = 0.0f;
    bool needsUpdate = true;
    juce::AudioSampleBuffer* buffer = nullptr;
    
    /**
        Internal storage for per-channel waveform display data.

        Channel holds the downsampled min/max envelope data for a single
        audio channel. The buffers store position, minimum, and maximum
        values for efficient waveform rendering at any zoom level.
    */
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
        juce::Array<float> posBuffer, minBuffer, maxBuffer;
    };
    
    juce::OwnedArray<Channel> channels;

    //==============================================================================
    void render (juce::Graphics& g);
    void processPendingSamples();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformComponent)
};

#pragma once

/** Draws an AudioSampleBuffer thumbnail
*/
class AudioSamplerBufferComponent : public juce::Component
{
public:
    AudioSamplerBufferComponent();
    ~AudioSamplerBufferComponent() override;

    void setBuffer (const juce::AudioSampleBuffer&);
    void clear ();

    void setScrollable (bool s)     { scrollable = s;                   }
    int getLength()                 { return buffer.getNumSamples();    }
    void setHiResDisabled (bool b)  { hiResDisabled = b;                }

    float getViewLeft();
    float getViewRight();

    void scrollViewBounds (float delta);
    void setViewLeft (float left);
    void setViewRight (float right);
    void setViewBounds (float left, float right);
    void setMaxZoom (float maxZoom);

    void zoom (float centrePoint, float factor);

    void setPlayheads (const std::vector<int>& playheads);

    float xToSample (float x);
    float sampleToX (int sample);

    void setLineColour (juce::Colour c)             { lineColour = c;   }
    void setBackgroundColour (juce::Colour c)       { bkColour = c;     }

    void paint ( juce::Graphics& ) override;
    void resized() override;
    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;

    juce::MouseCursor getMouseCursor() override;
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

private:
    struct Channel
    {
        void setSize (int s)
        {
            if (bufferSize != s)
            {
                bufferSize = s;

                minBuffer.resize (bufferSize);
                maxBuffer.resize (bufferSize);
            }
        }

        int bufferSize = 0;

        juce::Array<float>  minBuffer;
        juce::Array<float>  maxBuffer;
        juce::Path          path;
    };

    void setNumChannels (int num);
    void updatePixelCache();
    void updatePixelCacheHiRes();

    float getNumSamplesPerPixel();

    void paintWaveform (juce::Graphics& g, bool enabled);
    bool hiRes ();

    juce::OwnedArray<Channel> channels;
    std::vector<int> playheads;

    juce::AudioBuffer<float> buffer;

    float           viewLeft        = 0.0f;
    float           viewWidth       = 44100.0f;
    float           maxZoom         = 0.0f;
    float           mouseDownSample = -1.0f;

    bool            dragScrolling   = false;
    bool            scrollable      = false;
    bool            pixelCacheDirty = false;
    bool            hiResDisabled   = false;

    juce::Colour    lineColour      { juce::Colours::white.withAlpha (0.75f) };
    juce::Colour    bkColour        { juce::Colours::transparentBlack };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioSamplerBufferComponent)
};

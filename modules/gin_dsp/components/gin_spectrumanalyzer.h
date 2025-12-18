#pragma once

class SpectrumAnalyzer : public juce::Component,
                         public juce::Timer
{
public:

    /** Creates a Spectrum Analyzer

        Pass a fifo to read from
    */
    SpectrumAnalyzer (AudioFifo&);

    /** Destructor. */
    ~SpectrumAnalyzer() override;

    enum ColourIds
    {
        lineColourId             = 0x1291e10,
        backgroundColourId       = 0x1291e11,
        traceColourId            = 0x1291e12,
        envelopeColourId         = 0x1291f13,
    };

    void setNumChannels (int num);

    /** Set the frequency range to display (in Hz) */
    void setFrequencyRange (float minFreq, float maxFreq);

    /** Set the dB range to display */
    void setDbRange (float minDb, float maxDb);

    /** Set the sample rate for frequency calculations */
    void setSampleRate (double sr);

    /** Set smoothing factor for the display (0-1, higher = smoother) */
    void setSmoothing (float smoothing);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

private:
    //==============================================================================
    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;  // 2048

    AudioFifo& fifo;

    struct Channel
    {
        Channel();

        std::vector<float> inputBuffer;
        int inputBufferPos = 0;
        std::vector<float> fftData;
        std::vector<float> bins;
        std::vector<float> smoothedBins;
        juce::Path path;
    };

    juce::OwnedArray<Channel> channels;

    juce::dsp::FFT fft { fftOrder };
    juce::dsp::WindowingFunction<float> window { fftSize, juce::dsp::WindowingFunction<float>::hann };

    float minFrequency = 20.0f;
    float maxFrequency = 20000.0f;
    float minDecibels = -100.0f;
    float maxDecibels = 0.0f;
    double sampleRate = 44100.0;
    float smoothingFactor = 0.8f;

    bool needToUpdate = false;

    //==============================================================================
    void processFFT (Channel* channel);
    void updatePaths();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};

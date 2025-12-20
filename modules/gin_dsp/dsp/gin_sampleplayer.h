#pragma once

class SamplePlayer
{
public:
    SamplePlayer();
    SamplePlayer (const char* data, int dataLen);
    SamplePlayer (const juce::File& f);

    void load (const char* data, int dataLen);
    void load (const juce::File& f);
    void setBuffer (const juce::AudioSampleBuffer& newBuffer, double sampleRate);
    void setPlaybackSampleRate (double sampleRate);
    void reset();
    void setCrossfadeSamples (int samples);

    void play();
    void stop();
    bool isPlaying() const { return playing.load(); }

    void setLooping (bool shouldLoop) { looping.store (shouldLoop); }
    bool isLooping() const { return looping.load(); }

    void processBlock (juce::AudioSampleBuffer& output);

    double getPosition() const;
    void setPosition (double newPosition);

    bool hasFileLoaded() const { return fileLoaded.load(); }
    juce::File getLoadedFile() const { return loadedFilePath; }
    const juce::AudioSampleBuffer& getBuffer() const { return buffer; }
    double getSourceSampleRate() const { return sourceSampleRate; }
    void clear();

private:
    float interpolateSampleWithCrossfade (int channel, double pos);
    float interpolateSample (int channel, double samplePosition);

    juce::SpinLock lock;
    juce::File loadedFilePath;
    juce::AudioSampleBuffer buffer;
    double sourceSampleRate = 44100.0;
    double playbackSampleRate = 44100.0;
    std::atomic<double> position { 0.0 };
    int crossfadeSamples = 1000;
    std::atomic<bool> playing { false };
    std::atomic<bool> looping { true };
    std::atomic<bool> fileLoaded { false };
};

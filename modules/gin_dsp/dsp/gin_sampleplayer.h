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
    double getPositionInSeconds() const;
    void setPosition (double newPosition);

    double getLengthInSeconds() const;

    bool hasFileLoaded() const { return fileLoaded.load(); }
    juce::File getLoadedFile() const { return loadedFilePath; }
    const juce::AudioSampleBuffer& getBuffer() const { return buffer; }
    double getSourceSampleRate() const { return sourceSampleRate; }
    void clear();

    /** Returns position info for use with an AudioPlayHead.
        Call from the audio thread before processing.
        @return PositionInfo populated with current playback state
    */
    juce::AudioPlayHead::PositionInfo populatePositionInfo();

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
    double bpm { 120.0 };
    int timeSigNumerator { 4 };
    int timeSigDenominator { 4 };
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "gin_audioutil.h"

/**
    High-quality sample rate conversion FIFO using libsamplerate.

    ResamplingFifo provides real-time sample rate conversion with buffering,
    allowing you to push audio at one sample rate and pull it out at another.
    It uses the Secret Rabbit Code (libsamplerate) library for high-quality
    resampling with configurable quality levels.

    Key Features:
    - High-quality resampling (5 quality levels)
    - Real-time operation with internal buffering
    - Multi-channel support
    - Dynamic ratio adjustment
    - Read with or without consuming (adding mode)

    Quality Levels (0-4):
    - 0: SRC_SINC_FASTEST (lowest quality, fastest)
    - 1: SRC_SINC_MEDIUM_QUALITY
    - 2: SRC_SINC_BEST_QUALITY (highest quality, slowest)
    - 3: SRC_ZERO_ORDER_HOLD (for non-audio signals)
    - 4: SRC_LINEAR (simple linear interpolation)

    Common Use Cases:
    - Playing back audio files at different sample rates
    - Converting between plugin and host sample rates
    - Time-stretching (with dynamic ratio changes)
    - Sample rate conversion in offline processing

    Usage:
    @code
    ResamplingFifo resampler(512, 2, 44100); // 512 block, 2 channels, max 44100 samples
    resampler.setResamplingRatio(48000.0, 44100.0); // 48kHz to 44.1kHz
    resampler.setQuality(2); // Best quality

    // Push audio at input rate
    AudioBuffer<float> input48k(2, 512);
    resampler.pushAudioBuffer(input48k);

    // Pull audio at output rate
    AudioBuffer<float> output44k(2, 460); // ~460 samples at 44.1kHz
    if (resampler.samplesReady() >= 460)
        resampler.popAudioBuffer(output44k);
    @endcode

    @see AudioFifo, resampleBuffer
*/
class ResamplingFifo
{
public:
    ResamplingFifo (int blockSize, int numChannels = 2, int maxSamples = 44100);
    ~ResamplingFifo ();

    void setSize (int blockSize, int numChannels = 2, int maxSamples = 44100);

    void setResamplingRatio (double inputRate, double outputRate);
    void setRatio (float ratio);
    float getRatio();

    /** 0 - 4: 0 is worst, 4 is best */
    void setQuality (int q);

    void reset();
    int samplesReady()                                              { return outputFifo.getNumReady(); }

    void pushAudioBuffer (const juce::AudioSampleBuffer& buffer);
    void popAudioBuffer (juce::AudioSampleBuffer& buffer);
    void popAudioBufferAdding (juce::AudioSampleBuffer& buffer);

private:
    void pushAudioBufferInt (const juce::AudioSampleBuffer& buffer);
    int getQuality (int q);

    struct Impl;
    std::unique_ptr<Impl> impl;

    int quality = 5;
    int numChannels = 0, blockSize = 0;
    float ratio = 1.0f;
    AudioFifo outputFifo;
    juce::AudioSampleBuffer ilInputBuffer, ilOutputBuffer, outputBuffer;
};

/**
    Resamples an entire audio buffer from one sample rate to another.

    This is a convenience function that resamples a complete audio buffer in one
    operation, without requiring a ResamplingFifo. Useful for offline processing
    where you need to convert an entire buffer at once.

    @param buffer       The audio buffer to resample (input)
    @param inputRate    The sample rate of the input buffer
    @param outputRate   The desired output sample rate
    @param quality      Resampling quality (0-4, default 4):
                        0 = SRC_SINC_FASTEST,
                        1 = SRC_SINC_MEDIUM_QUALITY,
                        2 = SRC_SINC_BEST_QUALITY,
                        3 = SRC_ZERO_ORDER_HOLD,
                        4 = SRC_LINEAR
    @returns            A new AudioSampleBuffer at the output sample rate

    @see ResamplingFifo
*/
juce::AudioSampleBuffer resampleBuffer (juce::AudioSampleBuffer&, double inputRate, double outputRate, int quality = 4);

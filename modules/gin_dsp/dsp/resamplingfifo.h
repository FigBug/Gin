/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "audioutil.h"

/** ResamplingFifo - based on the algorithm from juce::ResamplingAudioSource
 */
class ResamplingFifo
{
public:
    ResamplingFifo (int expectedSamplesPerBlock, int numChannels = 2, int maxSamples = 44100);

    void setResamplingRatio (double inputRate, double outputRate);
    double getResamplingRatio() const noexcept                      { return ratio; }

    void flushBuffers();
    int samplesReady()                                              { return outputFifo.getNumReady(); }

    void pushAudioBuffer (const AudioSampleBuffer& buffer);
    void popAudioBuffer (AudioSampleBuffer& buffer);

private:
    void prepare();
    void process();

    //==============================================================================
    double ratio = 1.0, lastRatio = 1.0;
    AudioBuffer<float> buffer, outBuffer;
    int bufferPos = 0, sampsInBuffer = 0;
    double subSampleOffset = 0.0;
    double coefficients[6];
    const int numChannels, blockSize;
    HeapBlock<float*> destBuffers;
    HeapBlock<const float*> srcBuffers;

    AudioFifo inputFifo, outputFifo;

    void setFilterCoefficients (double c1, double c2, double c3, double c4, double c5, double c6);
    void createLowPass (double proportionalRate);

    struct FilterState
    {
        double x1, x2, y1, y2;
    };

    HeapBlock<FilterState> filterStates;
    void resetFilters();

    void applyFilter (float* samples, int num, FilterState& fs);
};

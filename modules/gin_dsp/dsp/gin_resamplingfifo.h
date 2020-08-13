/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "gin_audioutil.h"

/** ResamplingFifo - uses secret rabbit code
 */
class ResamplingFifo
{
public:
    ResamplingFifo (int blockSize, int numChannels = 2, int maxSamples = 44100);
    ~ResamplingFifo ();
    
    void setSize (int blockSize, int numChannels = 2, int maxSamples = 44100);

    void setResamplingRatio (double inputRate, double outputRate);
    void setRatio (float ratio);

    void reset();
    int samplesReady()                                              { return outputFifo.getNumReady(); }

    void pushAudioBuffer (const juce::AudioSampleBuffer& buffer);
    void popAudioBuffer (juce::AudioSampleBuffer& buffer);

private:
    void pushAudioBufferInt (const juce::AudioSampleBuffer& buffer);
    
    struct Impl;
    std::unique_ptr<Impl> impl;
    
    int numChannels = 0, blockSize = 0;
    float ratio = 1.0f;
    AudioFifo outputFifo;
    juce::AudioSampleBuffer ilInputBuffer, ilOutputBuffer, outputBuffer;
};

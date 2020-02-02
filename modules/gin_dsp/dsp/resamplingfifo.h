/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#pragma once

#include "audioutil.h"

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

    void pushAudioBuffer (const AudioSampleBuffer& buffer);
    void popAudioBuffer (AudioSampleBuffer& buffer);

private:
    void pushAudioBufferInt (const AudioSampleBuffer& buffer);
    
    struct Impl;
    std::unique_ptr<Impl> impl;
    
    int numChannels = 0, blockSize = 0;
    float ratio = 1.0f;
    AudioFifo outputFifo;
    AudioSampleBuffer ilInputBuffer, ilOutputBuffer, outputBuffer;
};

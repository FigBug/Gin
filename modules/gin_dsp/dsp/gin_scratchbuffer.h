/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
*/

#pragma once

class BufferCacheItem;

/** A temporay buffer provided from a list.
    Get a buffer without allocating.
    (It does lock however)
*/
class ScratchBuffer : public juce::AudioSampleBuffer
{
public:
    ScratchBuffer (int numChannels, int numSamples);
    ScratchBuffer (juce::AudioSampleBuffer& buffer);

    ScratchBuffer (const ScratchBuffer&);

    ~ScratchBuffer();

private:
    ScratchBuffer (BufferCacheItem&);
    BufferCacheItem& cache;
};

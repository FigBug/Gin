/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
*/

#pragma once

class BufferCacheItem;

class ScratchBuffer : public AudioSampleBuffer
{
public:
    ScratchBuffer (int numChannels, int numSamples);
    ScratchBuffer (AudioSampleBuffer& buffer);
    
    ~ScratchBuffer();
    
private:
    ScratchBuffer (BufferCacheItem&);
    BufferCacheItem& cache;
};

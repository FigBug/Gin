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

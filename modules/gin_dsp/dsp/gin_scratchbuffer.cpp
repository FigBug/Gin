/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#include <cmath>

JUCE_IMPLEMENT_SINGLETON(BufferCache)

//==============================================================================

ScratchBuffer::ScratchBuffer (BufferCacheItem& i)
    : juce::AudioSampleBuffer (i.data.getArrayOfWritePointers(), i.chans, 0, i.samps),
    cache (i)
{
}

ScratchBuffer::ScratchBuffer (int numChannels, int numSamples)
    : ScratchBuffer (*BufferCache::getInstance()->get (numChannels, numSamples))
{
    clear();
}

inline BufferCacheItem& copyCache (BufferCacheItem& other)
{
    BufferCache::getInstance()->incRef (other);
    return other;
}

ScratchBuffer::ScratchBuffer (const ScratchBuffer& other)
    : cache (copyCache (other.cache))
{
}

ScratchBuffer::ScratchBuffer (juce::AudioSampleBuffer& buffer)
    : ScratchBuffer (*BufferCache::getInstance()->get (buffer.getNumChannels(), buffer.getNumSamples()))
{
    for (int i = buffer.getNumChannels(); --i >= 0;)
        copyFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
}

ScratchBuffer::~ScratchBuffer()
{
    BufferCache::getInstance()->free (cache);
}


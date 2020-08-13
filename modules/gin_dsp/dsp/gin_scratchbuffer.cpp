/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

#include <cmath>

//==============================================================================
class BufferCacheItem
{
public:
    BufferCacheItem (int c = 2, int s = 44100) : data (c, s), chans (c), samps (s) {}

    void resize (int c, int s)
    {
        chans = c;
        samps = s;

        data.setSize (c, s);
    }

    juce::AudioSampleBuffer data;
    bool busy = false;
    int chans = 0, samps = 0;
};

//==============================================================================
class BufferCache : private juce::DeletedAtShutdown
{
public:
    BufferCache()
    {
        for (int i = 0; i < 10; i++)
            cache.add (new BufferCacheItem());
    }

    ~BufferCache()
    {
        clearSingletonInstance();
    }

    BufferCacheItem* get (int channels, int samples)
    {
        if (auto i = find (channels, samples))
        {
            if (channels > i->data.getNumChannels() || samples > i->data.getNumChannels())
                i->resize (channels, samples);

            return i;
        }

        auto i = new BufferCacheItem (channels, samples);
        i->busy = true;

        juce::ScopedLock sl (lock);
        cache.add (i);
        return i;
    }

    void free (BufferCacheItem& i)
    {
        juce::ScopedLock sl (lock);
        i.busy = false;
    }

    JUCE_DECLARE_SINGLETON(BufferCache, false)

private:
    BufferCacheItem* find (int channels, int samples)
    {
        juce::ScopedLock sl (lock);

        // First look for one the correct size
        for (auto i : cache)
        {
            if (! i->busy && channels <= i->data.getNumChannels() && samples <= i->data.getNumSamples())
            {
                i->busy = true;
                i->chans = channels;
                i->samps = samples;
                return i;
            }
        }

        // Then just find a free one
        for (auto i : cache)
        {
            if (! i->busy)
            {
                i->busy = true;
                return i;
            }
        }

        return {};
    }

    juce::CriticalSection lock;
    juce::OwnedArray<BufferCacheItem> cache;
};

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

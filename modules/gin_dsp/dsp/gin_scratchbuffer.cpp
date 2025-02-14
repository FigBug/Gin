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
    int refCount = 0;
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
        i->refCount = 1;

        juce::ScopedLock sl (lock);
        cache.add (i);
        return i;
    }

    void free (BufferCacheItem& i)
    {
        juce::ScopedLock sl (lock);
        i.refCount--;
    }

    void incRef (BufferCacheItem& i)
    {
        juce::ScopedLock sl (lock);
        i.refCount++;
    }

    int getUsedBuffers()
    {
        juce::ScopedLock sl (lock);

        int count = 0;

        for (auto itm : cache)
            count += itm->refCount;

        return count;
    }

    JUCE_DECLARE_SINGLETON(BufferCache, false)

private:
    BufferCacheItem* find (int channels, int samples)
    {
        juce::ScopedLock sl (lock);

        // First look for one the correct size
        for (auto i : cache)
        {
            if (i->refCount == 0 && channels <= i->data.getNumChannels() && samples <= i->data.getNumSamples())
            {
                i->refCount = 1;
                i->chans = channels;
                i->samps = samples;
                return i;
            }
        }

        // Then just find a free one
        for (auto i : cache)
        {
            if (i->refCount == 0)
            {
                i->refCount = 1;
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

//==============================================================================

#if GIN_UNIT_TESTS

class ScratchBufferTests : public juce::UnitTest
{
public:
    ScratchBufferTests()
        : UnitTest ("ScratchBuffer class", "gin_dsp")
    {
    }

    void runTest() override
    {
        beginTest ("ref count 1");
        {
            auto buf = std::make_unique<ScratchBuffer> (2, 1024);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);
            buf = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("ref count 2");
        {
            auto buf1 = std::make_unique<ScratchBuffer> (2, 1024);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);

            auto buf2 = std::make_unique<ScratchBuffer> (*buf1);
            expect (BufferCache::getInstance()->getUsedBuffers() == 2);

            buf1 = nullptr;
            buf2 = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("ref count 3");
        {
            juce::AudioSampleBuffer source (2, 1024);

            auto buf1 = std::make_unique<ScratchBuffer> (source);
            expect (BufferCache::getInstance()->getUsedBuffers() == 1);

            auto buf2 = std::make_unique<ScratchBuffer> (source);
            expect (BufferCache::getInstance()->getUsedBuffers() == 2);

            buf1 = nullptr;
            buf2 = nullptr;
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }

        beginTest ("mono buffer");
        {
            juce::AudioSampleBuffer source (2, 1024);
            {
                auto mono = monoBuffer (source);
                expect (mono.getNumChannels() == 1);
                expect (BufferCache::getInstance()->getUsedBuffers() == 1);
            }
            expect (BufferCache::getInstance()->getUsedBuffers() == 0);
        }
    }
};

static ScratchBufferTests scratchBufferTests;

#endif

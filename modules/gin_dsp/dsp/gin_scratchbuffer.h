/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
*/

#pragma once

class BufferCacheItem;

/**
    Temporary audio buffer allocated from a shared pool to avoid allocations.

    ScratchBuffer provides a convenient way to get temporary audio buffers without
    allocating memory on the heap. Buffers are retrieved from a thread-safe pool
    and automatically returned when the ScratchBuffer goes out of scope.

    This is useful in real-time audio code where you need temporary buffers but
    want to avoid allocation overhead and fragmentation. The pool grows as needed
    but buffers are reused, minimizing allocations after initialization.

    Key Features:
    - Automatic memory management (RAII)
    - Thread-safe buffer pool
    - No allocations after warm-up
    - Copy constructor for buffer cloning
    - Inherits from AudioSampleBuffer

    Note: While buffer retrieval from the pool uses locking, this is generally
    acceptable as the lock contention is minimal and allocation is avoided.

    Usage:
    @code
    void processAudio(AudioBuffer<float>& buffer)
    {
        // Get a temporary buffer - no allocation if pool has one
        ScratchBuffer temp(buffer.getNumChannels(), buffer.getNumSamples());

        // Use it like a regular AudioBuffer
        temp.copyFrom(0, 0, buffer, 0, 0, buffer.getNumSamples());
        applyEffect(temp);
        buffer.copyFrom(0, 0, temp, 0, 0, temp.getNumSamples());

        // Buffer automatically returned to pool when temp goes out of scope
    }
    @endcode

    @see juce::AudioSampleBuffer
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

//==============================================================================
/** @cond INTERNAL */
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
/** @endcond */

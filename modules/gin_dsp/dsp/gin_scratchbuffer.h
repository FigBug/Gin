/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

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

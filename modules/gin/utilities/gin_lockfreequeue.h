/*==============================================================================

 Copyright 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** A lock-free FIFO queue for single-producer, single-consumer scenarios.

    This template class provides a thread-safe queue that can be used to pass
    objects between threads without using locks. It's built on top of JUCE's
    AbstractFifo and is suitable for real-time audio processing where blocking
    is not acceptable.

    The queue uses a circular buffer internally and supports write, read, peek,
    and pop operations. All operations are lock-free and safe for use between
    one producer thread and one consumer thread.

    Example usage:
    @code
    LockFreeQueue<int> queue (100);

    // Producer thread
    queue.write (42);
    queue.write (123);

    // Consumer thread
    auto value = queue.read();
    if (value.has_value())
        process (*value);
    @endcode

    @tparam T The type of objects to store in the queue. Must be copyable.
*/
template <typename T>
class LockFreeQueue
{
public:
    /** Creates a lock-free queue with the specified capacity.
        @param capacity The maximum number of items the queue can hold (default: 128)
    */
    LockFreeQueue (int capacity = 128)
      : fifo (capacity + 1)
    {
        storage.resize (capacity + 1);
    }

    /** Resizes the queue to a new capacity.
        Note: This is not thread-safe and should only be called when no other
        threads are accessing the queue.
        @param capacity The new maximum number of items the queue can hold
    */
    void setSize (int capacity)
    {
        fifo.setTotalSize (capacity + 1);
        storage.resize (capacity + 1);
    }

    /** Returns the number of free slots available for writing.
        @return The number of items that can be written before the queue is full
    */
    int getFreeSpace() const noexcept       { return fifo.getFreeSpace(); }

    /** Returns the number of items ready to be read.
        @return The number of items currently in the queue
    */
    int getNumReady() const noexcept        { return fifo.getNumReady(); }

    /** Resets the queue to an empty state.
        Note: This is not thread-safe and should only be called when no other
        threads are accessing the queue.
    */
    void reset() noexcept                   { fifo.reset(); }

    /** Writes an item to the queue.
        This should only be called from the producer thread.
        @param item The item to write to the queue
        @return true if the write succeeded, false if the queue was full
    */
    bool write (T item)
    {
        int start1, size1, start2, size2;
        fifo.prepareToWrite (1, start1, size1, start2, size2);

        if (size1 + size2 < 1)
            return false;

        storage.set (start1, item);

        fifo.finishedWrite (size1 + size2);
        return true;
    }

    /** Peeks at the next item in the queue without removing it.
        This should only be called from the consumer thread.
        @return An optional containing the next item, or std::nullopt if the queue is empty
    */
    std::optional<T> peek()
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (1, start1, size1, start2, size2);

        if ((size1 + size2) < 1)
            return {};

        return storage[start1];
    }

    /** Reads and removes the next item from the queue.
        This should only be called from the consumer thread.
        @return An optional containing the next item, or std::nullopt if the queue is empty
    */
    std::optional<T> read()
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (1, start1, size1, start2, size2);

        if ((size1 + size2) < 1)
            return {};

        auto v = storage[start1];

        fifo.finishedRead (size1 + size2);
        return v;
    }

    /** Removes (pops) multiple items from the queue without returning them.
        This is useful for discarding items you've already processed via peek().
        This should only be called from the consumer thread.
        @param num The number of items to remove from the queue
        @return true if the pop succeeded, false if there weren't enough items
    */
    bool pop (int num)
    {
        int start1, size1, start2, size2;
        fifo.prepareToRead (num, start1, size1, start2, size2);

        if ((size1 + size2) < num)
            return false;

        fifo.finishedRead (size1 + size2);
        return true;
    }

private:
    juce::AbstractFifo fifo;
    juce::Array<T> storage;

    JUCE_DECLARE_NON_COPYABLE (LockFreeQueue)
};

/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Creates a block of shared memory. The first one to create the block sets the size
    otherwise it is ignored. The block may be a different size than requested, especially
    if it was already created.

    On Unix based OSes, if a process using the block crashes, the block will leak. Clean
    it up later with remove() and it will be deleted when the last process stops using it.
    (No new processes will be able to attach)

    On Windows the memory block will always disappear when the final process closes the
    handle / crashes.
 */
class SharedMemory
{
public:
    /** Creates or opens a shared memory block.

        If a block with this name already exists, it will be opened and the size
        parameter is ignored. Otherwise, a new block is created with the specified size.
        The actual size may differ from the requested size, especially if the block
        already existed.

        @param name  The unique system-wide name for this shared memory block
        @param size  The desired size in bytes (only used when creating a new block)
    */
    SharedMemory (const juce::String& name, int size);

    /** Destructor. Detaches from the shared memory block.

        On Windows, if this is the last process using the block, it will be deleted.
        On Unix systems, the block will persist until explicitly removed with remove().
    */
    ~SharedMemory();

    /** Removes a shared memory block by name.

        On Unix systems, this marks the block for deletion. It will be deleted when
        the last process detaches. On Windows, this has no effect as blocks are
        automatically cleaned up.

        @param name  The name of the shared memory block to remove
    */
    static void remove (const juce::String& name);

    /** Returns a pointer to the shared memory data.

        This pointer can be used to read from or write to the shared memory.
        All processes sharing this memory block will see the same data.

        @returns  Pointer to the shared memory, or nullptr if the block is invalid
    */
    void* getData();

    /** Returns the actual size of the shared memory block in bytes.

        This may differ from the requested size, especially if the block already
        existed when this SharedMemory object was created.

        @returns  The size of the shared memory block in bytes
    */
    int getSize();

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SharedMemory)
};

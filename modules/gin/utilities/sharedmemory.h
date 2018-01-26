/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

/** Creates a block of shared memory. The first one to create the block sets the size
    otherwise it is ignored. The block may be a different size than requested, especially
    if it was already created.
 
    On Unix based OSes, if a process using the block crashes, the block will leak. Clean
    it up later with remove() and it will be deleted when the last process stops using it.
    (No new processes will be able to attach
 */
class SharedMemory
{
public:
    SharedMemory (const String& name, int size);
    ~SharedMemory();
    
    static void remove (const String& name);
    
    void* getData();
    int getSize();
    
private:
    class Impl;
    ScopedPointer<Impl> impl;
};

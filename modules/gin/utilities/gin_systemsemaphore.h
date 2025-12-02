/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** A system-wide semaphore for inter-process synchronization.

    This class provides a named semaphore that can be shared between multiple
    processes on the same system. Unlike regular threading primitives, this
    semaphore persists at the OS level and can be accessed by any process
    that knows its name.

    @warning This class is not yet fully implemented and should be used with caution.

    @see lock, unlock, isValid
*/
class SystemSemaphore
{
public:
    /** Creates or opens a system-wide semaphore with the given name.

        If a semaphore with this name already exists, it will be opened.
        Otherwise, a new semaphore will be created.

        @param name  The unique name identifying this semaphore across the system
    */
    SystemSemaphore (const juce::String& name);

    /** Destructor. Releases the semaphore. */
    ~SystemSemaphore();

    /** Locks the semaphore, blocking until it becomes available.

        If another process has locked this semaphore, this call will block
        until that process unlocks it.

        @returns  true if the lock was acquired successfully, false on error
    */
    bool lock();

    /** Unlocks the semaphore, making it available to other processes.

        @returns  true if the unlock succeeded, false on error
    */
    bool unlock();

    /** Checks if the semaphore was created/opened successfully.

        @returns  true if the semaphore is valid and can be used, false otherwise
    */
    bool isValid();

private:
    class Impl;
    std::unique_ptr<Impl> impl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemSemaphore)
};

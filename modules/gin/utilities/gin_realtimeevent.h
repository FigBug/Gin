#pragma once

//==============================================================================
/**
    A realtime-safe event synchronization primitive for cross-thread signaling.

    RealtimeEvent provides a lightweight signaling mechanism where signal() is
    guaranteed to be realtime-safe (no allocations, no blocking). This is suitable
    for waking up non-realtime threads from audio callbacks or other time-critical
    contexts.

    Key Features:
    - signal() is realtime-safe (can be called from audio thread)
    - wait() blocks the calling thread until signal() is called
    - Platform-optimized implementations (Windows Events, POSIX semaphores)
    - No memory allocations after construction
    - No mutexes or condition variables

    Usage:
    @code
    RealtimeEvent event;

    // Non-realtime thread
    std::thread worker([&event]() {
        while (running) {
            event.wait();  // Block until signaled
            // ... do work ...
        }
    });

    // Realtime thread (audio callback)
    void audioCallback() {
        // ... process audio ...
        event.signal();  // Wake up worker (realtime-safe)
    }
    @endcode

    Platform Notes:
    - Windows: Uses Win32 Event objects (SetEvent/WaitForSingleObject)
    - macOS/iOS: Uses POSIX unnamed semaphores via dispatch_semaphore
    - Linux: Uses POSIX semaphores (sem_post/sem_wait)

    @see RealtimeAsyncUpdater
*/
class RealtimeEvent
{
public:
    RealtimeEvent();
    ~RealtimeEvent();

    /** Signals the event, waking up any thread blocked in wait().
        This method is realtime-safe and can be called from audio threads.
        Multiple calls to signal() before wait() will only wake once.
    */
    void signal();

    /** Blocks the calling thread until signal() is called.
        This method is NOT realtime-safe and should only be called from
        non-realtime threads.
    */
    void wait();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtimeEvent)
};

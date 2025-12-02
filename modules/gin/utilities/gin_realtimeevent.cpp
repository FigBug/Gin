//==============================================================================
#if JUCE_WINDOWS
 #include <windows.h>
#elif JUCE_MAC || JUCE_IOS
 #include <dispatch/dispatch.h>
#else
 #include <semaphore.h>
 #include <errno.h>
#endif

//==============================================================================
struct RealtimeEvent::Impl
{
   #if JUCE_WINDOWS
    void* event = nullptr;  // HANDLE
   #elif JUCE_MAC || JUCE_IOS
    void* semaphore = nullptr;  // dispatch_semaphore_t
   #else
    struct SemaphoreStorage
    {
        alignas(64) char data[32];  // sem_t storage
    };
    SemaphoreStorage sem;
   #endif

    Impl()
    {
       #if JUCE_WINDOWS
        // Create auto-reset event (resets automatically after WaitForSingleObject)
        event = CreateEventW (nullptr, FALSE, FALSE, nullptr);
        jassert (event != nullptr);
       #elif JUCE_MAC || JUCE_IOS
        // Create dispatch semaphore with initial value 0
        semaphore = dispatch_semaphore_create (0);
        jassert (semaphore != nullptr);
       #else
        // Initialize POSIX unnamed semaphore with initial value 0
        auto* s = reinterpret_cast<sem_t*> (&sem);
        int result = sem_init (s, 0, 0);
        jassert (result == 0);
        juce::ignoreUnused (result);
       #endif
    }

    ~Impl()
    {
       #if JUCE_WINDOWS
        if (event != nullptr)
            CloseHandle (event);
       #elif JUCE_MAC || JUCE_IOS
        if (semaphore != nullptr)
            dispatch_release ((dispatch_semaphore_t) semaphore);
       #else
        auto* s = reinterpret_cast<sem_t*> (&sem);
        sem_destroy (s);
       #endif
    }

    void signal()
    {
       #if JUCE_WINDOWS
        // SetEvent is realtime-safe (no allocation, just sets a flag)
        SetEvent (event);
       #elif JUCE_MAC || JUCE_IOS
        // dispatch_semaphore_signal is realtime-safe
        dispatch_semaphore_signal ((dispatch_semaphore_t) semaphore);
       #else
        // sem_post is async-signal-safe and realtime-safe
        auto* s = reinterpret_cast<sem_t*> (&sem);
        sem_post (s);
       #endif
    }

    void wait()
    {
       #if JUCE_WINDOWS
        // Wait indefinitely for the event to be signaled
        WaitForSingleObject (event, INFINITE);
       #elif JUCE_MAC || JUCE_IOS
        // Wait indefinitely for semaphore
        dispatch_semaphore_wait ((dispatch_semaphore_t) semaphore, DISPATCH_TIME_FOREVER);
       #else
        // Wait for semaphore, handling interruptions
        auto* s = reinterpret_cast<sem_t*> (&sem);
        while (sem_wait (s) == -1)
        {
            if (errno != EINTR)
                break;
        }
       #endif
    }
};

//==============================================================================
RealtimeEvent::RealtimeEvent()
    : pimpl (std::make_unique<Impl>())
{
}

RealtimeEvent::~RealtimeEvent() = default;

void RealtimeEvent::signal()
{
    pimpl->signal();
}

void RealtimeEvent::wait()
{
    pimpl->wait();
}

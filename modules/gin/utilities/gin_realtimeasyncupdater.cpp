/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
class RealtimeAsyncUpdater::Impl : public juce::Thread
{
public:
    Impl() : Thread ("RtAsyncUpdater")
    {
        // Some hosts (e.g. FL Studio) call exit() with plugins still loaded. Static
        // destruction then races this thread's message queue posts, which aborts.
        // atexit handlers run before those destructors, so use one to stop posting.
        static const bool atExitRegistered = []
        {
            std::atexit ([] { processIsExiting().store (true, std::memory_order_release); });
            return true;
        }();
        juce::ignoreUnused (atExitRegistered);

        startThread();
    }
    
    ~Impl() override
    {
        signalThreadShouldExit ();
        event.signal();
        stopThread (1000);
    }
    
    void registerUpdater (RealtimeAsyncUpdater* ras)
    {
        juce::ScopedLock sl (lock);
        updaters.add (ras);
    }
    
    void unregisterUpdater (RealtimeAsyncUpdater* ras)
    {
        juce::ScopedLock sl (lock);
        updaters.removeFirstMatchingValue (ras);
    }
    
    void signal()
    {
        event.signal();
    }
    
private:
    juce::CriticalSection lock;
    juce::Array<RealtimeAsyncUpdater*> updaters;
    RealtimeEvent event;
    
    static std::atomic<bool>& processIsExiting()
    {
        static std::atomic<bool> exiting { false };
        return exiting;
    }

    void run() override
    {
        while (! threadShouldExit())
        {
            event.wait();

            if (threadShouldExit() || processIsExiting().load (std::memory_order_acquire))
                break;

            if (auto* mm = juce::MessageManager::getInstanceWithoutCreating())
            {
                juce::WeakReference<Impl> weakSelf = this;
                mm->callAsync ([this, weakSelf] {
                    if (weakSelf != nullptr)
                        fireCallbacks();
                });
            }
        }
    }

    void fireCallbacks()
    {
        juce::ScopedLock sl (lock);
        for (auto au : updaters)
            if (au->triggered.exchange (false, std::memory_order_relaxed))
                au->handleAsyncUpdate();
    }
    
    JUCE_DECLARE_WEAK_REFERENCEABLE (Impl)
};

//==============================================================================
RealtimeAsyncUpdater::RealtimeAsyncUpdater()
{
    impl->registerUpdater (this);
}

RealtimeAsyncUpdater::~RealtimeAsyncUpdater()
{
    impl->unregisterUpdater (this);
}

void RealtimeAsyncUpdater::triggerAsyncUpdate()
{
    if (! triggered.exchange (true, std::memory_order_relaxed))
        impl->signal();
}

void RealtimeAsyncUpdater::cancelPendingUpdate() noexcept
{
    triggered.store (false, std::memory_order_relaxed);
}

void RealtimeAsyncUpdater::handleUpdateNowIfNeeded()
{
    if (triggered.exchange (false, std::memory_order_relaxed))
        handleAsyncUpdate();
}

bool RealtimeAsyncUpdater::isUpdatePending() const noexcept
{
    return triggered.load (std::memory_order_relaxed);
}

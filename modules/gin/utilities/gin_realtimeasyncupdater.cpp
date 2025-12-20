/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
class RealtimeAsyncUpdater::Impl : public juce::Thread
{
public:
    Impl() : Thread ("RealtimeAsyncUpdater")
    {
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
    
    void run() override
    {
        while (true)
        {
            event.wait();
            
            if (threadShouldExit())
                break;
            
            juce::WeakReference<Impl> weakSelf = this;
            juce::MessageManager::getInstance()->callAsync ([this, weakSelf] {
                if (weakSelf != nullptr)
                    fireCallbacks();
            });
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

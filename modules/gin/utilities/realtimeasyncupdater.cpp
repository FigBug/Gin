/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
class RealtimeAsyncUpdater::Impl : public Thread
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
        ScopedLock sl (lock);
        updaters.add (ras);
    }
    
    void unregisterUpdater (RealtimeAsyncUpdater* ras)
    {
        ScopedLock sl (lock);
        updaters.removeFirstMatchingValue (ras);
    }
    
    void signal()
    {
        static PerformanceCounter counter {"signal", 1000};
        counter.start();
        event.signal();
        counter.stop();
    }
    
private:
    CriticalSection lock;
    Array<RealtimeAsyncUpdater*> updaters;
    WaitableEvent event;
    
    void run() override
    {
        while (true)
        {
            event.wait (-1);
            
            if (threadShouldExit())
                break;
            
            WeakReference<Impl> weakSelf = this;
            MessageManager::getInstance()->callAsync ([this, weakSelf] {
                if (weakSelf != nullptr)
                    fireCallbacks();
            });
        }
    }

    void fireCallbacks()
    {
        ScopedLock sl (lock);
        for (int i = updaters.size(); --i >= 0;)
        {
            auto au = updaters[i];
            if (au->triggered.get())
            {
                au->triggered = false;
                au->handleAsyncUpdate();
            }
        }
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
    triggered = true;
    impl->signal();
}

void RealtimeAsyncUpdater::cancelPendingUpdate() noexcept
{
    triggered = false;
}

void RealtimeAsyncUpdater::handleUpdateNowIfNeeded()
{
    if (triggered.get())
    {
        triggered = false;
        handleAsyncUpdate();
    }
}

bool RealtimeAsyncUpdater::isUpdatePending() const noexcept
{
    return triggered.get();
}

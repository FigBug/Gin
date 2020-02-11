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
        next = 0;
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
    
    void signal (RealtimeAsyncUpdater& ras)
    {
        static PerformanceCounter counter {"signal", 1000};
        counter.start();
        ras.order = ++next;
        event.signal();
        counter.stop();
    }
    
private:
    CriticalSection lock;
    Array<RealtimeAsyncUpdater*> updaters;
    WaitableEvent event;
    Atomic<uint32_t> next;
    
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
        Array<RealtimeAsyncUpdater*> dirtyUpdaters;
        
        ScopedLock sl (lock);
        for (auto au : updaters)
            if (au->triggered.get())
                dirtyUpdaters.add (au);
        
        std::sort (dirtyUpdaters.begin(), dirtyUpdaters.end(),
                   [] (const RealtimeAsyncUpdater* a, const RealtimeAsyncUpdater* b) -> bool
                   {
                       return a->order.get() < b->order.get();
                   });
                
        for (auto au : dirtyUpdaters)
        {
            au->triggered = false;
            au->handleAsyncUpdate();
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
    if (! triggered.get())
    {
        triggered = true;
        impl->signal (*this);
    }
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

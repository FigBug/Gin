/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class RealtimeAsyncUpdater
{
public:
    RealtimeAsyncUpdater();

    virtual ~RealtimeAsyncUpdater();

    void triggerAsyncUpdate();

    void cancelPendingUpdate() noexcept;

    void handleUpdateNowIfNeeded();

    bool isUpdatePending() const noexcept;

    virtual void handleAsyncUpdate() = 0;

private:
    //==============================================================================
    class Impl;
    SharedResourcePointer<Impl> impl;
    
    Atomic<bool> triggered;
    Atomic<uint32_t> order;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtimeAsyncUpdater)
};

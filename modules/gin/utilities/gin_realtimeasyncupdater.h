/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** An asynchronous update dispatcher that bypasses the message queue.

    Similar to juce::AsyncUpdater, but uses a dedicated background thread instead
    of the message queue. This makes it suitable for real-time applications where
    the message queue might introduce unacceptable latency or where the message
    thread might be blocked.

    Multiple calls to triggerAsyncUpdate() are coalesced - if called multiple times
    before handleAsyncUpdate() runs, it will only be called once.

    @see triggerAsyncUpdate, handleAsyncUpdate, cancelPendingUpdate
*/
class RealtimeAsyncUpdater
{
public:
    /** Creates a RealtimeAsyncUpdater. */
    RealtimeAsyncUpdater();

    /** Destructor. Cancels any pending updates. */
    virtual ~RealtimeAsyncUpdater();

    /** Triggers an asynchronous update.

        Schedules handleAsyncUpdate() to be called on the message thread.
        Multiple calls before the update executes are coalesced into a single callback.
        This method is thread-safe and can be called from any thread, including
        real-time audio threads.
    */
    void triggerAsyncUpdate();

    /** Cancels any pending update.

        Prevents handleAsyncUpdate() from being called if it hasn't executed yet.
        This is thread-safe.
    */
    void cancelPendingUpdate() noexcept;

    /** Handles the update immediately if one is pending.

        If an update has been triggered but not yet executed, this will execute
        it immediately on the current thread and cancel the pending background execution.
    */
    void handleUpdateNowIfNeeded();

    /** Checks if an update is currently pending.

        @returns  true if triggerAsyncUpdate() has been called and handleAsyncUpdate()
                  hasn't executed yet
    */
    bool isUpdatePending() const noexcept;

    /** Called asynchronously when an update is triggered.

        Override this method to perform your update logic. This will be called
        on a dedicated background thread (not the message thread).
    */
    virtual void handleAsyncUpdate() = 0;

private:
    //==============================================================================
    class Impl;
    juce::SharedResourcePointer<Impl> impl;

    std::atomic<bool> triggered;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RealtimeAsyncUpdater)
};

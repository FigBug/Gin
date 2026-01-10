/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class RealtimeAsyncUpdaterTests : public juce::UnitTest
{
public:
    RealtimeAsyncUpdaterTests() : juce::UnitTest ("Realtime Async Updater", "gin") {}

    void runTest() override
    {
        testBasicTriggering();
        testMultipleTriggers();
        testCancelPending();
        testIsUpdatePending();
        testHandleUpdateNowIfNeeded();
        testMultipleUpdaters();
    }

private:
    class TestUpdater : public RealtimeAsyncUpdater
    {
    public:
        std::atomic<int> callCount {0};
        std::atomic<bool> wasHandled {false};

        void handleAsyncUpdate() override
        {
            callCount++;
            wasHandled = true;
        }

        void reset()
        {
            callCount = 0;
            wasHandled = false;
        }
    };

    void testBasicTriggering()
    {
        beginTest ("Realtime Async Updater - Basic Triggering");

        TestUpdater updater;

        expect (!updater.isUpdatePending(), "Should not be pending initially");

        updater.triggerAsyncUpdate();

        expect (updater.isUpdatePending(), "Should be pending after trigger");

        // Wait for async update to be processed
        int timeout = 0;
        while (!updater.wasHandled && timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (updater.wasHandled, "Should have been handled");
        expectEquals (updater.callCount.load(), 1, "Should have been called once");
    }

    void testMultipleTriggers()
    {
        beginTest ("Realtime Async Updater - Multiple Triggers Coalesce");

        TestUpdater updater;

        // Trigger multiple times rapidly
        updater.triggerAsyncUpdate();
        updater.triggerAsyncUpdate();
        updater.triggerAsyncUpdate();

        // Wait for processing
        int timeout = 0;
        while (!updater.wasHandled && timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        // Multiple triggers should coalesce into single call
        expectEquals (updater.callCount.load(), 1,
                     "Multiple triggers should coalesce into one call");
    }

    void testCancelPending()
    {
        beginTest ("Realtime Async Updater - Cancel Pending");

        TestUpdater updater;

        updater.triggerAsyncUpdate();
        expect (updater.isUpdatePending(), "Should be pending");

        updater.cancelPendingUpdate();
        expect (!updater.isUpdatePending(), "Should not be pending after cancel");

        // Wait to ensure it doesn't get called
        juce::Thread::sleep (100);

        expect (!updater.wasHandled, "Should not have been handled after cancel");
    }

    void testIsUpdatePending()
    {
        beginTest ("Realtime Async Updater - Is Update Pending");

        TestUpdater updater;

        expect (!updater.isUpdatePending(), "Should not be pending initially");

        updater.triggerAsyncUpdate();
        expect (updater.isUpdatePending(), "Should be pending after trigger");

        // Wait for processing
        int timeout = 0;
        while (updater.isUpdatePending() && timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (!updater.isUpdatePending(), "Should not be pending after handling");
    }

    void testHandleUpdateNowIfNeeded()
    {
        beginTest ("Realtime Async Updater - Handle Update Now If Needed");

        TestUpdater updater;

        updater.triggerAsyncUpdate();
        expect (updater.isUpdatePending(), "Should be pending");

        updater.handleUpdateNowIfNeeded();

        expect (!updater.isUpdatePending(), "Should not be pending after immediate handle");
        expectEquals (updater.callCount.load(), 1, "Should have been called");

        // Call again when nothing is pending
        updater.reset();
        updater.handleUpdateNowIfNeeded();

        expectEquals (updater.callCount.load(), 0,
                     "Should not call when nothing pending");
    }

    void testMultipleUpdaters()
    {
        beginTest ("Realtime Async Updater - Multiple Updaters");

        TestUpdater updater1;
        TestUpdater updater2;
        TestUpdater updater3;

        updater1.triggerAsyncUpdate();
        updater2.triggerAsyncUpdate();
        updater3.triggerAsyncUpdate();

        // Wait for all to be processed
        int timeout = 0;
        while ((!updater1.wasHandled || !updater2.wasHandled || !updater3.wasHandled) &&
               timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (updater1.wasHandled, "Updater 1 should be handled");
        expect (updater2.wasHandled, "Updater 2 should be handled");
        expect (updater3.wasHandled, "Updater 3 should be handled");
    }
};

static RealtimeAsyncUpdaterTests realtimeAsyncUpdaterTests;

#endif

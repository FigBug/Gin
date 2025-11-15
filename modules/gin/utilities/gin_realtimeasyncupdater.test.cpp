/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

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
        testOrdering();
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
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
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
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
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
        juce::MessageManager::getInstance()->runDispatchLoopUntil (100);

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
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
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
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (updater1.wasHandled, "Updater 1 should be handled");
        expect (updater2.wasHandled, "Updater 2 should be handled");
        expect (updater3.wasHandled, "Updater 3 should be handled");
    }

    void testOrdering()
    {
        beginTest ("Realtime Async Updater - Ordering");

        class OrderedUpdater : public RealtimeAsyncUpdater
        {
        public:
            juce::Array<int>* order;
            int id;

            OrderedUpdater (juce::Array<int>* o, int i) : order (o), id (i) {}

            void handleAsyncUpdate() override
            {
                order->add (id);
            }
        };

        juce::Array<int> order;

        OrderedUpdater updater1 (&order, 1);
        OrderedUpdater updater2 (&order, 2);
        OrderedUpdater updater3 (&order, 3);

        // Trigger in specific order
        updater1.triggerAsyncUpdate();
        juce::Thread::sleep (1); // Small delay to ensure different timestamps
        updater2.triggerAsyncUpdate();
        juce::Thread::sleep (1);
        updater3.triggerAsyncUpdate();

        // Wait for processing
        int timeout = 0;
        while (order.size() < 3 && timeout < 1000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expectEquals (order.size(), 3, "All three should have been called");

        if (order.size() == 3)
        {
            // They should be called in trigger order
            expectEquals (order[0], 1, "First should be updater 1");
            expectEquals (order[1], 2, "Second should be updater 2");
            expectEquals (order[2], 3, "Third should be updater 3");
        }
    }
};

static RealtimeAsyncUpdaterTests realtimeAsyncUpdaterTests;

#endif

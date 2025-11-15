/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

class ThreadingTests : public juce::UnitTest
{
public:
    ThreadingTests() : juce::UnitTest ("Threading Utilities", "gin") {}

    void runTest() override
    {
        testCallInBackground();
        testMultipleBackgroundCalls();
        testBackgroundCallCompletion();
    }

private:
    void testCallInBackground()
    {
        beginTest ("Call in Background - Basic");

        std::atomic<bool> executed {false};

        callInBackground ([&executed]()
        {
            executed = true;
        });

        // Wait for background task to complete
        int timeout = 0;
        while (!executed && timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (executed, "Background function should execute");
    }

    void testMultipleBackgroundCalls()
    {
        beginTest ("Call in Background - Multiple Calls");

        std::atomic<int> counter {0};

        for (int i = 0; i < 5; i++)
        {
            callInBackground ([&counter]()
            {
                counter++;
            });
        }

        // Wait for all background tasks to complete
        int timeout = 0;
        while (counter < 5 && timeout < 2000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expectEquals (counter.load(), 5, "All background functions should execute");
    }

    void testBackgroundCallCompletion()
    {
        beginTest ("Call in Background - Completion");

        std::atomic<bool> started {false};
        std::atomic<bool> completed {false};

        callInBackground ([&started, &completed]()
        {
            started = true;
            juce::Thread::sleep (50); // Simulate some work
            completed = true;
        });

        // Wait for task to start
        int timeout = 0;
        while (!started && timeout < 500)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (started, "Background task should start");

        // Wait for task to complete
        timeout = 0;
        while (!completed && timeout < 1000)
        {
            juce::Thread::sleep (10);
            timeout++;
        }

        expect (completed, "Background task should complete");
    }
};

static ThreadingTests threadingTests;

#endif

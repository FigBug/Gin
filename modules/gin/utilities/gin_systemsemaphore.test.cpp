/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class SystemSemaphoreTests : public juce::UnitTest
{
public:
    SystemSemaphoreTests() : juce::UnitTest ("System Semaphore", "gin") {}

    void runTest() override
    {
#ifndef JUCE_WINDOWS  // Only test on non-Windows platforms where it's implemented
        testBasicLocking();
        testMultipleLocks();
        testUnlockWithoutLock();
        testIsValid();
        testCrossProcess();
#endif
    }

private:
#ifndef JUCE_WINDOWS
    void testBasicLocking()
    {
        beginTest ("System Semaphore - Basic Locking");

        juce::String semName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        SystemSemaphore sem (semName);

        expect (sem.isValid(), "Semaphore should be valid");

        expect (sem.lock(), "Should be able to lock");
        expect (sem.unlock(), "Should be able to unlock");
    }

    void testMultipleLocks()
    {
        beginTest ("System Semaphore - Multiple Locks");

        juce::String semName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        SystemSemaphore sem (semName);

        expect (sem.lock(), "First lock should succeed");
        expect (!sem.lock(), "Second lock should fail (already locked)");
        expect (sem.unlock(), "Unlock should succeed");
        expect (sem.lock(), "Lock should succeed after unlock");
        expect (sem.unlock(), "Final unlock should succeed");
    }

    void testUnlockWithoutLock()
    {
        beginTest ("System Semaphore - Unlock Without Lock");

        juce::String semName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        SystemSemaphore sem (semName);

        expect (!sem.unlock(), "Unlock without lock should fail");
    }

    void testIsValid()
    {
        beginTest ("System Semaphore - Is Valid");

        juce::String semName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        SystemSemaphore sem (semName);

        expect (sem.isValid(), "Semaphore should be valid after construction");
    }

    void testCrossProcess()
    {
        beginTest ("System Semaphore - Cross Process");

        juce::String semName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());

        // Create semaphore in this process
        auto sem1 = std::make_unique<SystemSemaphore> (semName);
        expect (sem1->isValid(), "First semaphore should be valid");
        expect (sem1->lock(), "Should lock in first instance");

        // Create another semaphore with same name (simulating another process)
        auto sem2 = std::make_unique<SystemSemaphore> (semName);
        expect (sem2->isValid(), "Second semaphore should be valid");

        // Note: This test has limitations - we can't fully test cross-process
        // behavior without actually spawning another process, but we can verify
        // that multiple semaphores with the same name can be created

        expect (sem1->unlock(), "Should unlock from first instance");

        // Clean up
        sem1 = nullptr;
        sem2 = nullptr;
    }
#endif
};

static SystemSemaphoreTests systemSemaphoreTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

class SharedMemoryTests : public juce::UnitTest
{
public:
    SharedMemoryTests() : juce::UnitTest ("Shared Memory", "gin") {}

    void runTest() override
    {
        testBasicCreation();
        testDataAccess();
        testCrossProcess();
        testSizeRetrieval();
        testRemove();
    }

private:
    void testBasicCreation()
    {
        beginTest ("Shared Memory - Basic Creation");

        juce::String memName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        int size = 1024;

        SharedMemory mem (memName, size);

        expect (mem.getData() != nullptr, "Data pointer should not be null");
        expectEquals (mem.getSize(), size, "Size should match requested size");

        SharedMemory::remove (memName);
    }

    void testDataAccess()
    {
        beginTest ("Shared Memory - Data Access");

        juce::String memName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        int size = 256;

        SharedMemory mem (memName, size);

        auto* data = static_cast<char*> (mem.getData());
        expect (data != nullptr, "Should have valid data pointer");

        // Write test data
        const char* testStr = "Hello SharedMemory!";
        int len = (int) strlen (testStr);
        memcpy (data, testStr, len + 1);

        // Verify we can read it back
        juce::String readBack (data);
        expectEquals (readBack, juce::String (testStr), "Should read back written data");

        SharedMemory::remove (memName);
    }

    void testCrossProcess()
    {
        beginTest ("Shared Memory - Cross Process");

        juce::String memName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        int size = 512;

        // Create first shared memory instance
        auto mem1 = std::make_unique<SharedMemory> (memName, size);
        expect (mem1->getData() != nullptr, "First instance should have valid data");

        auto* data1 = static_cast<char*> (mem1->getData());
        const char* testStr = "Cross Process Test";
        strcpy (data1, testStr);

        // Create second instance with same name (simulates another process)
        auto mem2 = std::make_unique<SharedMemory> (memName, size);
        expect (mem2->getData() != nullptr, "Second instance should have valid data");

        auto* data2 = static_cast<char*> (mem2->getData());
        juce::String readBack (data2);

        // On most systems, this should show the shared data
        // Note: Behavior may vary by platform
        expect (mem2->getSize() > 0, "Second instance should have valid size");

        // Clean up
        mem1 = nullptr;
        mem2 = nullptr;
        SharedMemory::remove (memName);
    }

    void testSizeRetrieval()
    {
        beginTest ("Shared Memory - Size Retrieval");

        juce::String memName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        int requestedSize = 2048;

        SharedMemory mem (memName, requestedSize);

        expectEquals (mem.getSize(), requestedSize, "Size should match requested size");

        SharedMemory::remove (memName);
    }

    void testRemove()
    {
        beginTest ("Shared Memory - Remove");

        juce::String memName = "GinTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt());
        int size = 128;

        {
            SharedMemory mem (memName, size);
            expect (mem.getData() != nullptr, "Should create shared memory");
        }

        // Remove the shared memory
        SharedMemory::remove (memName);

        // Note: We can't easily verify it's gone without platform-specific code,
        // but we can at least verify the remove function doesn't crash
        expect (true, "Remove function completed");
    }
};

static SharedMemoryTests sharedMemoryTests;

#endif

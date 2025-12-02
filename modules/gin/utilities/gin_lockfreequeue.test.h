/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LockFreeQueueTests : public juce::UnitTest
{
public:
    LockFreeQueueTests() : juce::UnitTest ("Lock Free Queue", "gin") {}

    void runTest() override
    {
        testBasicWriteRead();
        testFreeSpaceAndReady();
        testCircularBuffer();
        testPeekOperation();
        testPopOperation();
        testResetOperation();
        testQueueFull();
        testQueueEmpty();
        testSetSize();
    }

private:
    void testBasicWriteRead()
    {
        beginTest ("Basic Write and Read");

        LockFreeQueue<int> queue (10);

        // Write some values
        expect (queue.write (42), "Write should succeed");
        expect (queue.write (123), "Write should succeed");
        expect (queue.write (999), "Write should succeed");

        expectEquals (queue.getNumReady(), 3, "Should have 3 items ready");

        // Read them back
        auto val1 = queue.read();
        expect (val1.has_value(), "Read should return a value");
        expectEquals (*val1, 42, "First value should be 42");

        auto val2 = queue.read();
        expect (val2.has_value(), "Read should return a value");
        expectEquals (*val2, 123, "Second value should be 123");

        auto val3 = queue.read();
        expect (val3.has_value(), "Read should return a value");
        expectEquals (*val3, 999, "Third value should be 999");

        expectEquals (queue.getNumReady(), 0, "Should have 0 items ready after reading all");
    }

    void testFreeSpaceAndReady()
    {
        beginTest ("Free Space and Ready Count");

        LockFreeQueue<int> queue (5);

        expectEquals (queue.getFreeSpace(), 5, "Initial free space should be capacity");
        expectEquals (queue.getNumReady(), 0, "Initially should have no data ready");

        queue.write (1);
        queue.write (2);
        queue.write (3);

        expectEquals (queue.getFreeSpace(), 2, "Free space should decrease");
        expectEquals (queue.getNumReady(), 3, "Ready count should increase");

        queue.read();

        expectEquals (queue.getFreeSpace(), 3, "Free space should increase after read");
        expectEquals (queue.getNumReady(), 2, "Ready count should decrease after read");
    }

    void testCircularBuffer()
    {
        beginTest ("Circular Buffer Behavior");

        LockFreeQueue<int> queue (4);

        // Write and read multiple times to wrap around
        for (int round = 0; round < 10; round++)
        {
            queue.write (round * 100);
            queue.write (round * 100 + 1);
            queue.write (round * 100 + 2);

            auto val1 = queue.read();
            auto val2 = queue.read();
            auto val3 = queue.read();

            expect (val1.has_value() && val2.has_value() && val3.has_value(), "All reads should succeed");
            expectEquals (*val1, round * 100, "First value should match");
            expectEquals (*val2, round * 100 + 1, "Second value should match");
            expectEquals (*val3, round * 100 + 2, "Third value should match");
        }

        expectEquals (queue.getNumReady(), 0, "Queue should be empty after all rounds");
    }

    void testPeekOperation()
    {
        beginTest ("Peek Operation");

        LockFreeQueue<int> queue (10);

        queue.write (777);
        queue.write (888);

        // Peek should not consume data
        auto peeked1 = queue.peek();
        expect (peeked1.has_value(), "Peek should return a value");
        expectEquals (*peeked1, 777, "Peeked value should be 777");
        expectEquals (queue.getNumReady(), 2, "Peek should not consume data");

        // Peek again - should return same value
        auto peeked2 = queue.peek();
        expect (peeked2.has_value(), "Peek should return a value");
        expectEquals (*peeked2, 777, "Peeked value should still be 777");

        // Now read - should get the peeked value
        auto val = queue.read();
        expect (val.has_value(), "Read should return a value");
        expectEquals (*val, 777, "Read value should match peeked value");
        expectEquals (queue.getNumReady(), 1, "Read should consume data");

        // Peek the next value
        auto peeked3 = queue.peek();
        expect (peeked3.has_value(), "Peek should return next value");
        expectEquals (*peeked3, 888, "Next peeked value should be 888");
    }

    void testPopOperation()
    {
        beginTest ("Pop Operation");

        LockFreeQueue<int> queue (10);

        queue.write (1);
        queue.write (2);
        queue.write (3);
        queue.write (4);
        queue.write (5);

        expectEquals (queue.getNumReady(), 5, "Should have 5 items");

        // Pop 2 items
        expect (queue.pop (2), "Pop should succeed");
        expectEquals (queue.getNumReady(), 3, "Should have 3 items after popping 2");

        // Next read should get the 3rd item
        auto val = queue.read();
        expect (val.has_value(), "Read should succeed");
        expectEquals (*val, 3, "Should read value that was 3rd in queue");

        // Pop remaining items
        expect (queue.pop (2), "Pop should succeed");
        expectEquals (queue.getNumReady(), 0, "Queue should be empty");
    }

    void testResetOperation()
    {
        beginTest ("Reset Operation");

        LockFreeQueue<int> queue (10);

        queue.write (1);
        queue.write (2);
        queue.write (3);

        expectEquals (queue.getNumReady(), 3, "Should have 3 items");

        queue.reset();

        expectEquals (queue.getNumReady(), 0, "Queue should be empty after reset");
        expectEquals (queue.getFreeSpace(), 10, "Free space should be full capacity after reset");

        // Should be able to write and read normally after reset
        expect (queue.write (99), "Write should succeed after reset");
        auto val = queue.read();
        expect (val.has_value(), "Read should succeed after reset");
        expectEquals (*val, 99, "Value should be correct after reset");
    }

    void testQueueFull()
    {
        beginTest ("Queue Full Behavior");

        LockFreeQueue<int> queue (3);

        // Fill the queue
        expect (queue.write (1), "First write should succeed");
        expect (queue.write (2), "Second write should succeed");
        expect (queue.write (3), "Third write should succeed");

        expectEquals (queue.getFreeSpace(), 0, "Queue should be full");
        expectEquals (queue.getNumReady(), 3, "Should have 3 items ready");

        // Try to write when full - should fail
        expect (!queue.write (4), "Write should fail when queue is full");

        // Read one item to make space
        queue.read();
        expectEquals (queue.getFreeSpace(), 1, "Should have space for 1 item");

        // Now write should succeed
        expect (queue.write (4), "Write should succeed after making space");
    }

    void testQueueEmpty()
    {
        beginTest ("Queue Empty Behavior");

        LockFreeQueue<int> queue (10);

        expectEquals (queue.getNumReady(), 0, "Queue should start empty");

        // Try to read from empty queue
        auto val1 = queue.read();
        expect (!val1.has_value(), "Read should return empty optional when queue is empty");

        // Try to peek from empty queue
        auto val2 = queue.peek();
        expect (!val2.has_value(), "Peek should return empty optional when queue is empty");

        // Try to pop from empty queue
        expect (!queue.pop (1), "Pop should fail when queue is empty");
    }

    void testSetSize()
    {
        beginTest ("Set Size Operation");

        LockFreeQueue<int> queue (5);

        expectEquals (queue.getFreeSpace(), 5, "Initial capacity should be 5");

        // Resize to larger
        queue.setSize (10);
        expectEquals (queue.getFreeSpace(), 10, "Capacity should be 10 after resize");

        // Write some data
        for (int i = 0; i < 8; i++)
            queue.write (i);

        expectEquals (queue.getNumReady(), 8, "Should have 8 items");
        expectEquals (queue.getFreeSpace(), 2, "Should have 2 free");

        // Verify data is correct
        for (int i = 0; i < 8; i++)
        {
            auto val = queue.read();
            expect (val.has_value(), "Read should succeed");
            expectEquals (*val, i, "Value should match");
        }
    }
};

static LockFreeQueueTests lockFreeQueueTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LRUCacheTests : public juce::UnitTest
{
public:
    LRUCacheTests() : juce::UnitTest ("LRU Cache", "gin") {}

    void runTest() override
    {
        testBasicPutGet();
        testEviction();
        testAccessOrderUpdate();
        testGetOrCreate();
        testRemove();
        testContains();
        testClear();
        testUpdateExisting();
    }

private:
    void testBasicPutGet()
    {
        beginTest ("Basic Put and Get");

        LRUCache<juce::String, int, 10> cache;

        cache.put ("one", 1);
        cache.put ("two", 2);
        cache.put ("three", 3);

        expectEquals (cache.size(), size_t (3), "Cache should have 3 items");

        auto* val1 = cache.get ("one");
        expect (val1 != nullptr, "Should find 'one'");
        expectEquals (*val1, 1, "Value should be 1");

        auto* val2 = cache.get ("two");
        expect (val2 != nullptr, "Should find 'two'");
        expectEquals (*val2, 2, "Value should be 2");

        auto* val3 = cache.get ("three");
        expect (val3 != nullptr, "Should find 'three'");
        expectEquals (*val3, 3, "Value should be 3");

        auto* val4 = cache.get ("four");
        expect (val4 == nullptr, "Should not find 'four'");
    }

    void testEviction()
    {
        beginTest ("LRU Eviction");

        LRUCache<int, juce::String, 3> cache;

        cache.put (1, "one");
        cache.put (2, "two");
        cache.put (3, "three");

        expectEquals (cache.size(), size_t (3), "Cache should be at capacity");

        // Adding a 4th item should evict the LRU item (1)
        cache.put (4, "four");

        expectEquals (cache.size(), size_t (3), "Cache should still be at capacity");
        expect (cache.get (1) == nullptr, "Item 1 should have been evicted");
        expect (cache.get (2) != nullptr, "Item 2 should still exist");
        expect (cache.get (3) != nullptr, "Item 3 should still exist");
        expect (cache.get (4) != nullptr, "Item 4 should exist");
    }

    void testAccessOrderUpdate()
    {
        beginTest ("Access Order Update");

        LRUCache<int, juce::String, 3> cache;

        cache.put (1, "one");
        cache.put (2, "two");
        cache.put (3, "three");

        // Access item 1, making it most recently used
        cache.get (1);

        // Add item 4 - should evict item 2 (now LRU)
        cache.put (4, "four");

        expect (cache.get (1) != nullptr, "Item 1 should still exist (was accessed)");
        expect (cache.get (2) == nullptr, "Item 2 should have been evicted");
        expect (cache.get (3) != nullptr, "Item 3 should still exist");
        expect (cache.get (4) != nullptr, "Item 4 should exist");
    }

    void testGetOrCreate()
    {
        beginTest ("Get Or Create");

        LRUCache<juce::String, int, 10> cache;

        int factoryCalls = 0;
        auto factory = [&factoryCalls]() { factoryCalls++; return 42; };

        // First call should invoke factory
        auto& val1 = cache.getOrCreate ("key", factory);
        expectEquals (val1, 42, "Value should be 42");
        expectEquals (factoryCalls, 1, "Factory should be called once");

        // Second call should return cached value without calling factory
        auto& val2 = cache.getOrCreate ("key", factory);
        expectEquals (val2, 42, "Value should still be 42");
        expectEquals (factoryCalls, 1, "Factory should not be called again");

        // Different key should call factory again
        auto& val3 = cache.getOrCreate ("other", factory);
        expectEquals (val3, 42, "Value should be 42");
        expectEquals (factoryCalls, 2, "Factory should be called for new key");
    }

    void testRemove()
    {
        beginTest ("Remove");

        LRUCache<juce::String, int, 10> cache;

        cache.put ("one", 1);
        cache.put ("two", 2);
        cache.put ("three", 3);

        expectEquals (cache.size(), size_t (3), "Cache should have 3 items");

        expect (cache.remove ("two"), "Remove should return true for existing key");
        expectEquals (cache.size(), size_t (2), "Cache should have 2 items");
        expect (cache.get ("two") == nullptr, "Removed item should not be found");

        expect (! cache.remove ("two"), "Remove should return false for missing key");
        expect (! cache.remove ("nonexistent"), "Remove should return false for nonexistent key");

        expect (cache.get ("one") != nullptr, "Other items should still exist");
        expect (cache.get ("three") != nullptr, "Other items should still exist");
    }

    void testContains()
    {
        beginTest ("Contains");

        LRUCache<juce::String, int, 10> cache;

        cache.put ("one", 1);
        cache.put ("two", 2);

        expect (cache.contains ("one"), "Should contain 'one'");
        expect (cache.contains ("two"), "Should contain 'two'");
        expect (! cache.contains ("three"), "Should not contain 'three'");
    }

    void testClear()
    {
        beginTest ("Clear");

        LRUCache<juce::String, int, 10> cache;

        cache.put ("one", 1);
        cache.put ("two", 2);
        cache.put ("three", 3);

        expectEquals (cache.size(), size_t (3), "Cache should have 3 items");
        expect (! cache.empty(), "Cache should not be empty");

        cache.clear();

        expectEquals (cache.size(), size_t (0), "Cache should be empty after clear");
        expect (cache.empty(), "Cache should be empty");
        expect (cache.get ("one") == nullptr, "Should not find items after clear");

        // Should work normally after clear
        cache.put ("new", 99);
        expect (cache.get ("new") != nullptr, "Should work after clear");
        expectEquals (*cache.get ("new"), 99, "Value should be correct");
    }

    void testUpdateExisting()
    {
        beginTest ("Update Existing");

        LRUCache<juce::String, int, 10> cache;

        cache.put ("key", 1);
        expectEquals (*cache.get ("key"), 1, "Initial value should be 1");

        cache.put ("key", 2);
        expectEquals (*cache.get ("key"), 2, "Updated value should be 2");
        expectEquals (cache.size(), size_t (1), "Size should still be 1");
    }
};

static LRUCacheTests lruCacheTests;

#endif

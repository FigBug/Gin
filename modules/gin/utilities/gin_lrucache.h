/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** A simple LRU (Least Recently Used) cache with a fixed maximum size.

    This template class provides a cache that automatically evicts the least
    recently accessed items when it reaches capacity. It uses a hash map for
    O(1) average-case lookups and a linked list to track access order.

    The cache is not thread-safe. If you need thread safety, you should
    synchronize access externally.

    Example usage:
    @code
    LRUCache<juce::String, int, 10> cache;

    // Insert items
    cache.put ("one", 1);
    cache.put ("two", 2);

    // Retrieve items
    if (auto* value = cache.get ("one"))
        process (*value);

    // Use getOrCreate for lazy initialization
    auto& value = cache.getOrCreate ("three", [] { return expensiveComputation(); });
    @endcode

    @tparam Key    The type used as the cache key. Must be hashable via std::hash.
    @tparam Value  The type of values stored in the cache.
    @tparam MaxSize The maximum number of items the cache can hold.
*/
template <typename Key, typename Value, size_t MaxSize>
class LRUCache
{
public:
    /** Creates an empty LRU cache. */
    LRUCache() = default;

    /** Returns a pointer to the cached value for the given key, or nullptr if not found.
        Accessing an item moves it to the front of the cache (most recently used).
        @param key The key to look up
        @return A pointer to the value if found, nullptr otherwise
    */
    Value* get (const Key& key)
    {
        auto it = map.find (key);
        if (it == map.end())
            return nullptr;

        // Move to front (most recently used)
        accessOrder.splice (accessOrder.begin(), accessOrder, it->second.listIterator);
        return &it->second.value;
    }

    /** Returns a const pointer to the cached value for the given key, or nullptr if not found.
        Accessing an item moves it to the front of the cache (most recently used).
        @param key The key to look up
        @return A const pointer to the value if found, nullptr otherwise
    */
    const Value* get (const Key& key) const
    {
        return const_cast<LRUCache*> (this)->get (key);
    }

    /** Inserts or updates a value in the cache.
        If the key already exists, its value is updated and it's moved to the front.
        If the cache is at capacity, the least recently used item is evicted.
        @param key   The key to insert or update
        @param value The value to store
    */
    void put (const Key& key, Value value)
    {
        auto it = map.find (key);
        if (it != map.end())
        {
            // Update existing entry and move to front
            it->second.value = std::move (value);
            accessOrder.splice (accessOrder.begin(), accessOrder, it->second.listIterator);
            return;
        }

        // Evict if at capacity
        while (map.size() >= MaxSize)
            evictLRU();

        // Insert new entry at front
        accessOrder.push_front (key);
        map.emplace (key, Entry { std::move (value), accessOrder.begin() });
    }

    /** Gets an existing value or creates a new one using the provided factory function.
        If the key exists, returns a reference to its value (moving it to front).
        If the key doesn't exist, calls the factory function to create a value,
        inserts it, and returns a reference to it.
        @param key     The key to look up or create
        @param factory A callable that takes no arguments and returns a Value
        @return A reference to the cached value
    */
    template <typename Factory>
    Value& getOrCreate (const Key& key, Factory&& factory)
    {
        if (auto* existing = get (key))
            return *existing;

        put (key, factory());
        return map.find (key)->second.value;
    }

    /** Removes an item from the cache.
        @param key The key to remove
        @return true if the item was found and removed, false otherwise
    */
    bool remove (const Key& key)
    {
        auto it = map.find (key);
        if (it == map.end())
            return false;

        accessOrder.erase (it->second.listIterator);
        map.erase (it);
        return true;
    }

    /** Checks if a key exists in the cache.
        Note: This does NOT update the access order.
        @param key The key to check
        @return true if the key exists, false otherwise
    */
    bool contains (const Key& key) const
    {
        return map.find (key) != map.end();
    }

    /** Returns the number of items currently in the cache.
        @return The current size of the cache
    */
    size_t size() const noexcept { return map.size(); }

    /** Checks if the cache is empty.
        @return true if the cache contains no items
    */
    bool empty() const noexcept { return map.empty(); }

    /** Removes all items from the cache. */
    void clear()
    {
        map.clear();
        accessOrder.clear();
    }

    /** Returns the maximum number of items the cache can hold.
        @return The maximum capacity
    */
    static constexpr size_t maxSize() noexcept { return MaxSize; }

private:
    void evictLRU()
    {
        if (accessOrder.empty())
            return;

        auto lruKey = accessOrder.back();
        accessOrder.pop_back();
        map.erase (lruKey);
    }

    struct Entry
    {
        Value value;
        typename std::list<Key>::iterator listIterator;
    };

    std::unordered_map<Key, Entry> map;
    std::list<Key> accessOrder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LRUCache)
};

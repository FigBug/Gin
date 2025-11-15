namespace Diff
{

std::vector<uint8_t> bsDiff (const juce::String& s1, const juce::String& s2)
{
    std::vector<uint8_t> result;

    uint32_t sz = uint32_t (s2.getNumBytesAsUTF8());
    result.insert (result.end(), (uint8_t*)&sz, (uint8_t*)&sz + sizeof(uint32_t));

    bsdiff_stream stream;

    stream.opaque = &result;
    stream.malloc = malloc;
    stream.free   = free;
    stream.write  = [] (struct bsdiff_stream* param, const void* buffer, int size)
    {
        auto& buf = *((std::vector<uint8_t>*)param->opaque);

        buf.insert (buf.end(), (uint8_t*)buffer, (uint8_t*)buffer + size);
        return 0;
    };

    [[maybe_unused]] auto err = bsdiff ((const uint8_t*)s1.toRawUTF8(), int64_t (s1.getNumBytesAsUTF8()), (const uint8_t*)s2.toRawUTF8(), int64_t (s2.getNumBytesAsUTF8()), &stream);
    jassert (err == 0);

    return result;
}

juce::String bsApplyPatch (const juce::String& s, const std::vector<uint8_t>& patch)
{
    uint32_t sz;
    memcpy (&sz, patch.data(), sizeof (sz));

    juce::MemoryBlock mb (sz + 1, true);

    juce::MemoryInputStream input (patch.data() + sizeof (uint32_t), patch.size() - 4, false);

    bspatch_stream stream;
    stream.opaque = &input;
    stream.read   = [] (const struct bspatch_stream* param, void* buffer, int length)
    {
        auto& is = *(juce::MemoryInputStream*)param->opaque;

        if (is.read (buffer, length) == length)
            return 0;

        return -1;
    };

    [[maybe_unused]] auto err = bspatch ((const uint8_t*)s.toRawUTF8(), int64_t (s.getNumBytesAsUTF8()), (uint8_t*)mb.getData(), sz, &stream);
    jassert (err == 0);

    return juce::String::fromUTF8 ((const char*)mb.getData());
}

}

//==============================================================================
#if JUCE_UNIT_TESTS

class DiffTests : public juce::UnitTest
{
public:
    DiffTests() : juce::UnitTest ("Diff Utilities", "gin") {}

    void runTest() override
    {
        testSimplePatch();
        testIdenticalStrings();
        testCompletelyDifferent();
        testInsertionDeletion();
        testRoundTrip();
    }

private:
    void testSimplePatch()
    {
        beginTest ("Simple Patch");

        juce::String original = "Hello World";
        juce::String modified = "Hello JUCE";

        auto patch = Diff::bsDiff (original, modified);
        expect (patch.size() > 0, "Patch should not be empty");

        juce::String result = Diff::bsApplyPatch (original, patch);
        expect (result == modified, "Patched string should match modified string");
    }

    void testIdenticalStrings()
    {
        beginTest ("Identical Strings");

        juce::String str = "Same text";

        auto patch = Diff::bsDiff (str, str);
        juce::String result = Diff::bsApplyPatch (str, patch);

        expect (result == str, "Patching identical string should return same string");
    }

    void testCompletelyDifferent()
    {
        beginTest ("Completely Different");

        juce::String s1 = "AAAAAAA";
        juce::String s2 = "BBBBBBB";

        auto patch = Diff::bsDiff (s1, s2);
        juce::String result = Diff::bsApplyPatch (s1, patch);

        expect (result == s2, "Should handle completely different strings");
    }

    void testInsertionDeletion()
    {
        beginTest ("Insertion and Deletion");

        // Test insertion
        juce::String original = "Hello";
        juce::String withInsertion = "Hello World!";

        auto insertPatch = Diff::bsDiff (original, withInsertion);
        juce::String insertResult = Diff::bsApplyPatch (original, insertPatch);
        expect (insertResult == withInsertion, "Should handle insertion");

        // Test deletion
        auto deletePatch = Diff::bsDiff (withInsertion, original);
        juce::String deleteResult = Diff::bsApplyPatch (withInsertion, deletePatch);
        expect (deleteResult == original, "Should handle deletion");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        juce::String v1 = "Version 1";
        juce::String v2 = "Version 2 with more text";
        juce::String v3 = "Version 3";

        // Create patches
        auto patch1to2 = Diff::bsDiff (v1, v2);
        auto patch2to3 = Diff::bsDiff (v2, v3);
        auto patch3to1 = Diff::bsDiff (v3, v1);

        // Apply patches
        juce::String result1 = Diff::bsApplyPatch (v1, patch1to2);
        expect (result1 == v2, "v1 -> v2 should work");

        juce::String result2 = Diff::bsApplyPatch (result1, patch2to3);
        expect (result2 == v3, "v2 -> v3 should work");

        juce::String result3 = Diff::bsApplyPatch (result2, patch3to1);
        expect (result3 == v1, "v3 -> v1 should complete the cycle");
    }
};

static DiffTests diffTests;

#endif

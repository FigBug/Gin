/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class DiffTests : public juce::UnitTest
{
public:
    DiffTests() : juce::UnitTest ("Binary Diff/Patch", "gin") {}

    void runTest() override
    {
        testIdenticalStrings();
        testSimpleChanges();
        testSimplePatch();
        testCompletelyDifferent();
        testEmptyStrings();
        testInsertionDeletion();
        testRoundTrip();
        testLargeChanges();
    }

private:
    void testIdenticalStrings()
    {
        beginTest ("Identical Strings");

        juce::String s1 = "Hello World";
        juce::String s2 = "Hello World";

        auto patch = Diff::bsDiff (s1, s2);
        auto result = Diff::bsApplyPatch (s1, patch);

        expectEquals (result, s2, "Patch should recreate identical string");
    }

    void testSimpleChanges()
    {
        beginTest ("Simple Changes");

        juce::String s1 = "Hello World";
        juce::String s2 = "Hello Earth";

        auto patch = Diff::bsDiff (s1, s2);
        auto result = Diff::bsApplyPatch (s1, patch);

        expectEquals (result, s2, "Patch should correctly apply simple changes");
    }

    void testCompletelyDifferent()
    {
        beginTest ("Completely Different");

        juce::String s1 = "The quick brown fox";
        juce::String s2 = "Lorem ipsum dolor";

        auto patch = Diff::bsDiff (s1, s2);
        auto result = Diff::bsApplyPatch (s1, patch);

        expectEquals (result, s2, "Patch should work with completely different strings");
    }

    void testEmptyStrings()
    {
        beginTest ("Empty Strings");

        // Empty to non-empty
        juce::String s1 = "";
        juce::String s2 = "Hello";

        auto patch1 = Diff::bsDiff (s1, s2);
        auto result1 = Diff::bsApplyPatch (s1, patch1);
        expectEquals (result1, s2, "Patch from empty to non-empty should work");

        // Non-empty to empty
        juce::String s3 = "Hello";
        juce::String s4 = "";

        auto patch2 = Diff::bsDiff (s3, s4);
        auto result2 = Diff::bsApplyPatch (s3, patch2);
        expectEquals (result2, s4, "Patch from non-empty to empty should work");

        // Empty to empty
        juce::String s5 = "";
        juce::String s6 = "";

        auto patch3 = Diff::bsDiff (s5, s6);
        auto result3 = Diff::bsApplyPatch (s5, patch3);
        expectEquals (result3, s6, "Patch from empty to empty should work");
    }

    void testLargeChanges()
    {
        beginTest ("Large Changes");

        // Build larger strings
        juce::String s1;
        juce::String s2;

        for (int i = 0; i < 100; i++)
        {
            s1 += "Line " + juce::String (i) + " of the original text. ";
            if (i % 2 == 0)
                s2 += "Line " + juce::String (i) + " of the modified text. ";
            else
                s2 += "Line " + juce::String (i) + " of the original text. ";
        }

        auto patch = Diff::bsDiff (s1, s2);
        auto result = Diff::bsApplyPatch (s1, patch);

        expectEquals (result, s2, "Patch should work with larger strings");
        expect (patch.size() > 0, "Patch size should be non-zero");
    }

    void testSimplePatch()
    {
        beginTest ("Simple Patch");

        juce::String original = "Hello World";
        juce::String modified = "Hello JUCE";

        auto patch = Diff::bsDiff (original, modified);
        expect (patch.size() > 0, "Patch should not be empty");

        juce::String result = Diff::bsApplyPatch (original, patch);
        expectEquals (result, modified, "Patched string should match modified string");
    }

    void testInsertionDeletion()
    {
        beginTest ("Insertion and Deletion");

        // Test insertion
        juce::String original = "Hello";
        juce::String withInsertion = "Hello World!";

        auto insertPatch = Diff::bsDiff (original, withInsertion);
        juce::String insertResult = Diff::bsApplyPatch (original, insertPatch);
        expectEquals (insertResult, withInsertion, "Should handle insertion");

        // Test deletion
        auto deletePatch = Diff::bsDiff (withInsertion, original);
        juce::String deleteResult = Diff::bsApplyPatch (withInsertion, deletePatch);
        expectEquals (deleteResult, original, "Should handle deletion");
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
        expectEquals (result1, v2, "v1 -> v2 should work");

        juce::String result2 = Diff::bsApplyPatch (result1, patch2to3);
        expectEquals (result2, v3, "v2 -> v3 should work");

        juce::String result3 = Diff::bsApplyPatch (result2, patch3to1);
        expectEquals (result3, v1, "v3 -> v1 should complete the cycle");
    }
};

static DiffTests diffTests;

#endif

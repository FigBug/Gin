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

//==============================================================================
class MyersDiffTests : public juce::UnitTest
{
public:
    MyersDiffTests() : juce::UnitTest ("Myers Diff", "gin") {}

    void runTest() override
    {
        testIdenticalText();
        testEmptyStrings();
        testSimpleInsertion();
        testSimpleDeletion();
        testSimpleChange();
        testMultipleChanges();
        testPatchSerialization();
        testRoundTrip();
        testLargeFiles();
    }

private:
    void testIdenticalText()
    {
        beginTest ("Identical Text");

        juce::String text = "line1\nline2\nline3\n";
        auto patch = Diff::createPatch (text, text);

        expect (patch.hunks.empty(), "No hunks for identical text");
        expectEquals (Diff::applyPatch (text, patch), text, "Applied patch should produce original text");
    }

    void testEmptyStrings()
    {
        beginTest ("Empty Strings");

        // Empty to non-empty
        {
            juce::String empty = "";
            juce::String text = "line1\nline2\n";
            auto patch = Diff::createPatch (empty, text);

            int inserts = 0;
            for (const auto& hunk : patch.hunks)
                if (hunk.isInsert) inserts++;

            expect (inserts > 0, "Should have insertions when adding to empty");
            expectEquals (Diff::applyPatch (text, patch), empty, "Should produce original empty text");
        }

        // Non-empty to empty
        {
            juce::String text = "line1\nline2\n";
            juce::String empty = "";
            auto patch = Diff::createPatch (text, empty);

            int removes = 0;
            for (const auto& hunk : patch.hunks)
                if (! hunk.isInsert) removes++;

            expect (removes > 0, "Should have removals when deleting to empty");
            expectEquals (Diff::applyPatch (empty, patch), text, "Should produce original text");
        }

        // Empty to empty
        {
            auto patch = Diff::createPatch ("", "");
            expect (patch.hunks.empty(), "No hunks for empty to empty");
        }
    }

    void testSimpleInsertion()
    {
        beginTest ("Simple Insertion");

        juce::String oldText = "line1\nline3\n";
        juce::String newText = "line1\nline2\nline3\n";

        auto patch = Diff::createPatch (oldText, newText);

        int inserts = 0;
        for (const auto& hunk : patch.hunks)
            if (hunk.isInsert) inserts++;

        expectEquals (inserts, 1, "Should have one insertion");
        expectEquals (Diff::applyPatch (newText, patch), oldText, "Applied patch should recover old text");
    }

    void testSimpleDeletion()
    {
        beginTest ("Simple Deletion");

        juce::String oldText = "line1\nline2\nline3\n";
        juce::String newText = "line1\nline3\n";

        auto patch = Diff::createPatch (oldText, newText);

        int removes = 0;
        for (const auto& hunk : patch.hunks)
            if (! hunk.isInsert) removes++;

        expectEquals (removes, 1, "Should have one removal");
        expectEquals (Diff::applyPatch (newText, patch), oldText, "Applied patch should recover old text");
    }

    void testSimpleChange()
    {
        beginTest ("Simple Change");

        juce::String oldText = "line1\nold line\nline3\n";
        juce::String newText = "line1\nnew line\nline3\n";

        auto patch = Diff::createPatch (oldText, newText);
        expectEquals (Diff::applyPatch (newText, patch), oldText, "Applied patch should recover old text");

        // A change is represented as remove + insert
        int removes = 0, inserts = 0;
        for (const auto& hunk : patch.hunks)
        {
            if (hunk.isInsert) inserts++;
            else removes++;
        }

        expect (removes >= 1, "Should have at least one removal for a change");
        expect (inserts >= 1, "Should have at least one insertion for a change");
    }

    void testMultipleChanges()
    {
        beginTest ("Multiple Changes");

        juce::String oldText = "aaa\nbbb\nccc\nddd\neee\n";
        juce::String newText = "aaa\nBBB\nccc\nDDD\neee\n";

        auto patch = Diff::createPatch (oldText, newText);
        expectEquals (Diff::applyPatch (newText, patch), oldText, "Applied patch should recover old text");
    }

    void testPatchSerialization()
    {
        beginTest ("Patch Serialization");

        juce::String oldText = "line1\nold\nline3\n";
        juce::String newText = "line1\nnew\nline3\n";

        auto patch = Diff::createPatch (oldText, newText);
        auto patchStr = patch.toString();

        expect (patchStr.contains ("-"), "Should contain remove markers");
        expect (patchStr.contains ("+"), "Should contain insert markers");

        auto restored = Diff::Patch::fromString (patchStr);
        expectEquals ((int) restored.hunks.size(), (int) patch.hunks.size(), "Restored patch should have same number of hunks");
        expectEquals (Diff::applyPatch (newText, restored), oldText, "Restored patch should work");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        juce::String v1 = "int main() {\n    return 0;\n}\n";
        juce::String v2 = "int main() {\n    printf(\"hello\");\n    return 0;\n}\n";
        juce::String v3 = "int main() {\n    return 1;\n}\n";

        auto patch1to2 = Diff::createPatch (v1, v2);
        expectEquals (Diff::applyPatch (v2, patch1to2), v1, "v2 + patch -> v1 should work");

        auto patch2to3 = Diff::createPatch (v2, v3);
        expectEquals (Diff::applyPatch (v3, patch2to3), v2, "v3 + patch -> v2 should work");

        auto patch3to1 = Diff::createPatch (v3, v1);
        expectEquals (Diff::applyPatch (v1, patch3to1), v3, "v1 + patch -> v3 should work");
    }

    void testLargeFiles()
    {
        beginTest ("Large Files");

        juce::String oldText, newText;
        for (int i = 0; i < 500; i++)
        {
            oldText += "Line " + juce::String (i) + " original content\n";
            if (i % 10 == 0)
                newText += "Line " + juce::String (i) + " modified content\n";
            else
                newText += "Line " + juce::String (i) + " original content\n";
        }

        auto patch = Diff::createPatch (oldText, newText);
        expectEquals (Diff::applyPatch (newText, patch), oldText, "Should recover original from large file");

        expect (! patch.hunks.empty(), "Should have hunks for changed file");

        // Check that patch is smaller than storing all lines
        auto patchStr = patch.toString();
        expect (patchStr.length() < oldText.length(), "Patch should be smaller than full text");
    }
};

static MyersDiffTests myersDiffTests;

#endif

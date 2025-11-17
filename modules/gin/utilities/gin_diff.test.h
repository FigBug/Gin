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
        testCompletelyDifferent();
        testEmptyStrings();
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
};

static DiffTests diffTests;

#endif

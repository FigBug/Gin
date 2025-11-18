/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class LayoutPathTests : public juce::UnitTest
{
public:
    LayoutPathTests() : juce::UnitTest ("Layout Path Utilities", "gin_gui") {}

    void runTest() override
    {
        testGetParentPath();
        testAddTrailingSlash();
        testIsAbsolutePath();
        testGetChildPath();
        testExpandTokens();
    }

private:
    void testGetParentPath()
    {
        beginTest ("Get Parent Path");

        expect (getParentPath ("/foo/bar/baz") == "/foo/bar", "Should get parent of /foo/bar/baz");
        expect (getParentPath ("/foo/bar") == "/foo", "Should get parent of /foo/bar");
        expect (getParentPath ("/foo") == "/", "Should get parent of /foo");
        expect (getParentPath ("/") == "/", "Parent of / should be /");
        expect (getParentPath ("foo") == "foo", "No slash should return same");
    }

    void testAddTrailingSlash()
    {
        beginTest ("Add Trailing Slash");

        expect (addTrailingSlash ("/foo") == "/foo/", "Should add trailing slash");
        expect (addTrailingSlash ("/foo/") == "/foo/", "Should not double slash");
        expect (addTrailingSlash ("") == "/", "Empty string should become /");
    }

    void testIsAbsolutePath()
    {
        beginTest ("Is Absolute Path");

        expect (isAbsolutePath ("/foo/bar"), "Should recognize /foo/bar as absolute");
        expect (isAbsolutePath ("/"), "Should recognize / as absolute");
        expect (isAbsolutePath ("~/foo"), "Should recognize ~/foo as absolute");
        expect (!isAbsolutePath ("foo/bar"), "Should recognize foo/bar as relative");
        expect (!isAbsolutePath ("./foo"), "Should recognize ./foo as relative");
        expect (!isAbsolutePath ("../foo"), "Should recognize ../foo as relative");
    }

    void testGetChildPath()
    {
        beginTest ("Get Child Path");

        // Basic child path
        expect (getChildPath ("/foo", "bar") == "/foo/bar", "Should append child");
        expect (getChildPath ("/foo/", "bar") == "/foo/bar", "Should handle trailing slash");

        // Absolute paths
        expect (getChildPath ("/foo", "/bar") == "/bar", "Absolute path should override");
        expect (getChildPath ("/foo", "~/bar") == "~/bar", "Home path should override");

        // Current directory
        expect (getChildPath ("/foo", "./bar") == "/foo/bar", "Should handle ./");
        expect (getChildPath ("/foo", "./bar/baz") == "/foo/bar/baz", "Should handle ./ with subpath");

        // Parent directory
        expect (getChildPath ("/foo/bar", "../baz") == "/foo/baz", "Should handle ../");
        expect (getChildPath ("/foo/bar/qux", "../../baz") == "/foo/baz", "Should handle ../../");
        expect (getChildPath ("/foo", "../bar") == "/bar", "Should handle ../ at root level");

        // Multiple dots
        expect (getChildPath ("/foo/bar", ".././baz") == "/foo/baz", "Should handle .././");
    }

    void testExpandTokens()
    {
        beginTest ("Expand Tokens");

        // No expansion
        juce::StringArray simple {"foo", "bar"};
        auto result1 = expandTokens (simple);
        expect (result1.size() == 2, "Should not expand simple strings");
        expect (result1[0] == "foo", "First should be foo");
        expect (result1[1] == "bar", "Second should be bar");

        // Range expansion
        juce::StringArray range {"item[0..2]"};
        auto result2 = expandTokens (range);
        expect (result2.size() == 3, "Should expand to 3 items");
        expect (result2[0] == "item0", "First should be item0");
        expect (result2[1] == "item1", "Second should be item1");
        expect (result2[2] == "item2", "Third should be item2");

        // Range with prefix and postfix
        juce::StringArray complex {"btn[1..3]_label"};
        auto result3 = expandTokens (complex);
        expect (result3.size() == 3, "Should expand to 3 items");
        expect (result3[0] == "btn1_label", "First should be btn1_label");
        expect (result3[1] == "btn2_label", "Second should be btn2_label");
        expect (result3[2] == "btn3_label", "Third should be btn3_label");

        // Mixed array
        juce::StringArray mixed {"static", "dyn[0..1]", "end"};
        auto result4 = expandTokens (mixed);
        expect (result4.size() == 4, "Should expand to 4 items");
        expect (result4[0] == "static", "First should be static");
        expect (result4[1] == "dyn0", "Second should be dyn0");
        expect (result4[2] == "dyn1", "Third should be dyn1");
        expect (result4[3] == "end", "Fourth should be end");
    }
};

static LayoutPathTests layoutPathTests;

#endif

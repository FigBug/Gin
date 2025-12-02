/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ValueTreeUtilitiesTests : public juce::UnitTest
{
public:
    ValueTreeUtilitiesTests() : juce::UnitTest ("ValueTree Utilities", "gin") {}

    void runTest() override
    {
        testJSONRoundTrip();
        testJSONWithChildren();
        testJSONWithProperties();
        testLambdaListener();
        testBasicConversion();
        testWithProperties();
        testWithChildren();
        testRoundTrip();
        testBinaryData();
    }

private:
    void testJSONRoundTrip()
    {
        beginTest ("JSON Round Trip");

        juce::ValueTree original ("Root");
        original.setProperty ("name", "Test", nullptr);
        original.setProperty ("value", 42, nullptr);
        original.setProperty ("flag", true, nullptr);

        juce::String json = valueTreeToJSON (original);
        expect (json.isNotEmpty(), "JSON should not be empty");

        juce::ValueTree restored = valueTreeFromJSON (json);
        expect (restored.isValid(), "Restored tree should be valid");
        expectEquals (restored.getType().toString(), juce::String ("Root"), "Type should match");
        expectEquals (int (restored.getProperty ("value")), 42, "Integer property should match");
        expect (bool (restored.getProperty ("flag")), "Boolean property should match");
    }

    void testJSONWithChildren()
    {
        beginTest ("JSON With Children");

        juce::ValueTree parent ("Parent");
        juce::ValueTree child1 ("Child");
        child1.setProperty ("id", 1, nullptr);
        juce::ValueTree child2 ("Child");
        child2.setProperty ("id", 2, nullptr);

        parent.appendChild (child1, nullptr);
        parent.appendChild (child2, nullptr);

        juce::String json = valueTreeToJSON (parent);
        juce::ValueTree restored = valueTreeFromJSON (json);

        expectEquals (restored.getNumChildren(), 2, "Should have 2 children");
        expectEquals (int (restored.getChild (0).getProperty ("id")), 1, "First child id should be 1");
        expectEquals (int (restored.getChild (1).getProperty ("id")), 2, "Second child id should be 2");
    }

    void testJSONWithProperties()
    {
        beginTest ("JSON With Various Property Types");

        juce::ValueTree tree ("Test");
        tree.setProperty ("int", 123, nullptr);
        tree.setProperty ("float", 3.14f, nullptr);
        tree.setProperty ("double", 2.71828, nullptr);
        tree.setProperty ("string", "hello", nullptr);
        tree.setProperty ("bool", false, nullptr);

        juce::String json = valueTreeToJSON (tree);
        juce::ValueTree restored = valueTreeFromJSON (json);

        expectEquals (int (restored.getProperty ("int")), 123, "Int should match");
        expectWithinAbsoluteError (float (restored.getProperty ("float")), 3.14f, 0.001f, "Float should match");
        expectEquals (restored.getProperty ("string").toString(), juce::String ("hello"), "String should match");
    }

    void testLambdaListener()
    {
        beginTest ("Lambda Listener");

        juce::ValueTree tree ("Test");
        tree.setProperty ("count", 0, nullptr);

        int changeCount = 0;
        juce::Identifier changedProperty;

        LambdaValueTreeListener listener (tree);
        listener.onChanged = [&changeCount] { changeCount++; };
        listener.onValueTreePropertyChanged = [&changedProperty] (juce::ValueTree&, const juce::Identifier& prop) {
            changedProperty = prop;
        };

        tree.setProperty ("count", 1, nullptr);
        expectEquals (changeCount, 1, "onChanged should be called");
        expectEquals (changedProperty.toString(), juce::String ("count"), "Property identifier should be passed");

        tree.setProperty ("name", "test", nullptr);
        expectEquals (changeCount, 2, "onChanged should be called again");
        expectEquals (changedProperty.toString(), juce::String ("name"), "New property identifier should be passed");
    }

    void testBasicConversion()
    {
        beginTest ("Basic Conversion");

        juce::ValueTree vt ("Root");

        juce::String json = valueTreeToJSON (vt);
        expect (json.contains ("Root"), "JSON should contain root name");

        juce::ValueTree vt2 = valueTreeFromJSON (json);
        expect (vt2.getType().toString() == "Root", "Restored tree should have same type");
    }

    void testWithProperties()
    {
        beginTest ("With Properties");

        juce::ValueTree vt ("Node");
        vt.setProperty ("name", "test", nullptr);
        vt.setProperty ("value", 42, nullptr);
        vt.setProperty ("enabled", true, nullptr);

        juce::String json = valueTreeToJSON (vt);
        juce::ValueTree vt2 = valueTreeFromJSON (json);

        expect (vt2.getType().toString() == "Node", "Type should match");
        expect (vt2["name"].toString() == "test", "String property should match");
        expect (vt2["value"].toString() == "42", "Int property should be converted to string");
        expect (vt2["enabled"].toString() == "1", "Bool property should be converted to string");
    }

    void testWithChildren()
    {
        beginTest ("With Children");

        juce::ValueTree root ("Root");
        juce::ValueTree child1 ("Child1");
        juce::ValueTree child2 ("Child2");

        child1.setProperty ("id", 1, nullptr);
        child2.setProperty ("id", 2, nullptr);

        root.addChild (child1, -1, nullptr);
        root.addChild (child2, -1, nullptr);

        juce::String json = valueTreeToJSON (root);
        juce::ValueTree restored = valueTreeFromJSON (json);

        expect (restored.getNumChildren() == 2, "Should have 2 children");
        expect (restored.getChild(0).getType().toString() == "Child1", "First child type should match");
        expect (restored.getChild(1).getType().toString() == "Child2", "Second child type should match");
        expect (restored.getChild(0)["id"].toString() == "1", "First child property should match");
        expect (restored.getChild(1)["id"].toString() == "2", "Second child property should match");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        juce::ValueTree original ("Settings");
        original.setProperty ("volume", 0.75, nullptr);
        original.setProperty ("pan", 0.5, nullptr);

        juce::ValueTree params ("Parameters");
        params.setProperty ("cutoff", 1000, nullptr);
        params.setProperty ("resonance", 0.3, nullptr);
        original.addChild (params, -1, nullptr);

        // Convert to JSON and back
        juce::String json = valueTreeToJSON (original);
        juce::ValueTree restored = valueTreeFromJSON (json);

        // Verify structure
        expect (restored.getType() == original.getType(), "Types should match");
        expect (restored.getNumChildren() == original.getNumChildren(), "Child count should match");

        // Convert back to JSON and compare
        juce::String json2 = valueTreeToJSON (restored);
        expect (json == json2, "JSON should be identical after round trip");
    }

    void testBinaryData()
    {
        beginTest ("Binary Data");

        juce::ValueTree vt ("Data");

        juce::MemoryBlock mb;
        mb.append ("Hello", 5);
        vt.setProperty ("binary", juce::var (mb), nullptr);

        juce::String json = valueTreeToJSON (vt);
        expect (json.contains ("base64:binary"), "JSON should contain base64-encoded binary");

        juce::ValueTree restored = valueTreeFromJSON (json);

        auto restoredMb = restored["binary"].getBinaryData();
        expect (restoredMb != nullptr, "Binary data should be restored");

        if (restoredMb)
        {
            expect (restoredMb->getSize() == 5, "Binary data size should match");
            expect (std::memcmp (restoredMb->getData(), "Hello", 5) == 0,
                   "Binary data content should match");
        }
    }
};

static ValueTreeUtilitiesTests valueTreeUtilitiesTests;

#endif

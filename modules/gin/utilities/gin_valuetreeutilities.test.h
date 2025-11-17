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
};

static ValueTreeUtilitiesTests valueTreeUtilitiesTests;

#endif

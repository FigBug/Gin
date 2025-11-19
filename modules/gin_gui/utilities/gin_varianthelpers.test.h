//==============================================================================
#if GIN_UNIT_TESTS

class VariantHelpersTests : public juce::UnitTest
{
public:
    VariantHelpersTests() : juce::UnitTest ("VariantHelpers", "gin")
    {
    }

    void runTest() override
    {
        testGetAllPropertyNames();
        testGetAllPropertiesWithDynamicObject();
        testGetAllPropertiesWithArray();
        testGetAllPropertiesWithPrimitive();
        testGetPropertySet();
        testGetPropertyWithDefault();
        testSetProperty();
        testPropertyStruct();
        testEmptyCases();
    }

private:
    void testGetAllPropertyNames()
    {
        beginTest ("Get All Property Names");

        // Create a DynamicObject with properties
        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("name", "test");

        auto names = getAllPropertyNames (obj);

        expectEquals (names.size(), 3, "Should have 3 properties");
        expect (names.contains (juce::Identifier ("width")), "Should contain 'width'");
        expect (names.contains (juce::Identifier ("height")), "Should contain 'height'");
        expect (names.contains (juce::Identifier ("name")), "Should contain 'name'");
    }

    void testGetAllPropertiesWithDynamicObject()
    {
        beginTest ("Get All Properties - DynamicObject");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("visible", true);

        auto props = getAllProperties (obj);

        expectEquals (props.size(), 3, "Should have 3 property values");

        // Values should be present (order may vary)
        bool hasWidth = false;
        bool hasHeight = false;
        bool hasVisible = false;

        for (const auto& prop : props)
        {
            if (prop.isInt() && int (prop) == 100)
                hasWidth = true;
            if (prop.isInt() && int (prop) == 200)
                hasHeight = true;
            if (prop.isBool() && bool (prop) == true)
                hasVisible = true;
        }

        expect (hasWidth, "Should contain width value 100");
        expect (hasHeight, "Should contain height value 200");
        expect (hasVisible, "Should contain visible value true");
    }

    void testGetAllPropertiesWithArray()
    {
        beginTest ("Get All Properties - Array");

        juce::var arr;
        arr.append (10);
        arr.append (20);
        arr.append (30);

        auto props = getAllProperties (arr);

        expectEquals (props.size(), 3, "Should have 3 array elements");
        expectEquals (int (props[0]), 10, "First element should be 10");
        expectEquals (int (props[1]), 20, "Second element should be 20");
        expectEquals (int (props[2]), 30, "Third element should be 30");
    }

    void testGetAllPropertiesWithPrimitive()
    {
        beginTest ("Get All Properties - Primitive");

        juce::var primitive = 42;
        auto props = getAllProperties (primitive);

        expectEquals (props.size(), 0, "Primitive should return empty array");
    }

    void testGetPropertySet()
    {
        beginTest ("Get Property Set");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("name", "TestComponent");

        auto propSet = getPropertySet (obj);

        expectEquals (propSet.size(), 3, "Should have 3 key-value pairs");

        // Find and verify each property
        bool foundWidth = false;
        bool foundHeight = false;
        bool foundName = false;

        for (const auto& prop : propSet)
        {
            if (prop.key == "width")
            {
                expectEquals (int (prop.value), 100, "Width should be 100");
                foundWidth = true;
            }
            else if (prop.key == "height")
            {
                expectEquals (int (prop.value), 200, "Height should be 200");
                foundHeight = true;
            }
            else if (prop.key == "name")
            {
                expectEquals (prop.value.toString(), juce::String ("TestComponent"), "Name should be 'TestComponent'");
                foundName = true;
            }
        }

        expect (foundWidth, "Should find width property");
        expect (foundHeight, "Should find height property");
        expect (foundName, "Should find name property");
    }

    void testGetPropertyWithDefault()
    {
        beginTest ("Get Property With Default");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("visible", false);

        // Test existing property
        auto width = getPropertyWithDefault (obj, "width", 50);
        expectEquals (int (width), 100, "Should return existing property value");

        auto visible = getPropertyWithDefault (obj, "visible", true);
        expectEquals (bool (visible), false, "Should return existing boolean property value");

        // Test missing property with default
        auto height = getPropertyWithDefault (obj, "height", 200);
        expectEquals (int (height), 200, "Should return default value for missing property");

        auto opacity = getPropertyWithDefault (obj, "opacity", 1.0);
        expectWithinAbsoluteError (double (opacity), 1.0, 0.001, "Should return default double value");

        // Test with string default
        auto title = getPropertyWithDefault (obj, "title", juce::var ("Default Title"));
        expectEquals (title.toString(), juce::String ("Default Title"), "Should return default string value");
    }

    void testSetProperty()
    {
        beginTest ("Set Property");

        juce::var obj = new juce::DynamicObject();

        // Set various property types
        setProperty (obj, "width", 100);
        setProperty (obj, "height", 200);
        setProperty (obj, "name", "TestComponent");
        setProperty (obj, "visible", true);
        setProperty (obj, "opacity", 0.5);

        // Verify properties were set
        expect (obj.hasProperty ("width"), "Should have width property");
        expect (obj.hasProperty ("height"), "Should have height property");
        expect (obj.hasProperty ("name"), "Should have name property");
        expect (obj.hasProperty ("visible"), "Should have visible property");
        expect (obj.hasProperty ("opacity"), "Should have opacity property");

        expectEquals (int (obj["width"]), 100, "Width should be 100");
        expectEquals (int (obj["height"]), 200, "Height should be 200");
        expectEquals (obj["name"].toString(), juce::String ("TestComponent"), "Name should be 'TestComponent'");
        expectEquals (bool (obj["visible"]), true, "Visible should be true");
        expectWithinAbsoluteError (double (obj["opacity"]), 0.5, 0.001, "Opacity should be 0.5");

        // Test overwriting existing property
        setProperty (obj, "width", 150);
        expectEquals (int (obj["width"]), 150, "Width should be updated to 150");
    }

    void testPropertyStruct()
    {
        beginTest ("Property Struct");

        // Test default constructor
        Property prop1;
        expectEquals (prop1.key, juce::String(), "Default key should be empty");
        expect (prop1.value.isVoid(), "Default value should be void");

        // Test parameterized constructor
        Property prop2 ("width", 100);
        expectEquals (prop2.key, juce::String ("width"), "Key should be 'width'");
        expectEquals (int (prop2.value), 100, "Value should be 100");

        // Test with different value types
        Property prop3 ("name", "TestComponent");
        expectEquals (prop3.key, juce::String ("name"), "Key should be 'name'");
        expectEquals (prop3.value.toString(), juce::String ("TestComponent"), "Value should be 'TestComponent'");

        Property prop4 ("visible", true);
        expectEquals (prop4.key, juce::String ("visible"), "Key should be 'visible'");
        expectEquals (bool (prop4.value), true, "Value should be true");
    }

    void testEmptyCases()
    {
        beginTest ("Empty Cases");

        // Test with empty DynamicObject
        juce::var emptyObj = new juce::DynamicObject();

        auto names = getAllPropertyNames (emptyObj);
        expectEquals (names.size(), 0, "Empty object should have no property names");

        auto props = getAllProperties (emptyObj);
        expectEquals (props.size(), 0, "Empty object should have no properties");

        auto propSet = getPropertySet (emptyObj);
        expectEquals (propSet.size(), 0, "Empty object should have no property set");

        // Test with void var
        juce::var voidVar;

        auto voidNames = getAllPropertyNames (voidVar);
        expectEquals (voidNames.size(), 0, "Void var should have no property names");

        auto voidProps = getAllProperties (voidVar);
        expectEquals (voidProps.size(), 0, "Void var should have no properties");

        auto voidPropSet = getPropertySet (voidVar);
        expectEquals (voidPropSet.size(), 0, "Void var should have no property set");

        // Test with empty array
        juce::var emptyArr;
        emptyArr = juce::Array<juce::var>();

        auto arrProps = getAllProperties (emptyArr);
        expectEquals (arrProps.size(), 0, "Empty array should have no properties");

        // Test setProperty on non-DynamicObject (should do nothing, not crash)
        juce::var primitive = 42;
        setProperty (primitive, "width", 100);
        expect (! primitive.hasProperty ("width"), "Primitive should not accept properties");
    }
};

static VariantHelpersTests variantHelpersTests;

#endif

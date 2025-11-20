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
        testSetJSONPointerObject();
        testSetJSONPointerArray();
        testSetJSONPointerNested();
        testSetJSONPointerArrayAppend();
        testSetJSONPointerInvalidPaths();
        testGetJSONPointerObject();
        testGetJSONPointerArray();
        testGetJSONPointerNested();
        testGetJSONPointerDefaultValue();
        testGetJSONPointerInvalidPaths();
        testVarIteratorObject();
        testVarIteratorArray();
        testVarIteratorEmpty();
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
        expect (! bool (visible), "Should return existing boolean property value");

        // Test missing property with default
        auto height = getPropertyWithDefault (obj, "height", 200);
        expectEquals (int (height), 200, "Should return default value for missing property");

        auto opacity = getPropertyWithDefault (obj, "opacity", 1.0);
        expectWithinAbsoluteError (double (opacity), 1.0, 0.001, "Should return default double value");

        // Test with string default
        auto title = getPropertyWithDefault (obj, "title", juce::var ("Default Title"));
        expect (title.toString() == juce::String ("Default Title"), "Should return default string value");
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
        expect (obj["name"].toString() == juce::String ("TestComponent"), "Name should be 'TestComponent'");
        expect (bool (obj["visible"]), "Visible should be true");
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
        expect (prop2.key == juce::String ("width"), "Key should be 'width'");
        expectEquals (int (prop2.value), 100, "Value should be 100");

        // Test with different value types
        Property prop3 ("name", "TestComponent");
        expect (prop3.key == juce::String ("name"), "Key should be 'name'");
        expect (prop3.value.toString() == juce::String ("TestComponent"), "Value should be 'TestComponent'");

        Property prop4 ("visible", true);
        expect (prop4.key == juce::String ("visible"), "Key should be 'visible'");
        expect (bool (prop4.value) == true, "Value should be true");
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

    void testSetJSONPointerObject()
    {
        beginTest ("Set JSON Pointer - Object");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);

        // Set existing property
        bool result = setJSONPointer (obj, "/width", 150);
        expect (result, "Should successfully set existing property");
        expectEquals (int (obj["width"]), 150, "Width should be updated to 150");

        // Set new property
        result = setJSONPointer (obj, "/name", "TestObject");
        expect (result, "Should successfully set new property");
        expect (obj["name"].toString() == juce::String ("TestObject"), "Name should be set to TestObject");

        // Verify other properties unchanged
        expectEquals (int (obj["height"]), 200, "Height should remain 200");
    }

    void testSetJSONPointerArray()
    {
        beginTest ("Set JSON Pointer - Array");

        juce::var arr;
        arr.append (10);
        arr.append (20);
        arr.append (30);

        // Set existing array element
        bool result = setJSONPointer (arr, "/1", 99);
        expect (result, "Should successfully set array element");
        expectEquals (int (arr[1]), 99, "Second element should be updated to 99");

        // Set first element
        result = setJSONPointer (arr, "/0", 5);
        expect (result, "Should successfully set first element");
        expectEquals (int (arr[0]), 5, "First element should be updated to 5");

        // Verify other elements unchanged
        expectEquals (int (arr[2]), 30, "Third element should remain 30");
    }

    void testSetJSONPointerNested()
    {
        beginTest ("Set JSON Pointer - Nested");

        // Create nested structure: { user: { name: "John", age: 30 } }
        juce::var user = new juce::DynamicObject();
        user.getDynamicObject()->setProperty ("name", "John");
        user.getDynamicObject()->setProperty ("age", 30);

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("user", user);

        // Set nested property
        bool result = setJSONPointer (obj, "/user/name", "Jane");
        expect (result, "Should successfully set nested property");
        expect (obj["user"]["name"].toString() == juce::String ("Jane"), "Nested name should be updated to Jane");

        // Set another nested property
        result = setJSONPointer (obj, "/user/age", 25);
        expect (result, "Should successfully set nested age");
        expectEquals (int (obj["user"]["age"]), 25, "Nested age should be updated to 25");
    }

    void testSetJSONPointerArrayAppend()
    {
        beginTest ("Set JSON Pointer - Array Append");

        juce::var arr;
        arr.append (10);
        arr.append (20);

        // Append to array using "-" syntax
        bool result = setJSONPointer (arr, "/-", 30);
        expect (result, "Should successfully append to array");
        expectEquals (arr.size(), 3, "Array should have 3 elements");
        expectEquals (int (arr[2]), 30, "Appended element should be 30");

        // Append another element
        result = setJSONPointer (arr, "/-", 40);
        expect (result, "Should successfully append another element");
        expectEquals (arr.size(), 4, "Array should have 4 elements");
        expectEquals (int (arr[3]), 40, "Appended element should be 40");
    }

    void testSetJSONPointerInvalidPaths()
    {
        beginTest ("Set JSON Pointer - Invalid Paths");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);

        // Empty pointer should fail
        bool result = setJSONPointer (obj, "", 200);
        expect (! result, "Empty pointer should fail");

        // Non-existent nested path should fail
        result = setJSONPointer (obj, "/user/name", "John");
        expect (! result, "Setting non-existent nested path should fail");

        // Out of bounds array index should fail
        juce::var arr;
        arr.append (10);
        arr.append (20);

        result = setJSONPointer (arr, "/5", 99);
        expect (! result, "Out of bounds array index should fail");
    }

    void testGetJSONPointerObject()
    {
        beginTest ("Get JSON Pointer - Object");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("name", "TestObject");

        // Get existing properties
        auto width = getJSONPointer (obj, "/width", -1);
        expectEquals (int (width), 100, "Should get width value 100");

        auto name = getJSONPointer (obj, "/name", "default");
        expect (name.toString() == juce::String ("TestObject"), "Should get name value TestObject");

        auto height = getJSONPointer (obj, "/height", -1);
        expectEquals (int (height), 200, "Should get height value 200");
    }

    void testGetJSONPointerArray()
    {
        beginTest ("Get JSON Pointer - Array");

        juce::var arr;
        arr.append (10);
        arr.append (20);
        arr.append (30);

        // Get array elements by index
        auto elem0 = getJSONPointer (arr, "/0", -1);
        expectEquals (int (elem0), 10, "Should get first element 10");

        auto elem1 = getJSONPointer (arr, "/1", -1);
        expectEquals (int (elem1), 20, "Should get second element 20");

        auto elem2 = getJSONPointer (arr, "/2", -1);
        expectEquals (int (elem2), 30, "Should get third element 30");
    }

    void testGetJSONPointerNested()
    {
        beginTest ("Get JSON Pointer - Nested");

        // Create nested structure
        juce::var address = new juce::DynamicObject();
        address.getDynamicObject()->setProperty ("city", "London");
        address.getDynamicObject()->setProperty ("zip", "SW1A 1AA");

        juce::var user = new juce::DynamicObject();
        user.getDynamicObject()->setProperty ("name", "John");
        user.getDynamicObject()->setProperty ("age", 30);
        user.getDynamicObject()->setProperty ("address", address);

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("user", user);

        // Get nested properties
        auto name = getJSONPointer (obj, "/user/name", "default");
        expect (name.toString() == juce::String ("John"), "Should get nested name John");

        auto city = getJSONPointer (obj, "/user/address/city", "default");
        expect (city.toString() == juce::String ("London"), "Should get deeply nested city London");

        auto age = getJSONPointer (obj, "/user/age", -1);
        expectEquals (int (age), 30, "Should get nested age 30");
    }

    void testGetJSONPointerDefaultValue()
    {
        beginTest ("Get JSON Pointer - Default Value");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);

        // Get non-existent property should return default
        auto missing = getJSONPointer (obj, "/height", 999);
        expectEquals (int (missing), 999, "Should return default value for missing property");

        // Get non-existent nested property should return default
        auto nestedMissing = getJSONPointer (obj, "/user/name", "default");
        expect (nestedMissing.toString() == juce::String ("default"), "Should return default for missing nested property");

        // Array out of bounds should return default
        juce::var arr;
        arr.append (10);
        arr.append (20);

        auto outOfBounds = getJSONPointer (arr, "/5", -1);
        expectEquals (int (outOfBounds), -1, "Should return default for out of bounds index");
    }

    void testGetJSONPointerInvalidPaths()
    {
        beginTest ("Get JSON Pointer - Invalid Paths");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);

        // Empty pointer should return default
        auto empty = getJSONPointer (obj, "", -1);
        expectEquals (int (empty), -1, "Empty pointer should return default");

        // Non-existent path should return default
        auto missing = getJSONPointer (obj, "/user/name/first", "default");
        expect (missing.toString() == juce::String ("default"), "Non-existent path should return default");
    }

    void testVarIteratorObject()
    {
        beginTest ("Var Iterator - Object");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("name", "TestObject");

        // Iterate using range-based for loop
        int count = 0;
        bool foundWidth = false;
        bool foundHeight = false;
        bool foundName = false;

        for (const auto& item : obj)
        {
            count++;

            if (item.name.toString() == "width")
            {
                expectEquals (int (item.value), 100, "Width value should be 100");
                foundWidth = true;
            }
            else if (item.name.toString() == "height")
            {
                expectEquals (int (item.value), 200, "Height value should be 200");
                foundHeight = true;
            }
            else if (item.name.toString() == "name")
            {
                expect (item.value.toString() == juce::String ("TestObject"), "Name value should be TestObject");
                foundName = true;
            }
        }

        expectEquals (count, 3, "Should iterate over 3 properties");
        expect (foundWidth, "Should find width property");
        expect (foundHeight, "Should find height property");
        expect (foundName, "Should find name property");
    }

    void testVarIteratorArray()
    {
        beginTest ("Var Iterator - Array");

        juce::var arr;
        arr.append (10);
        arr.append (20);
        arr.append (30);

        // Iterate using range-based for loop
        int count = 0;
        juce::Array<int> values;

        for (const auto& item : arr)
        {
            count++;
            values.add (int (item.value));

            // For arrays, name should be the index
            expectEquals (int (item.name), count - 1, "Array item name should be index");
        }

        expectEquals (count, 3, "Should iterate over 3 elements");
        expectEquals (values[0], 10, "First value should be 10");
        expectEquals (values[1], 20, "Second value should be 20");
        expectEquals (values[2], 30, "Third value should be 30");
    }

    void testVarIteratorEmpty()
    {
        beginTest ("Var Iterator - Empty");

        // Empty object
        juce::var emptyObj = new juce::DynamicObject();
        int objCount = 0;

        for (const auto& item : emptyObj)
        {
            (void) item;  // Suppress unused warning
            objCount++;
        }

        expectEquals (objCount, 0, "Should not iterate over empty object");

        // Empty array
        juce::var emptyArr;
        emptyArr = juce::Array<juce::var>();
        int arrCount = 0;

        for (const auto& item : emptyArr)
        {
            (void) item;  // Suppress unused warning
            arrCount++;
        }

        expectEquals (arrCount, 0, "Should not iterate over empty array");

        // Primitive value (should not iterate)
        juce::var primitive = 42;
        int primCount = 0;

        for (const auto& item : primitive)
        {
            (void) item;  // Suppress unused warning
            primCount++;
        }

        expectEquals (primCount, 0, "Should not iterate over primitive value");
    }
};

static VariantHelpersTests variantHelpersTests;

#endif

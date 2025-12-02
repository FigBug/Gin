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
        testRemoveJsonCommentsLineComments();
        testRemoveJsonCommentsBlockComments();
        testRemoveJsonCommentsInStrings();
        testRemoveJsonCommentsEscapedQuotes();
        testRemoveJsonCommentsMixed();
        testRemoveJsonCommentsMultiline();
        testRemoveJsonCommentsEdgeCases();
        testRemoveJsonCommentsRealWorld();
        testJsonObject();
        testToStdString();
        testRemoveProperty();
        testSetJSONPointerAutoCreate();
        testHasJSONPointer();
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

    void testRemoveJsonCommentsLineComments()
    {
        beginTest ("Remove JSON Comments - Line Comments");

        // Single line comment
        juce::String input = R"({
            "width": 100, // This is a width
            "height": 200
        })";

        auto result = removeJsonComments (input);
        expect (! result.contains ("This is a width"), "Should remove line comment text");
        expect (result.contains ("100"), "Should preserve value before comment");
        expect (result.contains ("200"), "Should preserve value after comment");

        // Line comment at end
        input = R"({
            "name": "test" // end comment
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("end comment"), "Should remove comment at end");
        expect (result.contains ("test"), "Should preserve string value");

        // Multiple line comments
        input = R"({
            // First comment
            "a": 1, // Second comment
            "b": 2  // Third comment
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("First comment"), "Should remove first comment");
        expect (! result.contains ("Second comment"), "Should remove second comment");
        expect (! result.contains ("Third comment"), "Should remove third comment");
        expect (result.contains ("\"a\""), "Should preserve key a");
        expect (result.contains ("\"b\""), "Should preserve key b");
    }

    void testRemoveJsonCommentsBlockComments()
    {
        beginTest ("Remove JSON Comments - Block Comments");

        // Single block comment
        juce::String input = R"({
            "width": 100, /* this is width */
            "height": 200
        })";

        auto result = removeJsonComments (input);
        expect (! result.contains ("this is width"), "Should remove block comment");
        expect (result.contains ("100"), "Should preserve value");
        expect (result.contains ("200"), "Should preserve next value");

        // Block comment at start
        input = R"(/* Header comment */
        {
            "name": "test"
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("Header comment"), "Should remove header comment");
        expect (result.contains ("test"), "Should preserve content");

        // Multiple block comments
        input = R"({
            /* First */ "a": 1,
            "b": /* Second */ 2
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("First"), "Should remove first block comment");
        expect (! result.contains ("Second"), "Should remove second block comment");
        expect (result.contains ("\"a\""), "Should preserve key");
        expect (result.contains ("1"), "Should preserve value");
    }

    void testRemoveJsonCommentsInStrings()
    {
        beginTest ("Remove JSON Comments - Comments In Strings");

        // Line comment syntax in string should be preserved
        juce::String input = R"({
            "url": "http://example.com",
            "note": "Use // for comments"
        })";

        auto result = removeJsonComments (input);
        expect (result.contains ("http://example.com"), "Should preserve // in URL");
        expect (result.contains ("Use // for comments"), "Should preserve // in string");

        // Block comment syntax in string should be preserved
        input = R"({
            "pattern": "/* wildcard */",
            "text": "Use /* and */ for blocks"
        })";

        result = removeJsonComments (input);
        expect (result.contains ("/* wildcard */"), "Should preserve /* */ in string");
        expect (result.contains ("Use /* and */ for blocks"), "Should preserve block syntax in string");

        // Mixed real comments and string content
        input = R"({
            "url": "http://test.com", // Real comment
            "note": "Not a // comment"
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("Real comment"), "Should remove actual comment");
        expect (result.contains ("Not a // comment"), "Should preserve comment-like text in string");
    }

    void testRemoveJsonCommentsEscapedQuotes()
    {
        beginTest ("Remove JSON Comments - Escaped Quotes");

        // String with escaped quote
        juce::String input = R"({
            "text": "He said \"hello\"", // Comment here
            "width": 100
        })";

        auto result = removeJsonComments (input);
        expect (result.contains ("He said \\\"hello\\\""), "Should preserve escaped quotes");
        expect (! result.contains ("Comment here"), "Should remove comment after string with escapes");
        expect (result.contains ("100"), "Should preserve next value");

        // Multiple escaped quotes
        input = R"({
            "quote": "\"First\" and \"Second\""
        })";

        result = removeJsonComments (input);
        expect (result.contains ("\\\"First\\\""), "Should preserve first escaped quote");
        expect (result.contains ("\\\"Second\\\""), "Should preserve second escaped quote");

        // Escaped backslash before quote
        input = R"({
            "path": "C:\\Program Files\\Test" // Windows path
        })";

        result = removeJsonComments (input);
        expect (result.contains ("C:\\\\Program Files\\\\Test"), "Should preserve escaped backslashes");
        expect (! result.contains ("Windows path"), "Should remove comment");
    }

    void testRemoveJsonCommentsMixed()
    {
        beginTest ("Remove JSON Comments - Mixed Comments");

        // Line and block comments together
        juce::String input = R"({
            /* Block comment */
            "a": 1, // Line comment
            /* Another block */ "b": 2 // Another line
        })";

        auto result = removeJsonComments (input);
        expect (! result.contains ("Block comment"), "Should remove block comment");
        expect (! result.contains ("Line comment"), "Should remove line comment");
        expect (! result.contains ("Another block"), "Should remove second block comment");
        expect (! result.contains ("Another line"), "Should remove second line comment");
        expect (result.contains ("\"a\""), "Should preserve key a");
        expect (result.contains ("\"b\""), "Should preserve key b");

        // Comments and strings mixed
        input = R"({
            "url": "http://test.com", // Real comment
            /* Block */ "note": "Not // comment" // End
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("Real comment"), "Should remove line comment");
        expect (! result.contains ("Block"), "Should remove block comment");
        expect (! result.contains ("End"), "Should remove end comment");
        expect (result.contains ("http://test.com"), "Should preserve URL");
        expect (result.contains ("Not // comment"), "Should preserve string with //");
    }

    void testRemoveJsonCommentsMultiline()
    {
        beginTest ("Remove JSON Comments - Multiline");

        // Multiline block comment
        juce::String input = R"({
            /* This is a
               multiline
               comment */
            "width": 100,
            "height": 200
        })";

        auto result = removeJsonComments (input);
        expect (! result.contains ("This is a"), "Should remove multiline comment line 1");
        expect (! result.contains ("multiline"), "Should remove multiline comment line 2");
        expect (! result.contains ("comment"), "Should remove multiline comment line 3");
        expect (result.contains ("100"), "Should preserve value after multiline comment");

        // Multiple line comments
        input = R"({
            // Comment 1
            // Comment 2
            // Comment 3
            "value": 42
        })";

        result = removeJsonComments (input);
        expect (! result.contains ("Comment 1"), "Should remove first line comment");
        expect (! result.contains ("Comment 2"), "Should remove second line comment");
        expect (! result.contains ("Comment 3"), "Should remove third line comment");
        expect (result.contains ("42"), "Should preserve value");
    }

    void testRemoveJsonCommentsEdgeCases()
    {
        beginTest ("Remove JSON Comments - Edge Cases");

        // Empty string
        juce::String input = "";
        auto result = removeJsonComments (input);
        expectEquals (result, juce::String (""), "Empty string should remain empty");

        // No comments
        input = R"({"width": 100, "height": 200})";
        result = removeJsonComments (input);
        expect (result.contains ("100"), "Should preserve content without comments");
        expect (result.contains ("200"), "Should preserve all content");

        // Only comments
        input = R"(// Just a comment
        /* Block comment */)";
        result = removeJsonComments (input);
        expect (! result.contains ("Just a comment"), "Should remove line comment");
        expect (! result.contains ("Block comment"), "Should remove block comment");

        // Comment-like but not quite
        input = R"({
            "value": 100 / 2,
            "star": "*"
        })";
        result = removeJsonComments (input);
        expect (result.contains ("100 / 2"), "Should preserve division operator");
        expect (result.contains ("*"), "Should preserve asterisk in string");

        // Adjacent comment markers
        input = R"({
            "a": 1, //// Multiple slashes
            "b": 2  /* Nested /* attempt */ value */
        })";
        result = removeJsonComments (input);
        expect (! result.contains ("Multiple slashes"), "Should remove comment with multiple slashes");
        expect (! result.contains ("Nested"), "Should remove block comment");
        expect (! result.contains ("attempt"), "Should remove nested-like content");
        expect (result.contains ("value"), "Should preserve content after first closing");

        // Note: C-style comments don't actually nest, so /* /* */ */ closes at first */
        input = R"({
            "c": 3 /* outer /* inner */ still commenting */
        })";
        result = removeJsonComments (input);
        expect (! result.contains ("outer"), "Should remove first part");
        expect (! result.contains ("inner"), "Should remove inner part");
        expect (result.contains ("still commenting"), "Content after first */ is not in comment");
    }

    void testRemoveJsonCommentsRealWorld()
    {
        beginTest ("Remove JSON Comments - Real World Example");

        // Realistic configuration file with documentation
        juce::String input = R"({
            // Application Configuration
            /*
             * Window settings
             */
            "window": {
                "width": 800,   // Default width
                "height": 600,  // Default height
                "title": "My App // Not a comment"
            },
            /* User preferences */
            "preferences": {
                "theme": "dark", // Can be "light" or "dark"
                "language": "en" // ISO 639-1 code
            },
            // Feature flags
            "features": {
                "experimental": false  /* Set to true to enable */
            }
        })";

        auto result = removeJsonComments (input);

        // Verify comments are removed
        expect (! result.contains ("Application Configuration"), "Should remove header comment");
        expect (! result.contains ("Window settings"), "Should remove block comment");
        expect (! result.contains ("Default width"), "Should remove inline comment");
        expect (! result.contains ("User preferences"), "Should remove section comment");
        expect (! result.contains ("Can be"), "Should remove explanation comment");
        expect (! result.contains ("ISO 639-1"), "Should remove format comment");
        expect (! result.contains ("Feature flags"), "Should remove feature comment");
        expect (! result.contains ("Set to true"), "Should remove instruction comment");

        // Verify content is preserved
        expect (result.contains ("\"window\""), "Should preserve window key");
        expect (result.contains ("800"), "Should preserve width value");
        expect (result.contains ("600"), "Should preserve height value");
        expect (result.contains ("My App // Not a comment"), "Should preserve comment-like text in string");
        expect (result.contains ("\"theme\""), "Should preserve theme key");
        expect (result.contains ("\"dark\""), "Should preserve theme value");
        expect (result.contains ("\"language\""), "Should preserve language key");
        expect (result.contains ("\"experimental\""), "Should preserve experimental key");
        expect (result.contains ("false"), "Should preserve boolean value");

        // Verify it can be parsed as valid JSON
        auto parsed = juce::JSON::parse (result);
        expect (! parsed.isVoid(), "Result should be valid JSON");
        expect (parsed.hasProperty ("window"), "Should have window property");
        expect (parsed["window"].hasProperty ("width"), "Should have nested width property");
        expectEquals (int (parsed["window"]["width"]), 800, "Width should be 800");
    }

    void testJsonObject()
    {
        beginTest ("JSON Object Helper");

        // Create empty object
        auto obj = jsonObject();
        expect (! obj.isVoid(), "Should not be void");
        expect (obj.isObject(), "Should be an object");
        expect (obj.getDynamicObject() != nullptr, "Should have DynamicObject");

        // Should start empty
        auto names = getAllPropertyNames (obj);
        expectEquals (names.size(), 0, "Should start with no properties");

        // Should be able to add properties
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);

        expectEquals (int (obj["width"]), 100, "Should have width property");
        expectEquals (int (obj["height"]), 200, "Should have height property");

        // Multiple objects should be independent
        auto obj2 = jsonObject();
        obj2.getDynamicObject()->setProperty ("name", "test");

        expect (! obj.hasProperty ("name"), "First object should not have name");
        expect (obj2.hasProperty ("name"), "Second object should have name");
    }

    void testToStdString()
    {
        beginTest ("To Std String");

        // String conversion
        juce::var str = "hello";
        auto stdStr = toStdString (str);
        expectEquals (stdStr, std::string ("hello"), "Should convert string");

        // Number conversion
        juce::var num = 42;
        auto stdNum = toStdString (num);
        expectEquals (stdNum, std::string ("42"), "Should convert number to string");

        // Boolean conversion
        juce::var boolTrue = true;
        auto stdBool = toStdString (boolTrue);
        expectEquals (stdBool, std::string ("1"), "Should convert true to '1'");

        // Empty string
        juce::var empty = "";
        auto stdEmpty = toStdString (empty);
        expectEquals (stdEmpty, std::string (""), "Should handle empty string");

        // Object conversion
        juce::var obj = new juce::DynamicObject();
        auto stdObj = toStdString (obj);
        expect (stdObj.length() > 0, "Should convert object to some string representation");
    }

    void testRemoveProperty()
    {
        beginTest ("Remove Property");

        juce::var obj = new juce::DynamicObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);
        obj.getDynamicObject()->setProperty ("name", "test");

        // Verify properties exist
        expect (obj.hasProperty ("width"), "Should have width initially");
        expect (obj.hasProperty ("height"), "Should have height initially");
        expect (obj.hasProperty ("name"), "Should have name initially");

        // Remove one property
        removeProperty (obj, "width");
        expect (! obj.hasProperty ("width"), "Should not have width after removal");
        expect (obj.hasProperty ("height"), "Should still have height");
        expect (obj.hasProperty ("name"), "Should still have name");

        // Remove another property
        removeProperty (obj, "name");
        expect (! obj.hasProperty ("name"), "Should not have name after removal");
        expect (obj.hasProperty ("height"), "Should still have height");

        // Remove last property
        removeProperty (obj, "height");
        expect (! obj.hasProperty ("height"), "Should not have height after removal");

        auto names = getAllPropertyNames (obj);
        expectEquals (names.size(), 0, "Should have no properties left");

        // Removing non-existent property should not crash
        removeProperty (obj, "nonexistent");
        expectEquals (names.size(), 0, "Should still have no properties");

        // Removing from non-object should not crash
        juce::var primitive = 42;
        removeProperty (primitive, "test");
        expectEquals (int (primitive), 42, "Primitive should be unchanged");
    }

    void testSetJSONPointerAutoCreate()
    {
        beginTest ("Set JSON Pointer - Auto Create Objects");

        juce::var obj = jsonObject();

        // Setting a nested property should auto-create intermediate objects
        bool result = setJSONPointer (obj, "/user/name", "John");
        expect (result, "Should successfully set nested property");
        expect (obj.hasProperty ("user"), "Should auto-create user object");
        expect (obj["user"].isObject(), "User should be an object");
        expect (obj["user"]["name"].toString() == juce::String ("John"), "Should set nested value");

        // Setting deeply nested property should create multiple levels
        result = setJSONPointer (obj, "/user/address/city", "London");
        expect (result, "Should successfully set deeply nested property");
        expect (obj["user"].hasProperty ("address"), "Should auto-create address object");
        expect (obj["user"]["address"].isObject(), "Address should be an object");
        expect (obj["user"]["address"]["city"].toString() == juce::String ("London"), "Should set deeply nested value");

        // Original properties should be preserved
        expect (obj["user"]["name"].toString() == juce::String ("John"), "Should preserve existing properties");

        // Setting another branch
        result = setJSONPointer (obj, "/config/width", 800);
        expect (result, "Should create separate branch");
        expect (obj.hasProperty ("config"), "Should have config object");
        expectEquals (int (obj["config"]["width"]), 800, "Should set config value");

        // Existing user branch should be unaffected
        expect (obj.hasProperty ("user"), "User branch should still exist");
        expect (obj["user"]["name"].toString() == juce::String ("John"), "User data should be preserved");
    }

    void testHasJSONPointer()
    {
        beginTest ("Has JSON Pointer");

        juce::var obj = jsonObject();
        obj.getDynamicObject()->setProperty ("width", 100);
        obj.getDynamicObject()->setProperty ("height", 200);

        // Test existing properties
        expect (hasJSONPointer (obj, "/width"), "Should find existing width");
        expect (hasJSONPointer (obj, "/height"), "Should find existing height");

        // Test non-existent properties
        expect (! hasJSONPointer (obj, "/depth"), "Should not find non-existent property");
        expect (! hasJSONPointer (obj, "/name"), "Should not find non-existent name");

        // Test nested properties
        setJSONPointer (obj, "/user/name", "John");
        setJSONPointer (obj, "/user/age", 30);

        expect (hasJSONPointer (obj, "/user"), "Should find user object");
        expect (hasJSONPointer (obj, "/user/name"), "Should find nested name");
        expect (hasJSONPointer (obj, "/user/age"), "Should find nested age");
        expect (! hasJSONPointer (obj, "/user/email"), "Should not find non-existent nested property");

        // Test deeply nested
        setJSONPointer (obj, "/user/address/city", "London");
        expect (hasJSONPointer (obj, "/user/address"), "Should find address object");
        expect (hasJSONPointer (obj, "/user/address/city"), "Should find deeply nested city");
        expect (! hasJSONPointer (obj, "/user/address/country"), "Should not find non-existent deep property");

        // Test arrays
        juce::var arr;
        arr.append (10);
        arr.append (20);
        arr.append (30);

        expect (hasJSONPointer (arr, "/0"), "Should find first array element");
        expect (hasJSONPointer (arr, "/1"), "Should find second array element");
        expect (hasJSONPointer (arr, "/2"), "Should find third array element");
        expect (! hasJSONPointer (arr, "/3"), "Should not find out of bounds element");
        expect (! hasJSONPointer (arr, "/10"), "Should not find far out of bounds element");

        // Test empty pointer
        expect (! hasJSONPointer (obj, ""), "Empty pointer should return false");

        // Test with null/void values
        obj.getDynamicObject()->setProperty ("nullValue", juce::var());
        expect (hasJSONPointer (obj, "/nullValue"), "Should find property even if value is void");

        // Distinguish between "doesn't exist" and "exists but is null"
        auto value = getJSONPointer (obj, "/nullValue", -999);
        expect (value.isVoid(), "Null value should be void");
        expect (hasJSONPointer (obj, "/nullValue"), "But pointer should exist");

        auto missingValue = getJSONPointer (obj, "/missing", -999);
        expectEquals (int (missingValue), -999, "Missing should return default");
        expect (! hasJSONPointer (obj, "/missing"), "And pointer should not exist");
    }
};

static VariantHelpersTests variantHelpersTests;

#endif

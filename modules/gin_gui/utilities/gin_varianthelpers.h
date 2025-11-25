#pragma once

/**
    @file gin_varianthelpers.h
    Utility functions for working with JUCE var objects and their properties.
    Provides convenient accessors for extracting property names, values, and sets
    from var objects that contain DynamicObject data.
*/

/**
    Creates an empty JSON object (DynamicObject wrapped in a var).

    This is a convenience function that's cleaner than writing
    `juce::var (new juce::DynamicObject())` repeatedly.

    @return  A new empty var containing a DynamicObject

    @code
    auto config = jsonObject();
    setProperty (config, "width", 800);
    setProperty (config, "height", 600);
    @endcode
*/
inline juce::var jsonObject()
{
    return juce::var (new juce::DynamicObject());
}

/**
    Convert a var to a std::string.

    This is a convenience function equivalent to `v.toString().toStdString()`.

    @param v  The var to convert
    @return   The var's string representation as a std::string

    @code
    juce::var name = "John";
    std::string stdName = toStdString (name);  // "John"

    juce::var num = 42;
    std::string stdNum = toStdString (num);    // "42"
    @endcode
*/
inline std::string toStdString (const juce::var& v)
{
    return v.toString().toStdString();
}

/**
    Get all property names from a var object.

    If the var contains a DynamicObject, this returns all property names.
    If the var is not a DynamicObject, returns an empty array.

    @param v  The var object to extract property names from
    @return   Array of property name identifiers

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);
    setProperty (obj, "height", 200);
    auto names = getAllPropertyNames (obj);  // Returns ["width", "height"]
    @endcode
*/
inline static juce::Array<juce::Identifier> getAllPropertyNames (const juce::var& v)
{
    juce::Array<juce::Identifier> names;

    if (auto obj = v.getDynamicObject())
    {
        for (const auto& property : obj->getProperties())
            names.add (property.name);
    }

    return names;
}

/**
    Get all property values from a var object.

    If the var is an Array, returns the array contents directly.
    If the var contains a DynamicObject, returns all property values (ignoring keys).
    Otherwise, returns an empty array.

    @param v  The var object to extract property values from
    @return   Array of property values

    @code
    // With DynamicObject:
    auto obj = jsonObject();
    setProperty (obj, "width", 100);
    setProperty (obj, "height", 200);
    auto values = getAllProperties (obj);  // Returns [100, 200]

    // With Array:
    juce::var arr;
    arr.append (10);
    arr.append (20);
    auto values = getAllProperties (arr);  // Returns [10, 20]
    @endcode
*/
inline static juce::Array<juce::var> getAllProperties (const juce::var& v)
{
    juce::Array<juce::var> properties;

    if (v.isArray())
    {
        return *v.getArray();
    }
    else if (auto obj = v.getDynamicObject())
    {
        for (const auto& property : obj->getProperties())
            properties.add (property.value);
    }

    return properties;
}

/**
    Simple key-value pair structure for representing a property.
    Used by getPropertySet to return both property names and values together.
*/
struct Property
{
    Property() = default;

    /**
        Construct a Property with a key and value.
        @param k  Property key/name
        @param v  Property value
    */
    Property (const juce::String& k, const juce::var v) : key (k), value (v) {}

    juce::String key;   ///< Property name/key
    juce::var value;    ///< Property value
};

/**
    Get all properties as key-value pairs from a var object.

    If the var contains a DynamicObject, returns all properties as Property objects
    containing both the key (name) and value.
    If the var is not a DynamicObject, returns an empty array.

    @param v  The var object to extract properties from
    @return   Array of Property objects containing key-value pairs

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);
    setProperty (obj, "height", 200);
    auto props = getPropertySet (obj);
    // props[0].key = "width", props[0].value = 100
    // props[1].key = "height", props[1].value = 200
    for (const auto& prop : props)
        DBG (prop.key + " = " + prop.value.toString());
    @endcode
*/
inline static juce::Array<Property> getPropertySet (const juce::var& v)
{
    juce::Array<Property> properties;

    if (auto obj = v.getDynamicObject())
    {
        for (const auto& property : obj->getProperties())
            properties.add ({property.name.toString(), property.value});
    }

    return properties;
}

/**
    Get a property value with a default fallback.

    Returns the property value if it exists, otherwise returns the default value.
    This is a safer alternative to direct property access when the property might not exist.

    @param v             The var object to get the property from
    @param i             Property name identifier
    @param defaultValue  Value to return if property doesn't exist
    @return              Property value if it exists, otherwise defaultValue

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);

    auto w = getPropertyWithDefault (obj, "width", 50);   // Returns 100
    auto h = getPropertyWithDefault (obj, "height", 50);  // Returns 50 (default)
    @endcode
*/
inline juce::var getPropertyWithDefault (const juce::var& v, const juce::Identifier& i, const juce::var& defaultValue)
{
    if (v.hasProperty (i))
        return v[i];

    return defaultValue;
}

/**
    Set a property on a var object.

    If the var contains a DynamicObject, sets the property on it.
    If the var is not a DynamicObject, this function does nothing.

    @param v      The var object to set the property on (passed by reference)
    @param i      Property name identifier
    @param value  Value to set for the property

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);
    setProperty (obj, "height", 200);
    // obj now has properties: width=100, height=200
    @endcode
*/
inline void setProperty ( juce::var& v, const juce::Identifier& i, const juce::var& value )
{
    if (auto obj = v.getDynamicObject())
        obj->setProperty ( i, value);
}

/**
    Remove a property from a var object.

    If the var contains a DynamicObject, removes the specified property from it.
    If the var is not a DynamicObject, this function does nothing.

    @param v  The var object to remove the property from (passed by reference)
    @param i  Property name identifier to remove

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);
    setProperty (obj, "height", 200);

    removeProperty (obj, "width");
    // obj now only has height property
    @endcode
*/
inline void removeProperty (juce::var& v, const juce::Identifier& i)
{
    if (auto obj = v.getDynamicObject())
        obj->removeProperty (i);
}

/** Given a JSON array/object 'v', a string representing a JSON pointer,
    and a new property value 'newValue', updates 'v' where the
    property or array index referenced by the pointer has been set to 'newValue'.

    If intermediate objects in the path don't exist, they will be automatically
    created as empty DynamicObjects. However, intermediate arrays are NOT automatically
    created - the array must already exist.

    If the pointer cannot be followed due to referencing missing array indices,
    this returns false.

    @param v         The var object to modify (passed by reference)
    @param pointer   JSON Pointer string (e.g., "/user/name")
    @param newValue  The value to set at the pointer location
    @return          true if successful, false if pointer path is invalid

    @code
    juce::var obj = jsonObject();

    // Simple property
    setJSONPointer (obj, "/name", "John");

    // Nested - automatically creates "user" object if it doesn't exist
    setJSONPointer (obj, "/user/age", 30);
    setJSONPointer (obj, "/user/address/city", "London");  // Creates address too
    @endcode

    For more details, check the JSON Pointer RFC 6901:
    https://datatracker.ietf.org/doc/html/rfc6901
*/
bool setJSONPointer (juce::var& v, juce::String pointer, const juce::var& newValue);

/** Given a JSON array/object 'v', a string representing a JSON pointer,
    returns the value of the property or array index referenced by the pointer
    If the pointer cannot be followed, due to referencing missing array indices
    or fields, then this returns defaultValue.
    For more details, check the JSON Pointer RFC 6901:
    https://datatracker.ietf.org/doc/html/rfc6901
*/
juce::var getJSONPointer (const juce::var& v, juce::String pointer, const juce::var& defaultValue);

/** Check if a JSON Pointer path exists in a var.

    Given a JSON array/object 'v' and a JSON pointer string, returns true if
    the property or array index referenced by the pointer exists.

    This is useful for checking existence before calling getJSONPointer to
    distinguish between "property doesn't exist" and "property exists but is null/void".

    @param v        The var object to check
    @param pointer  JSON Pointer string (e.g., "/user/name")
    @return         true if the pointer path exists, false otherwise

    @code
    auto obj = jsonObject();
    setProperty (obj, "width", 100);

    hasJSONPointer (obj, "/width");   // true
    hasJSONPointer (obj, "/height");  // false

    // Check nested paths
    setJSONPointer (obj, "/user/name", "John");
    hasJSONPointer (obj, "/user/name");  // true
    hasJSONPointer (obj, "/user/age");   // false
    @endcode

    For more details, check the JSON Pointer RFC 6901:
    https://datatracker.ietf.org/doc/html/rfc6901
*/
bool hasJSONPointer (const juce::var& v, juce::String pointer);

/**
    Remove C-style and C++-style comments from a JSON string.

    This function strips both line comments and block comments from JSON text,
    allowing you to use commented JSON files. Comments within string literals are preserved.

    Supported comment styles:
    - Line comments (until end of line)
    - Block comments (can span multiple lines)

    String handling:
    - Comments inside quoted strings are NOT removed
    - Escaped quotes (\") are properly handled
    - Backslashes within strings are preserved

    @param input  The JSON string potentially containing comments
    @return       The JSON string with all comments removed

    @note This allows you to use more readable, documented JSON configuration files
          while still being able to parse them with standard JSON parsers.

    @see https://jsonc.org/ for more about JSON with Comments (JSONC) format
*/
juce::String removeJsonComments (const juce::String& input);

}  // namespace gin

//==============================================================================
namespace juce
{

/** Iterator for a var.
    You shouldn't ever need to use this class directly - it's used internally by begin()
    and end() to allow range-based-for loops on a var.
*/
struct VarIterator
{
    struct NamedValue
    {
        juce::var name;
        juce::var value;
    };

    VarIterator (const juce::var&, bool isEnd);
    VarIterator& operator++();

    bool operator== (const VarIterator&) const;
    bool operator!= (const VarIterator&) const;
    NamedValue operator*() const;

    using difference_type    = std::ptrdiff_t;
    using value_type         = NamedValue;
    using reference          = NamedValue&;
    using pointer            = NamedValue*;
    using iterator_category  = std::forward_iterator_tag;

private:
    const juce::var& v;
    int index = 0;
    const void* itr = nullptr;
};

VarIterator begin (const juce::var&);
VarIterator end (const juce::var&);

}  // namespace juce

namespace gin
{

#pragma once

/**
    @file gin_varianthelpers.h
    Utility functions for working with JUCE var objects and their properties.
    Provides convenient accessors for extracting property names, values, and sets
    from var objects that contain DynamicObject data.
*/

/**
    Get all property names from a var object.

    If the var contains a DynamicObject, this returns all property names.
    If the var is not a DynamicObject, returns an empty array.

    @param v  The var object to extract property names from
    @return   Array of property name identifiers

    @code
    juce::var obj = new juce::DynamicObject();
    obj.getDynamicObject()->setProperty ("width", 100);
    obj.getDynamicObject()->setProperty ("height", 200);
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
    juce::var obj = new juce::DynamicObject();
    obj.getDynamicObject()->setProperty ("width", 100);
    obj.getDynamicObject()->setProperty ("height", 200);
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
    juce::var obj = new juce::DynamicObject();
    obj.getDynamicObject()->setProperty ("width", 100);
    obj.getDynamicObject()->setProperty ("height", 200);
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
    juce::var obj = new juce::DynamicObject();
    obj.getDynamicObject()->setProperty ("width", 100);

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
    juce::var obj = new juce::DynamicObject();
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

/** Given a JSON array/object 'v', a string representing a JSON pointer,
    and a new property value 'newValue', updates 'v' where the
    property or array index referenced by the pointer has been set to 'newValue'.
    If the pointer cannot be followed, due to referencing missing array indices
    or fields, then this returns false.
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

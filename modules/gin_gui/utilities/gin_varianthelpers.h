#pragma once

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

struct Property
{
    Property() = default;
    Property (const juce::String& k, const juce::var v) : key (k), value (v) {}

    juce::String key;
    juce::var value;
};

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

inline juce::var getPropertyWithDefault (const juce::var& v, const juce::Identifier& i, const juce::var& defaultValue)
{
    if (v.hasProperty (i))
        return v[i];

    return defaultValue;
}

inline void setProperty ( juce::var& v, const juce::Identifier& i, const juce::var& value )
{
    if (auto obj = v.getDynamicObject())
        obj->setProperty ( i, value);
}

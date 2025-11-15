/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
static juce::var toVar (const juce::ValueTree& v)
{
    auto obj = new juce::DynamicObject();

    obj->setProperty ("_name", v.getType().toString());

    juce::Array<juce::var> children;

    for (auto c : v)
        children.add (toVar (c));

    if (children.size() > 0)
        obj->setProperty ("_children", children);

    for (int i = 0; i < v.getNumProperties(); i++)
    {
        auto name = v.getPropertyName (i).toString();
        auto val  = v.getProperty (name, {});

        if (auto mb = val.getBinaryData())
        {
            obj->setProperty ("base64:" + name, mb->toBase64Encoding());
        }
        else
        {
            // These types can't be stored as JSON!
            jassert (! val.isObject());
            jassert (! val.isMethod());
            jassert (! val.isArray());

            obj->setProperty (name, val.toString());
        }
    }

    return juce::var (obj);
}

juce::String valueTreeToJSON (const juce::ValueTree& v)
{
    auto obj = toVar (v);
    return juce::JSON::toString (obj);
}

//==============================================================================
static juce::ValueTree fromVar (const juce::var& obj)
{
    if (auto dobj = obj.getDynamicObject())
    {
        juce::ValueTree v (dobj->getProperty ("_name").toString());

        auto c = dobj->getProperty ("_children");
        if (c.isArray())
            for (auto& child : *c.getArray())
                v.addChild (fromVar (child), -1, nullptr);

        auto properties = dobj->getProperties();
        for (auto itr : properties)
        {
            auto name = itr.name.toString();
            if (name == "_name" || name == "_children") continue;

            if (name.startsWith ("base64:"))
            {
                juce::MemoryBlock mb;
                if (mb.fromBase64Encoding (itr.value.toString()))
                    v.setProperty (name.substring (7), juce::var (mb), nullptr);
            }
            else
            {
                v.setProperty (name, juce::var (itr.value), nullptr);
            }
        }

        return v;
    }
    return {};
}

juce::ValueTree valueTreeFromJSON (const juce::String& jsonText)
{
    juce::var obj = juce::JSON::parse (jsonText);
    if (obj.isObject())
        return fromVar (obj);
    return {};
}

//==============================================================================
#if JUCE_UNIT_TESTS

class ValueTreeUtilitiesTests : public juce::UnitTest
{
public:
    ValueTreeUtilitiesTests() : juce::UnitTest ("ValueTree Utilities", "gin") {}

    void runTest() override
    {
        testBasicConversion();
        testWithProperties();
        testWithChildren();
        testRoundTrip();
        testBinaryData();
    }

private:
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

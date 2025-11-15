/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if JUCE_UNIT_TESTS

class PlistTests : public juce::UnitTest
{
public:
    PlistTests() : juce::UnitTest ("Plist Parser", "gin") {}

    void runTest() override
    {
        testParseString();
        testParseInteger();
        testParseReal();
        testParseBoolean();
        testParseDate();
        testParseData();
        testParseArray();
        testParseDictionary();
        testParseNestedStructures();
        testParseFromFile();
        testParseEmpty();
    }

private:
    void testParseString()
    {
        beginTest ("Parse String");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>testString</key>
    <string>Hello, World!</string>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        expect (result.isObject(), "Result should be an object");

        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");
        expect (obj->hasProperty ("testString"), "Should have testString property");
        expectEquals (obj->getProperty ("testString").toString(), juce::String ("Hello, World!"),
                     "String value should match");
    }

    void testParseInteger()
    {
        beginTest ("Parse Integer");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>positiveInt</key>
    <integer>42</integer>
    <key>negativeInt</key>
    <integer>-100</integer>
    <key>zero</key>
    <integer>0</integer>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        expectEquals ((int) obj->getProperty ("positiveInt"), 42, "Positive integer should match");
        expectEquals ((int) obj->getProperty ("negativeInt"), -100, "Negative integer should match");
        expectEquals ((int) obj->getProperty ("zero"), 0, "Zero should match");
    }

    void testParseReal()
    {
        beginTest ("Parse Real");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>pi</key>
    <real>3.14159</real>
    <key>negative</key>
    <real>-2.5</real>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        expectWithinAbsoluteError ((double) obj->getProperty ("pi"), 3.14159, 0.00001,
                                  "Pi value should match");
        expectWithinAbsoluteError ((double) obj->getProperty ("negative"), -2.5, 0.00001,
                                  "Negative real should match");
    }

    void testParseBoolean()
    {
        beginTest ("Parse Boolean");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>isTrue</key>
    <true/>
    <key>isFalse</key>
    <false/>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        expect ((bool) obj->getProperty ("isTrue") == true, "True value should be true");
        expect ((bool) obj->getProperty ("isFalse") == false, "False value should be false");
    }

    void testParseDate()
    {
        beginTest ("Parse Date");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>timestamp</key>
    <date>2024-01-15T12:30:00Z</date>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        expectEquals (obj->getProperty ("timestamp").toString(), juce::String ("2024-01-15T12:30:00Z"),
                     "Date string should match");
    }

    void testParseData()
    {
        beginTest ("Parse Data");

        // Base64 encoded "Hello"
        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>binaryData</key>
    <data>SGVsbG8=</data>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        auto data = obj->getProperty ("binaryData");
        expect (data.isBinaryData(), "Should be binary data");

        auto mb = data.getBinaryData();
        expect (mb != nullptr, "Should have memory block");
        expectEquals ((int) mb->getSize(), 5, "Data size should match");
        expect (mb->toString() == "Hello", "Decoded data should match");
    }

    void testParseArray()
    {
        beginTest ("Parse Array");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>numbers</key>
    <array>
        <integer>1</integer>
        <integer>2</integer>
        <integer>3</integer>
    </array>
    <key>strings</key>
    <array>
        <string>one</string>
        <string>two</string>
    </array>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        auto numbers = obj->getProperty ("numbers");
        expect (numbers.isArray(), "Numbers should be array");
        auto numArray = numbers.getArray();
        expectEquals (numArray->size(), 3, "Numbers array should have 3 elements");
        expectEquals ((int) (*numArray)[0], 1, "First number should be 1");
        expectEquals ((int) (*numArray)[1], 2, "Second number should be 2");
        expectEquals ((int) (*numArray)[2], 3, "Third number should be 3");

        auto strings = obj->getProperty ("strings");
        expect (strings.isArray(), "Strings should be array");
        auto strArray = strings.getArray();
        expectEquals (strArray->size(), 2, "Strings array should have 2 elements");
        expectEquals ((*strArray)[0].toString(), juce::String ("one"), "First string should match");
        expectEquals ((*strArray)[1].toString(), juce::String ("two"), "Second string should match");
    }

    void testParseDictionary()
    {
        beginTest ("Parse Dictionary");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>person</key>
    <dict>
        <key>name</key>
        <string>John Doe</string>
        <key>age</key>
        <integer>30</integer>
    </dict>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        auto person = obj->getProperty ("person");
        expect (person.isObject(), "Person should be object");

        auto personObj = person.getDynamicObject();
        expect (personObj != nullptr, "Should have person object");
        expectEquals (personObj->getProperty ("name").toString(), juce::String ("John Doe"),
                     "Name should match");
        expectEquals ((int) personObj->getProperty ("age"), 30, "Age should match");
    }

    void testParseNestedStructures()
    {
        beginTest ("Parse Nested Structures");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>config</key>
    <dict>
        <key>servers</key>
        <array>
            <dict>
                <key>host</key>
                <string>server1.com</string>
                <key>port</key>
                <integer>8080</integer>
            </dict>
            <dict>
                <key>host</key>
                <string>server2.com</string>
                <key>port</key>
                <integer>9090</integer>
            </dict>
        </array>
        <key>enabled</key>
        <true/>
    </dict>
</dict>
</plist>)";

        auto result = parsePlist (plist);
        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");

        auto config = obj->getProperty ("config").getDynamicObject();
        expect (config != nullptr, "Should have config object");

        auto servers = config->getProperty ("servers");
        expect (servers.isArray(), "Servers should be array");
        auto serverArray = servers.getArray();
        expectEquals (serverArray->size(), 2, "Should have 2 servers");

        auto server1 = (*serverArray)[0].getDynamicObject();
        expect (server1 != nullptr, "Should have server1 object");
        expectEquals (server1->getProperty ("host").toString(), juce::String ("server1.com"),
                     "Server1 host should match");
        expectEquals ((int) server1->getProperty ("port"), 8080, "Server1 port should match");

        expect ((bool) config->getProperty ("enabled") == true, "Enabled should be true");
    }

    void testParseFromFile()
    {
        beginTest ("Parse from File");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".plist");

        juce::String plist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
    <key>fromFile</key>
    <string>test value</string>
</dict>
</plist>)";

        tempFile.replaceWithText (plist);

        auto result = parsePlist (tempFile);
        expect (result.isObject(), "Result should be an object");

        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");
        expectEquals (obj->getProperty ("fromFile").toString(), juce::String ("test value"),
                     "Value from file should match");

        tempFile.deleteFile();
    }

    void testParseEmpty()
    {
        beginTest ("Parse Empty");

        // Empty plist
        juce::String emptyPlist = R"(<?xml version="1.0" encoding="UTF-8"?>
<plist version="1.0">
<dict>
</dict>
</plist>)";

        auto result = parsePlist (emptyPlist);
        expect (result.isObject(), "Empty plist should return object");

        auto obj = result.getDynamicObject();
        expect (obj != nullptr, "Should have dynamic object");
        expectEquals (obj->getProperties().size(), 0, "Should have no properties");

        // Invalid XML
        auto invalidResult = parsePlist ("not xml at all");
        expect (invalidResult.isVoid(), "Invalid XML should return void");
    }
};

static PlistTests plistTests;

#endif

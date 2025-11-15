/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

#if _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4310)
 #pragma warning (disable: 4100)
#endif

static void toData (juce::OutputStream& os, const juce::var& obj)
{
    if (obj.isVoid())
    {
        os.writeByte (char (0xc0));
    }
    else if (obj.isInt() || obj.isInt64())
    {
        juce::int64 v = (juce::int64) obj;
        if (v >= 0)
        {
            if (v <= 127)
            {
                os.writeByte (char (v));
            }
            else if (v <= 255)
            {
                os.writeByte (char (0xcc));
                os.writeByte (char ((unsigned char) v));
            }
            else if (v <= 65535)
            {
                os.writeByte (char (0xcd));
                os.writeShortBigEndian (short ((unsigned short) v));
            }
            else if (v <= 4294967295)
            {
                os.writeByte (char (0xce));
                os.writeIntBigEndian (int ((unsigned int) v));
            }
            else
            {
                os.writeByte (char (0xcf));
                os.writeInt64BigEndian (juce::int64 (v));
            }
        }
        else
        {
            if (v >= -7)
            {
                os.writeByte (char (0xe0 | - char (-v)));
            }
            else if (v >= -128)
            {
                os.writeByte (char (0xd0));
                os.writeByte (char (v));
            }
            else if (v >= 32768)
            {
                os.writeByte (char (0xd1));
                os.writeShortBigEndian (short (v));
            }
            else if (v >= 2147483648)
            {
                os.writeByte (char (0xd2));
                os.writeIntBigEndian (int (v));
            }
            else
            {
                os.writeByte (char (0xd3));
                os.writeInt64BigEndian (v);
            }
        }
    }
    else if (obj.isBool())
    {
        if ((bool) obj)
            os.writeByte (char (0xc3));
        else
            os.writeByte (char (0xc2));
    }
    else if (obj.isDouble())
    {
        os.writeByte (char (0xcb));
        os.writeDoubleBigEndian (obj);
    }
    else if (obj.isString())
    {
        auto str = obj.toString();
        auto s = str.toRawUTF8();
        size_t n = str.getNumBytesAsUTF8();

        if (n <= 31)
        {
            os.writeByte (char (0xa0 | n));
            os.write (s, n);
        }
        else if (n <= 255)
        {
            os.writeByte (char (0xd9));
            os.writeByte (char (n));
            os.write (s, n);
        }
        else if (n <= 65535)
        {
            os.writeByte (char (0xda));
            os.writeShortBigEndian (short (n));
            os.write (s, n);
        }
        else
        {
            os.writeByte (char (0xdb));
            os.writeIntBigEndian (int (n));
            os.write (s, n);
        }
    }
    else if (obj.isObject() && obj.getDynamicObject() != nullptr)
    {
        auto& dobj = *obj.getDynamicObject();
        auto& names = dobj.getProperties();

        int n = names.size();

        if (n <= 15)
        {
            os.writeByte (char (0x80 | n));
        }
        else if (n <= 65535)
        {
            os.writeByte (char (0xde));
            os.writeShortBigEndian (short (n));
        }
        else
        {
            os.writeByte (char (0xdf));
            os.writeIntBigEndian (n);
        }

        for (auto& itm : names)
        {
            toData (os, juce::var (itm.name.toString()));
            toData (os, itm.value);
        }
    }
    else if (obj.isArray())
    {
        auto& arr = *obj.getArray();

        int n = arr.size();

        if (n <= 15)
        {
           os.writeByte (char (0x90 | n));
        }
        else if (n <= 65535)
        {
            os.writeByte (char (0xdc));
            os.writeShortBigEndian (short (n));
        }
        else
        {
            os.writeByte (char (0xdc));
            os.writeIntBigEndian (n);
        }

        for (auto& a : arr)
            toData (os, a);
    }
    else if (obj.isBinaryData())
    {
        if (auto bd = obj.getBinaryData())
        {
            void* s = bd->getData();
            size_t n = bd->getSize();

            if (n <= 255)
            {
                os.writeByte (char (0xc4));
                os.writeByte (char (n));
                os.write (s, n);
            }
            else if (n <= 65535)
            {
                os.writeByte (char (0xc5));
                os.writeShortBigEndian (short (n));
                os.write (s, n);
            }
            else
            {
                os.writeByte (char (0xc6));
                os.writeIntBigEndian (short (n));
                os.write (s, n);
            }
        }
    }
    else
    {
        jassertfalse;
    }
}

static juce::var fromData (juce::InputStream& is);

static juce::var fromArray (juce::InputStream& is, int n)
{
    juce::Array<juce::var> res;

    for (int i = 0; i < n; i++)
        res.add (fromData (is));

    return res;
}

static juce::var fromMap (juce::InputStream& is, int n)
{
    auto obj = new juce::DynamicObject();

    for (int i = 0; i < n; i++)
    {
        juce::var k = fromData (is);
        juce::var v = fromData (is);

        auto ident = k.toString();
        if (ident.isNotEmpty())
            obj->setProperty (ident, v);
    }

    return juce::var (obj);
}

static juce::var fromString (juce::InputStream& is, int n)
{
    juce::MemoryBlock mb;
    is.readIntoMemoryBlock (mb, n);

    return juce::String::fromUTF8 ((const char*)mb.getData(), int (mb.getSize()));
}

static juce::var fromData (juce::InputStream& is)
{
    uint8_t d = uint8_t (is.readByte());

    if ((d & 0x80) == 0x00)
    {
        return (int) d;
    }
    else if ((d & 0xf0) == 0x80)
    {
        return fromMap (is, d & 0x0f);
    }
    else if ((d & 0xf0) == 0x90)
    {
        return fromArray (is, d & 0x0f);
    }
    else if ((d & 0xe0) == 0xa0)
    {
        return fromString (is, d & 0x1f);
    }
    else if (d == 0xc0)
    {
        return {};
    }
    else if (d == 0xc1)
    {
        jassertfalse;
        return {};
    }
    else if (d == 0xc2)
    {
        return false;
    }
    else if (d == 0xc3)
    {
        return true;
    }
    else if (d == 0xc4)
    {
        uint8_t n = uint8_t (is.readByte());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n);
        return mb;
    }
    else if (d == 0xc5)
    {
        uint16_t n = uint16_t (is.readShortBigEndian());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n);
        return mb;
    }
    else if (d == 0xc6)
    {
        uint32_t n = uint32_t (is.readIntBigEndian());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n);
        return mb;
    }
    else if (d == 0xc7)
    {
        uint8_t n = uint8_t (is.readByte());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n + 1);
        return mb;
    }
    else if (d == 0xc8)
    {
        uint16_t n = uint16_t (is.readShortBigEndian());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n + 1);
        return mb;
    }
    else if (d == 0xc9)
    {
        uint32_t n = uint32_t (is.readIntBigEndian());
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, n + 1);
        return mb;
    }
    else if (d == 0xca)
    {
        return is.readFloatBigEndian();
    }
    else if (d == 0xcb)
    {
        return is.readDoubleBigEndian();
    }
    else if (d == 0xcc)
    {
        return int (uint8_t (is.readByte()));
    }
    else if (d == 0xcd)
    {
        return int (uint16_t (is.readShortBigEndian()));
    }
    else if (d == 0xce)
    {
        return int (uint32_t (is.readIntBigEndian()));
    }
    else if (d == 0xcf)
    {
        return int (uint64_t (is.readInt64BigEndian()));
    }
    else if (d == 0xd4)
    {
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, 1 + 1);
        return mb;
    }
    else if (d == 0xd5)
    {
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, 1 + 2);
        return mb;
    }
    else if (d == 0xd6)
    {
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, 1 + 4);
        return mb;
    }
    else if (d == 0xd7)
    {
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, 1 + 8);
        return mb;
    }
    else if (d == 0xd8)
    {
        juce::MemoryBlock mb;
        is.readIntoMemoryBlock (mb, 1 + 16);
        return mb;
    }
    else if (d == 0xd9)
    {
        uint8_t n = uint8_t (is.readByte());
        return fromString (is, n);
    }
    else if (d == 0xda)
    {
        uint16_t n = uint16_t (is.readShortBigEndian());
        return fromString (is, n);
    }
    else if (d == 0xdb)
    {
        uint32_t n = uint32_t (is.readIntBigEndian());
        return fromString (is, int (n));
    }
    else if (d == 0xdc)
    {
        uint16_t n = uint16_t (is.readShortBigEndian());
        return fromArray (is, n);
    }
    else if (d == 0xdd)
    {
        uint32_t n = uint32_t (is.readIntBigEndian());
        return fromArray (is, int (n));
    }
    else if (d == 0xde)
    {
        uint16_t n = uint16_t (is.readShortBigEndian());
        return fromArray (is, n);
    }
    else if (d == 0xdf)
    {
        uint32_t n = uint32_t (is.readIntBigEndian());
        return fromArray (is, int (n));
    }
    else if ((d & 0xe0) == 0xe0)
    {
        return -(d & 0x1f);
    }

    jassertfalse;
    return {};
}
//==============================================================================

juce::MemoryBlock MessagePack::toMessagePack (const juce::var& obj)
{
    juce::MemoryBlock data;

    {
        juce::MemoryOutputStream os (data, false);
        toData (os, obj);
    }

    return data;
}

juce::var MessagePack::parse (const juce::MemoryBlock& data)
{
    juce::MemoryInputStream is (data, false);
    return fromData (is);
}

#if _MSC_VER
 #pragma warning (pop)
#endif

//==============================================================================
#if JUCE_UNIT_TESTS

class MessagePackTests : public juce::UnitTest
{
public:
    MessagePackTests() : juce::UnitTest ("MessagePack", "gin") {}

    void runTest() override
    {
        testIntegers();
        testNegativeIntegers();
        testBooleans();
        testFloatingPoint();
        testStrings();
        testArrays();
        testObjects();
        testNullValues();
        testRoundTrip();
    }

private:
    void testIntegers()
    {
        beginTest ("Integers");

        // Small positive integers
        juce::var v1 = 0;
        auto data1 = MessagePack::toMessagePack (v1);
        auto parsed1 = MessagePack::parse (data1);
        expectEquals (int (parsed1), 0, "Should handle 0");

        juce::var v2 = 127;
        auto data2 = MessagePack::toMessagePack (v2);
        auto parsed2 = MessagePack::parse (data2);
        expectEquals (int (parsed2), 127, "Should handle small positive int");

        // Larger integers
        juce::var v3 = 255;
        auto data3 = MessagePack::toMessagePack (v3);
        auto parsed3 = MessagePack::parse (data3);
        expectEquals (int (parsed3), 255, "Should handle uint8");

        juce::var v4 = 65535;
        auto data4 = MessagePack::toMessagePack (v4);
        auto parsed4 = MessagePack::parse (data4);
        expectEquals (int (parsed4), 65535, "Should handle uint16");

        juce::var v5 = juce::int64 (1000000);
        auto data5 = MessagePack::toMessagePack (v5);
        auto parsed5 = MessagePack::parse (data5);
        expectEquals (juce::int64 (parsed5), juce::int64 (1000000), "Should handle large int");
    }

    void testNegativeIntegers()
    {
        beginTest ("Negative Integers");

        juce::var v1 = -1;
        auto data1 = MessagePack::toMessagePack (v1);
        auto parsed1 = MessagePack::parse (data1);
        expectEquals (int (parsed1), -1, "Should handle -1");

        juce::var v2 = -7;
        auto data2 = MessagePack::toMessagePack (v2);
        auto parsed2 = MessagePack::parse (data2);
        expectEquals (int (parsed2), -7, "Should handle small negative");

        juce::var v3 = -128;
        auto data3 = MessagePack::toMessagePack (v3);
        auto parsed3 = MessagePack::parse (data3);
        expectEquals (int (parsed3), -128, "Should handle int8");

        juce::var v4 = -1000;
        auto data4 = MessagePack::toMessagePack (v4);
        auto parsed4 = MessagePack::parse (data4);
        expectEquals (int (parsed4), -1000, "Should handle larger negative");
    }

    void testBooleans()
    {
        beginTest ("Booleans");

        juce::var vTrue = true;
        auto dataTrue = MessagePack::toMessagePack (vTrue);
        auto parsedTrue = MessagePack::parse (dataTrue);
        expect (bool (parsedTrue) == true, "Should handle true");

        juce::var vFalse = false;
        auto dataFalse = MessagePack::toMessagePack (vFalse);
        auto parsedFalse = MessagePack::parse (dataFalse);
        expect (bool (parsedFalse) == false, "Should handle false");
    }

    void testFloatingPoint()
    {
        beginTest ("Floating Point");

        juce::var v1 = 3.14159;
        auto data1 = MessagePack::toMessagePack (v1);
        auto parsed1 = MessagePack::parse (data1);
        expectWithinAbsoluteError (double (parsed1), 3.14159, 0.00001, "Should handle double");

        juce::var v2 = -2.71828;
        auto data2 = MessagePack::toMessagePack (v2);
        auto parsed2 = MessagePack::parse (data2);
        expectWithinAbsoluteError (double (parsed2), -2.71828, 0.00001, "Should handle negative double");

        juce::var v3 = 0.0;
        auto data3 = MessagePack::toMessagePack (v3);
        auto parsed3 = MessagePack::parse (data3);
        expectWithinAbsoluteError (double (parsed3), 0.0, 0.00001, "Should handle 0.0");
    }

    void testStrings()
    {
        beginTest ("Strings");

        // Short string
        juce::var v1 = "Hello";
        auto data1 = MessagePack::toMessagePack (v1);
        auto parsed1 = MessagePack::parse (data1);
        expect (parsed1.toString() == "Hello", "Should handle short string");

        // Empty string
        juce::var v2 = "";
        auto data2 = MessagePack::toMessagePack (v2);
        auto parsed2 = MessagePack::parse (data2);
        expect (parsed2.toString() == "", "Should handle empty string");

        // Longer string
        juce::var v3 = "This is a longer string with more characters to test larger string handling";
        auto data3 = MessagePack::toMessagePack (v3);
        auto parsed3 = MessagePack::parse (data3);
        expect (parsed3.toString() == v3.toString(), "Should handle longer string");

        // Unicode string
        juce::var v4 = "Hello 世界 🌍";
        auto data4 = MessagePack::toMessagePack (v4);
        auto parsed4 = MessagePack::parse (data4);
        expect (parsed4.toString() == v4.toString(), "Should handle Unicode");
    }

    void testArrays()
    {
        beginTest ("Arrays");

        // Empty array
        juce::Array<juce::var> arr1;
        juce::var v1 = arr1;
        auto data1 = MessagePack::toMessagePack (v1);
        auto parsed1 = MessagePack::parse (data1);
        expect (parsed1.isArray(), "Should be array");
        expect (parsed1.size() == 0, "Empty array should have size 0");

        // Simple array
        juce::Array<juce::var> arr2;
        arr2.add (1);
        arr2.add (2);
        arr2.add (3);
        juce::var v2 = arr2;
        auto data2 = MessagePack::toMessagePack (v2);
        auto parsed2 = MessagePack::parse (data2);
        expect (parsed2.isArray(), "Should be array");
        expect (parsed2.size() == 3, "Should have 3 elements");
        expectEquals (int (parsed2[0]), 1, "First element should be 1");
        expectEquals (int (parsed2[1]), 2, "Second element should be 2");
        expectEquals (int (parsed2[2]), 3, "Third element should be 3");

        // Mixed type array
        juce::Array<juce::var> arr3;
        arr3.add (42);
        arr3.add ("test");
        arr3.add (true);
        arr3.add (3.14);
        juce::var v3 = arr3;
        auto data3 = MessagePack::toMessagePack (v3);
        auto parsed3 = MessagePack::parse (data3);
        expect (parsed3.size() == 4, "Should have 4 elements");
        expectEquals (int (parsed3[0]), 42, "Should be int");
        expect (parsed3[1].toString() == "test", "Should be string");
        expect (bool (parsed3[2]) == true, "Should be bool");
        expectWithinAbsoluteError (double (parsed3[3]), 3.14, 0.01, "Should be double");
    }

    void testObjects()
    {
        beginTest ("Objects");

        juce::DynamicObject::Ptr obj = new juce::DynamicObject();
        obj->setProperty ("name", "John");
        obj->setProperty ("age", 30);
        obj->setProperty ("active", true);

        juce::var v = juce::var (obj.get());
        auto data = MessagePack::toMessagePack (v);
        auto parsed = MessagePack::parse (data);

        expect (parsed.isObject(), "Should be object");
        auto* parsedObj = parsed.getDynamicObject();
        expect (parsedObj != nullptr, "Should have dynamic object");

        expect (parsedObj->getProperty ("name").toString() == "John", "Name should match");
        expectEquals (int (parsedObj->getProperty ("age")), 30, "Age should match");
        expect (bool (parsedObj->getProperty ("active")) == true, "Active should match");
    }

    void testNullValues()
    {
        beginTest ("Null Values");

        juce::var v;
        auto data = MessagePack::toMessagePack (v);
        auto parsed = MessagePack::parse (data);
        expect (parsed.isVoid(), "Should be void/null");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        // Complex nested structure
        juce::DynamicObject::Ptr root = new juce::DynamicObject();

        juce::Array<juce::var> numbers;
        numbers.add (1);
        numbers.add (2);
        numbers.add (3);
        root->setProperty ("numbers", numbers);

        juce::DynamicObject::Ptr user = new juce::DynamicObject();
        user->setProperty ("name", "Alice");
        user->setProperty ("age", 25);
        root->setProperty ("user", juce::var (user.get()));

        root->setProperty ("active", true);
        root->setProperty ("value", 42.5);

        juce::var original = juce::var (root.get());
        auto data = MessagePack::toMessagePack (original);
        auto parsed = MessagePack::parse (data);

        expect (parsed.isObject(), "Root should be object");
        auto* parsedRoot = parsed.getDynamicObject();

        auto parsedNumbers = parsedRoot->getProperty ("numbers");
        expect (parsedNumbers.isArray(), "Numbers should be array");
        expect (parsedNumbers.size() == 3, "Should have 3 numbers");

        auto parsedUser = parsedRoot->getProperty ("user");
        expect (parsedUser.isObject(), "User should be object");
        expect (parsedUser.getDynamicObject()->getProperty ("name").toString() == "Alice", "Name should match");

        expect (bool (parsedRoot->getProperty ("active")) == true, "Active should match");
        expectWithinAbsoluteError (double (parsedRoot->getProperty ("value")), 42.5, 0.01, "Value should match");
    }
};

static MessagePackTests messagePackTests;

#endif

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

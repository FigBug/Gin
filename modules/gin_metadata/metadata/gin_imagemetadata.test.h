/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ImageMetadataTests : public juce::UnitTest
{
public:
    ImageMetadataTests() : juce::UnitTest ("Image Metadata", "gin") {}

    void runTest() override
    {
        testJpeg();
        testPngText();
        testPngCompressedText();
        testPngExif();
        testNoMetadata();
        testIptcRecords();
        testMalformedExif();
        testMalformedIptc();
    }

private:
    static constexpr const char* xml = "<x:xmpmeta xmlns:x=\"adobe:ns:meta/\">test</x:xmpmeta>";

    static juce::MemoryBlock makeExifPayload()
    {
        juce::MemoryOutputStream tiff;
        tiff.write ("Exif\0\0", 6);
        tiff.write ("II", 2);       // little endian tiff
        tiff.writeShort (42);
        tiff.writeInt (8);          // offset to IFD0
        tiff.writeShort (1);        // 1 entry
        tiff.writeShort (0x010f);   // Make
        tiff.writeShort (2);        // ascii
        tiff.writeInt (4);          // count, fits inline
        tiff.write ("Gin\0", 4);
        tiff.writeInt (0);          // no next IFD
        return tiff.getMemoryBlock();
    }

    static const ImageMetadata* findType (const juce::OwnedArray<ImageMetadata>& metadata, const juce::String& type)
    {
        for (auto* md : metadata)
            if (md->getType() == type)
                return md;

        return nullptr;
    }

    void testJpeg()
    {
        beginTest ("JPEG segments");

        juce::MemoryOutputStream jpg;
        jpg.writeByte (char (0xff)); jpg.writeByte (char (0xd8)); // SOI

        auto writeSegment = [&] (juce::uint8 marker, const void* data, size_t len)
        {
            jpg.writeByte (char (0xff));
            jpg.writeByte (char (marker));
            jpg.writeShortBigEndian (short (len + 2));
            jpg.write (data, len);
        };

        const char* comment = "A test comment";
        writeSegment (0xfe, comment, strlen (comment));

        auto exif = makeExifPayload();
        writeSegment (0xe1, exif.getData(), exif.getSize());

        juce::MemoryOutputStream xmp;
        xmp.write ("http://ns.adobe.com/xap/1.0/\0", 29);
        xmp.write (xml, strlen (xml));
        writeSegment (0xe1, xmp.getData(), xmp.getDataSize());

        juce::MemoryOutputStream iptc;
        iptc.write ("Photoshop 3.0\0", 14);
        iptc.write ("8BIM", 4);
        iptc.writeShort (0x0404);                   // IPTC resource
        iptc.writeByte (0); iptc.writeByte (0);     // empty pascal name, padded
        const char* caption = "Hello IPTC";
        const int captionLen = int (strlen (caption));
        iptc.writeIntBigEndian (5 + captionLen);    // resource size
        iptc.writeByte (0x1c); iptc.writeByte (2); iptc.writeByte (120);
        iptc.writeShortBigEndian (short (captionLen));
        iptc.write (caption, size_t (captionLen));
        writeSegment (0xed, iptc.getData(), iptc.getDataSize());

        // image data after SOS shouldn't be scanned, even if it contains marker-like bytes
        writeSegment (0xda, "\0\0", 2);
        jpg.writeByte (char (0xff)); jpg.writeByte (char (0xe1)); jpg.writeByte (0); jpg.writeByte (0);
        jpg.writeByte (char (0xff)); jpg.writeByte (char (0xd9)); // EOI

        juce::MemoryInputStream is (jpg.getData(), jpg.getDataSize(), false);
        juce::OwnedArray<ImageMetadata> metadata;
        expect (ImageMetadata::getFromImage (is, metadata), "Should find JPEG metadata");
        expectEquals (metadata.size(), 4, "Should find comment, exif, xmp and iptc");

        if (auto md = findType (metadata, "Comment"))
            expectEquals (md->getAllMetadata()["Comment"], juce::String (comment));
        else
            expect (false, "No comment metadata");

        if (auto md = findType (metadata, "EXIF"))
            expectEquals (md->getAllMetadata()["Make"], juce::String ("Gin"));
        else
            expect (false, "No exif metadata");

        if (auto md = findType (metadata, "Xmp"))
            expectEquals (md->getAllMetadata()["XMP"], juce::String (xml));
        else
            expect (false, "No xmp metadata");

        if (auto md = findType (metadata, "IPTC"))
            expectEquals (md->getAllMetadata()["Caption/Abstract"], juce::String (caption));
        else
            expect (false, "No iptc metadata");
    }

    static void writePngHeader (juce::MemoryOutputStream& png)
    {
        const juce::uint8 sig[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
        png.write (sig, 8);
    }

    static void writePngChunk (juce::MemoryOutputStream& png, const char* type, const void* data, size_t len)
    {
        png.writeIntBigEndian (int (len));
        png.write (type, 4);
        png.write (data, len);
        png.writeInt (0); // crc, not validated
    }

    static bool parsePng (const juce::MemoryOutputStream& png, juce::OwnedArray<ImageMetadata>& metadata)
    {
        juce::MemoryInputStream is (png.getData(), png.getDataSize(), false);
        return ImageMetadata::getFromImage (is, metadata);
    }

    void testPngText()
    {
        beginTest ("PNG text chunks");

        juce::MemoryOutputStream itxt;
        itxt.write ("XML:com.adobe.xmp\0", 18);
        itxt.writeByte (0); itxt.writeByte (0); // uncompressed
        itxt.writeByte (0);                     // empty language
        itxt.writeByte (0);                     // empty translated keyword
        itxt.write (xml, strlen (xml));

        {
            juce::MemoryOutputStream png;
            writePngHeader (png);
            writePngChunk (png, "iTXt", itxt.getData(), itxt.getDataSize());
            writePngChunk (png, "IEND", nullptr, 0);

            juce::OwnedArray<ImageMetadata> metadata;
            expect (parsePng (png, metadata), "Should find PNG metadata");
            expectEquals (metadata.size(), 1, "Should find xmp in iTXt");

            if (auto md = findType (metadata, "Xmp"))
                expectEquals (md->getAllMetadata()["XMP"], juce::String (xml));
            else
                expect (false, "No xmp metadata");
        }

        juce::MemoryOutputStream text;
        text.write ("XML:com.adobe.xmp\0", 18);
        text.write (xml, strlen (xml));

        {
            juce::MemoryOutputStream png;
            writePngHeader (png);
            writePngChunk (png, "tEXt", text.getData(), text.getDataSize());
            writePngChunk (png, "IEND", nullptr, 0);

            juce::OwnedArray<ImageMetadata> metadata;
            expect (parsePng (png, metadata), "Should find PNG metadata");

            if (auto md = findType (metadata, "Xmp"))
                expectEquals (md->getAllMetadata()["XMP"], juce::String (xml));
            else
                expect (false, "No xmp metadata");
        }
    }

    void testPngCompressedText()
    {
        beginTest ("PNG compressed text chunks");

        juce::MemoryOutputStream compressed;
        {
            juce::GZIPCompressorOutputStream zlib (compressed);
            zlib.write (xml, strlen (xml));
        }

        juce::MemoryOutputStream ztxt;
        ztxt.write ("XML:com.adobe.xmp\0", 18);
        ztxt.writeByte (0); // deflate
        ztxt.write (compressed.getData(), compressed.getDataSize());

        juce::MemoryOutputStream png;
        writePngHeader (png);
        writePngChunk (png, "zTXt", ztxt.getData(), ztxt.getDataSize());
        writePngChunk (png, "IEND", nullptr, 0);

        juce::OwnedArray<ImageMetadata> metadata;
        expect (parsePng (png, metadata), "Should find PNG metadata");

        if (auto md = findType (metadata, "Xmp"))
            expectEquals (md->getAllMetadata()["XMP"], juce::String (xml));
        else
            expect (false, "No xmp metadata");
    }

    void testPngExif()
    {
        beginTest ("PNG eXIf chunk");

        auto exif = makeExifPayload();

        juce::MemoryOutputStream png;
        writePngHeader (png);
        // The chunk holds the tiff data without the jpeg Exif header
        writePngChunk (png, "eXIf", (const juce::uint8*) exif.getData() + 6, exif.getSize() - 6);
        writePngChunk (png, "IEND", nullptr, 0);

        juce::OwnedArray<ImageMetadata> metadata;
        expect (parsePng (png, metadata), "Should find PNG metadata");

        if (auto md = findType (metadata, "EXIF"))
            expectEquals (md->getAllMetadata()["Make"], juce::String ("Gin"));
        else
            expect (false, "No exif metadata");
    }

    void testNoMetadata()
    {
        beginTest ("Images without metadata");

        {
            juce::MemoryOutputStream jpg;
            jpg.writeByte (char (0xff)); jpg.writeByte (char (0xd8));
            jpg.writeByte (char (0xff)); jpg.writeByte (char (0xe0));
            jpg.writeShortBigEndian (16);
            for (int i = 0; i < 14; i++)
                jpg.writeByte (0);
            jpg.writeByte (char (0xff)); jpg.writeByte (char (0xd9));

            juce::MemoryInputStream is (jpg.getData(), jpg.getDataSize(), false);
            juce::OwnedArray<ImageMetadata> metadata;
            expect (! ImageMetadata::getFromImage (is, metadata), "Plain JPEG has no metadata");
        }

        {
            juce::MemoryOutputStream png;
            writePngHeader (png);
            juce::uint8 ihdr[13] = {};
            writePngChunk (png, "IHDR", ihdr, sizeof (ihdr));
            writePngChunk (png, "IEND", nullptr, 0);

            juce::OwnedArray<ImageMetadata> metadata;
            expect (! parsePng (png, metadata), "Plain PNG has no metadata");
        }

        {
            juce::MemoryBlock garbage (256);
            garbage.fillWith (0x55);

            juce::MemoryInputStream is (garbage, false);
            juce::OwnedArray<ImageMetadata> metadata;
            expect (! ImageMetadata::getFromImage (is, metadata), "Garbage is not an image");
        }
    }
    void testIptcRecords()
    {
        beginTest ("IPTC multiple records");

        juce::MemoryOutputStream iptc;
        iptc.write ("Photoshop 3.0\0", 14);
        iptc.write ("8BIM", 4);
        iptc.writeShort (0x0404);
        iptc.writeByte (0); iptc.writeByte (0);
        iptc.writeIntBigEndian (5 + 5 + 5 + 4);
        iptc.writeByte (0x1c); iptc.writeByte (2); iptc.writeByte (120);
        iptc.writeShortBigEndian (5);
        iptc.write ("Hello", 5);
        iptc.writeByte (0x1c); iptc.writeByte (2); iptc.writeByte (80);
        iptc.writeShortBigEndian (4);
        iptc.write ("John", 4);

        auto md = std::unique_ptr<IptcMetadata> (IptcMetadata::create ((const juce::uint8*) iptc.getData(), int (iptc.getDataSize())));
        if (md != nullptr)
        {
            expectEquals (md->getAllMetadata()["Caption/Abstract"], juce::String ("Hello"));
            expectEquals (md->getAllMetadata()["By-line"], juce::String ("John"));
        }
        else
        {
            expect (false, "No iptc metadata");
        }
    }

    void testMalformedExif()
    {
        beginTest ("Malformed EXIF");

        juce::MemoryOutputStream tiff;
        tiff.write ("Exif\0\0", 6);
        tiff.write ("II", 2);
        tiff.writeShort (42);
        tiff.writeInt (8);              // IFD0 at 8
        tiff.writeShort (6);            // 6 entries
        tiff.writeShort (0x010f);       // Make: ascii with no null terminator
        tiff.writeShort (2);
        tiff.writeInt (4);
        tiff.write ("ABCD", 4);
        tiff.writeShort (0x0110);       // Model: count that overflows count * sizeof
        tiff.writeShort (4);
        tiff.writeInt (0x40000001);
        tiff.writeInt (0);
        tiff.writeShort (0x0132);       // DateTime: data offset past the end
        tiff.writeShort (2);
        tiff.writeInt (32);
        tiff.writeInt (10000);
        tiff.writeShort (513);          // thumbnail offset: huge
        tiff.writeShort (4);
        tiff.writeInt (1);
        tiff.writeInt (0x7fffffff);
        tiff.writeShort (514);          // thumbnail size: huge
        tiff.writeShort (4);
        tiff.writeInt (1);
        tiff.writeInt (0x7fffffff);
        tiff.writeShort (0x8769);       // sub IFD pointing back at IFD0
        tiff.writeShort (4);
        tiff.writeInt (1);
        tiff.writeInt (8);
        tiff.writeInt (8);              // next IFD: cycle back to IFD0

        auto md = std::unique_ptr<ExifMetadata> (ExifMetadata::create ((const juce::uint8*) tiff.getData(), int (tiff.getDataSize())));
        if (md != nullptr)
        {
            auto all = md->getAllMetadata();
            expectEquals (all["Make"], juce::String ("ABCD"), "Unterminated ascii should be bounded");
            expect (! all.containsKey ("Model"), "Overflowing count should be rejected");
            expect (! all.containsKey ("Date Time"), "Out of bounds data should be rejected");
            expect (md->getThumbnailImage().isNull(), "Bogus thumbnail should be rejected");
        }
        else
        {
            expect (false, "No exif metadata");
        }
    }

    void testMalformedIptc()
    {
        beginTest ("Malformed IPTC");

        // block claims 0xffff bytes but the data ends after one record header
        juce::MemoryOutputStream iptc;
        iptc.write ("Photoshop 3.0\0", 14);
        iptc.write ("8BIM", 4);
        iptc.writeShort (0x0404);
        iptc.writeByte (0); iptc.writeByte (0);
        iptc.writeIntBigEndian (0xffff);
        iptc.writeByte (0x1c); iptc.writeByte (2); iptc.writeByte (120);

        auto md = std::unique_ptr<IptcMetadata> (IptcMetadata::create ((const juce::uint8*) iptc.getData(), int (iptc.getDataSize())));
        expect (md == nullptr || md->getAllMetadata().size() == 0, "Truncated IPTC should parse to nothing");
    }
};

static ImageMetadataTests imageMetadataTests;

#endif

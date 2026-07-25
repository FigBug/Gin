/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
// JPEG is a sequence of marker segments: 0xFF, marker id, then (for most
// markers) a 16 bit big endian length that includes the length bytes
// themselves. All the metadata lives in segments before the image data, so
// walk the segments and stop at SOS/EOI.
static bool loadJPEGMetadata (juce::OwnedArray<ImageMetadata>& metadata, const juce::uint8* data, size_t size)
{
    constexpr juce::uint8 markerTEM   = 0x01;
    constexpr juce::uint8 markerRST0  = 0xd0;
    constexpr juce::uint8 markerSOI   = 0xd8;
    constexpr juce::uint8 markerEOI   = 0xd9;
    constexpr juce::uint8 markerSOS   = 0xda;
    constexpr juce::uint8 markerAPP1  = 0xe1;
    constexpr juce::uint8 markerAPP13 = 0xed;
    constexpr juce::uint8 markerCOM   = 0xfe;

    size_t pos = 2;
    while (pos + 2 <= size)
    {
        if (data[pos] != 0xff)
            break;

        const auto marker = data[pos + 1];

        if (marker == 0xff) // padding
        {
            pos++;
            continue;
        }

        pos += 2;

        if (marker == markerTEM || marker == markerSOI || (marker >= markerRST0 && marker < markerEOI))
            continue; // standalone markers with no length

        if (marker == markerEOI || marker == markerSOS)
            break;

        if (pos + 2 > size)
            break;

        const size_t len = juce::uint16 (juce::ByteOrder::bigEndianShort (data + pos));
        if (len < 2 || len > size - pos)
            break;

        auto payload    = data + pos + 2;
        auto payloadLen = int (len - 2);
        pos += len;

        ImageMetadata* md;
        if (marker == markerCOM   && (md = CommentMetadata::create (payload, payloadLen)) != nullptr)
            metadata.add (md);
        if (marker == markerAPP1  && (md = ExifMetadata::create (payload, payloadLen)) != nullptr)
            metadata.add (md);
        if (marker == markerAPP1  && (md = XmpMetadata::createFromJpg (payload, payloadLen)) != nullptr)
            metadata.add (md);
        if (marker == markerAPP13 && (md = IptcMetadata::create (payload, payloadLen)) != nullptr)
            metadata.add (md);
    }

    return metadata.size() > 0;
}

//==============================================================================
static juce::MemoryBlock inflateZlib (const juce::uint8* data, size_t size)
{
    juce::MemoryInputStream in (data, size, false);
    juce::GZIPDecompressorInputStream zlib (&in, false, juce::GZIPDecompressorInputStream::zlibFormat);

    juce::MemoryOutputStream out;
    out.writeFromInputStream (zlib, 16 * 1024 * 1024);
    return out.getMemoryBlock();
}

// PNG text chunks start with a 1-79 byte keyword followed by a null
static int findKeywordEnd (const juce::uint8* data, size_t size)
{
    for (size_t i = 1; i < std::min (size, size_t (80)); i++)
        if (data[i] == 0)
            return int (i);

    return -1;
}

// PNG is an 8 byte signature followed by chunks: 32 bit big endian length,
// 4 byte type, data, 32 bit crc. XMP lives in a text chunk with the keyword
// XML:com.adobe.xmp, exif in an eXIf chunk.
static bool loadPNGMetadata (juce::OwnedArray<ImageMetadata>& metadata, const juce::uint8* data, size_t size)
{
    const juce::String xmpKeyword ("XML:com.adobe.xmp");

    size_t pos = 8;
    while (pos + 12 <= size)
    {
        const size_t len = juce::uint32 (juce::ByteOrder::bigEndianInt (data + pos));
        if (len > size - pos - 12)
            break;

        const char* type = (const char*) data + pos + 4;
        auto chunk = data + pos + 8;
        pos += len + 12;

        if (memcmp (type, "IEND", 4) == 0)
            break;

        if (memcmp (type, "tEXt", 4) == 0 || memcmp (type, "zTXt", 4) == 0 || memcmp (type, "iTXt", 4) == 0)
        {
            const int keywordEnd = findKeywordEnd (chunk, len);
            if (keywordEnd < 0 || juce::String::fromUTF8 ((const char*) chunk, keywordEnd) != xmpKeyword)
                continue;

            juce::MemoryBlock text;

            if (type[0] == 't') // tEXt: keyword \0 text
            {
                text.append (chunk + keywordEnd + 1, len - size_t (keywordEnd) - 1);
            }
            else if (type[0] == 'z') // zTXt: keyword \0 method text
            {
                if (size_t (keywordEnd) + 2 > len || chunk[keywordEnd + 1] != 0)
                    continue;

                text = inflateZlib (chunk + keywordEnd + 2, len - size_t (keywordEnd) - 2);
            }
            else // iTXt: keyword \0 compressed? method language \0 translated \0 text
            {
                if (size_t (keywordEnd) + 3 > len)
                    continue;

                size_t p = size_t (keywordEnd) + 3;
                const bool compressed = chunk[keywordEnd + 1] != 0;

                for (int nulls = 0; p < len && nulls < 2;)
                    if (chunk[p++] == 0)
                        nulls++;

                if (p >= len)
                    continue;

                if (compressed)
                    text = inflateZlib (chunk + p, len - p);
                else
                    text.append (chunk + p, len - p);
            }

            if (text.getSize() > 0)
                metadata.add (XmpMetadata::createFromPng ((const char*) text.getData(), int (text.getSize())));
        }
        else if (memcmp (type, "eXIf", 4) == 0)
        {
            // The chunk holds raw TIFF data, ExifMetadata expects it with the JPEG APP1 header
            juce::MemoryBlock exif ("Exif\0\0", 6);
            exif.append (chunk, len);

            if (auto md = ExifMetadata::create ((const juce::uint8*) exif.getData(), int (exif.getSize())))
                metadata.add (md);
        }
    }

    return metadata.size() > 0;
}

//==============================================================================
ImageMetadata::ImageMetadata(const juce::String& type_) : type(type_)
{
}

ImageMetadata::~ImageMetadata()
{
}

bool ImageMetadata::getFromImage (juce::InputStream& is, juce::OwnedArray<ImageMetadata>& metadata)
{
    juce::MemoryBlock mb;
    is.setPosition (0);
    is.readIntoMemoryBlock (mb);

    auto data = (const juce::uint8*) mb.getData();
    auto size = mb.getSize();

    static const juce::uint8 pngSig[] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };

    if (size > 16 && data[0] == 0xff && data[1] == 0xd8 && data[2] == 0xff)
        return loadJPEGMetadata (metadata, data, size);

    if (size > 16 && memcmp (data, pngSig, 8) == 0)
        return loadPNGMetadata (metadata, data, size);

    return false;
}

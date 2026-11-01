/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

namespace
{

//==============================================================================
/** The compression field of a BMP's info header */
enum BMPCompression
{
    bmpRGB            = 0,  // uncompressed
    bmpRLE8           = 1,  // run length encoded, 8 bits per pixel
    bmpRLE4           = 2,  // run length encoded, 4 bits per pixel
    bmpBitfields      = 3,  // uncompressed, the channels described by masks
    bmpJPEG           = 4,  // a whole JPEG file, which is not handled here
    bmpPNG            = 5,  // a whole PNG file, which is not handled here
    bmpAlphaBitfields = 6   // as bmpBitfields, with a mask for alpha as well
};

/** The size of the info header tells its version apart: 12 is the OS/2 header, whose
    fields are narrower and whose palette entries are three bytes rather than four
*/
constexpr uint32_t bmpCoreHeaderSize = 12;

/** The largest image that will be decoded, so a corrupt header cannot ask for a
    preposterous allocation
*/
constexpr int64_t bmpMaxPixels = 1 << 28;

struct BMPHeader
{
    uint32_t headerSize     = 0;
    int32_t  width          = 0;
    int32_t  height         = 0;
    uint16_t planes         = 1;
    uint16_t bitsPerPixel   = 0;
    uint32_t compression    = bmpRGB;
    uint32_t imageDataSize  = 0;
    uint32_t coloursUsed    = 0;

    uint32_t redMask        = 0;
    uint32_t greenMask      = 0;
    uint32_t blueMask       = 0;
    uint32_t alphaMask      = 0;

    bool topDown            = false;
    bool hasMasks           = false;
};

/** One channel's bits inside a packed pixel, as a mask of a 16 or 32 bit word. Scaling
    to eight bits spreads the channel over the full range, so a five bit 31 reaches 255
    rather than stopping at 248
*/
struct BMPChannel
{
    uint32_t mask  = 0;
    int      shift = 0;
    int      bits  = 0;

    void setMask (uint32_t m)
    {
        mask  = m;
        shift = 0;
        bits  = 0;

        if (m == 0)
            return;

        while ((m & 1u) == 0) { m >>= 1; shift++; }
        while ((m & 1u) != 0) { m >>= 1; bits++; }
    }

    uint8_t extract (uint32_t pixel) const
    {
        if (mask == 0 || bits == 0)
            return 0;

        const auto v = (pixel & mask) >> shift;

        if (bits == 8)  return uint8_t (v);
        if (bits  > 8)  return uint8_t (v >> (bits - 8));

        const auto largest = (1u << bits) - 1u;
        return uint8_t ((v * 255u + largest / 2u) / largest);
    }
};

/** Reads the file header and whichever info header follows it, leaving the stream on the
    first byte after the info header. Returns false if this is not a BMP this can decode.
*/
bool readBMPHeader (juce::InputStream& input, BMPHeader& hdr)
{
    if (input.readByte() != 'B' || input.readByte() != 'M')
        return false;

    // The rest of the file header: size, two reserved shorts and the offset of the pixel
    // data, which the caller has already read
    input.skipNextBytes (12);

    hdr.headerSize = uint32_t (input.readInt());

    if (hdr.headerSize == bmpCoreHeaderSize)
    {
        hdr.width        = int32_t  (int16_t (input.readShort()));
        hdr.height       = int32_t  (int16_t (input.readShort()));
        hdr.planes       = uint16_t (input.readShort());
        hdr.bitsPerPixel = uint16_t (input.readShort());
    }
    else if (hdr.headerSize >= 40)
    {
        hdr.width         = int32_t  (input.readInt());
        hdr.height        = int32_t  (input.readInt());
        hdr.planes        = uint16_t (input.readShort());
        hdr.bitsPerPixel  = uint16_t (input.readShort());
        hdr.compression   = uint32_t (input.readInt());
        hdr.imageDataSize = uint32_t (input.readInt());
        input.skipNextBytes (8);                    // pixels per metre, horizontal and vertical
        hdr.coloursUsed   = uint32_t (input.readInt());
        input.skipNextBytes (4);                    // colours required

        // A V2 header or later carries its channel masks inside the header itself, where
        // a plain info header puts them after it
        if (hdr.headerSize >= 52)
        {
            hdr.redMask   = uint32_t (input.readInt());
            hdr.greenMask = uint32_t (input.readInt());
            hdr.blueMask  = uint32_t (input.readInt());
            hdr.hasMasks  = true;

            if (hdr.headerSize >= 56)
                hdr.alphaMask = uint32_t (input.readInt());
        }
        else if (hdr.compression == bmpBitfields || hdr.compression == bmpAlphaBitfields)
        {
            hdr.redMask   = uint32_t (input.readInt());
            hdr.greenMask = uint32_t (input.readInt());
            hdr.blueMask  = uint32_t (input.readInt());
            hdr.hasMasks  = true;

            if (hdr.compression == bmpAlphaBitfields)
                hdr.alphaMask = uint32_t (input.readInt());
        }
    }
    else
    {
        return false;
    }

    // A negative height means the rows run top to bottom instead of bottom to top
    hdr.topDown = hdr.height < 0;
    hdr.height  = std::abs (hdr.height);

    if (hdr.width <= 0 || hdr.height <= 0 || hdr.planes != 1)
        return false;

    if (int64_t (hdr.width) * int64_t (hdr.height) > bmpMaxPixels)
        return false;

    switch (hdr.bitsPerPixel)
    {
        case 1: case 4: case 8: case 16: case 24: case 32: break;
        default: return false;
    }

    if (hdr.compression == bmpJPEG || hdr.compression == bmpPNG)
        return false;

    if (hdr.compression == bmpRLE8 && hdr.bitsPerPixel != 8)  return false;
    if (hdr.compression == bmpRLE4 && hdr.bitsPerPixel != 4)  return false;

    if ((hdr.compression == bmpBitfields || hdr.compression == bmpAlphaBitfields)
        && hdr.bitsPerPixel != 16 && hdr.bitsPerPixel != 32)
        return false;

    return true;
}

/** The palette sits between the info header and the pixel data. Its entries are three
    bytes wide in an OS/2 header and four everywhere else.
*/
juce::Array<juce::PixelARGB> readBMPPalette (juce::InputStream& input, const BMPHeader& hdr)
{
    juce::Array<juce::PixelARGB> colours;

    if (hdr.bitsPerPixel > 8)
        return colours;

    auto count = hdr.coloursUsed;

    if (count == 0 || count > (1u << hdr.bitsPerPixel))
        count = 1u << hdr.bitsPerPixel;

    const auto entrySize = hdr.headerSize == bmpCoreHeaderSize ? 3 : 4;

    for (uint32_t i = 0; i < count; i++)
    {
        const auto b = uint8_t (input.readByte());
        const auto g = uint8_t (input.readByte());
        const auto r = uint8_t (input.readByte());

        if (entrySize == 4)
            input.readByte();

        colours.add (juce::PixelARGB (255, r, g, b));
    }

    return colours;
}

/** A palette entry, or opaque black when the index is out of range, so a damaged file
    cannot read past the table
*/
juce::PixelARGB paletteColour (const juce::Array<juce::PixelARGB>& colours, int index)
{
    if (juce::isPositiveAndBelow (index, colours.size()))
        return colours.getUnchecked (index);

    return juce::PixelARGB (255, 0, 0, 0);
}

/** The index of the pixel at x in a row of 1, 4 or 8 bit indices */
int indexInRow (const uint8_t* row, int x, int bitsPerPixel)
{
    switch (bitsPerPixel)
    {
        case 1:  return (row[x / 8] >> (7 - (x % 8))) & 0x01;
        case 4:  return (row[x / 2] >> ((x % 2) == 0 ? 4 : 0)) & 0x0f;
        default: return row[x];
    }
}

} // namespace

//==============================================================================
juce::String BMPImageFormat::getFormatName()
{
    return "Windows Bitmap";
}

bool BMPImageFormat::canUnderstand (juce::InputStream& input)
{
    return input.readByte() == 'B' &&
           input.readByte() == 'M';
}

bool BMPImageFormat::usesFileExtension (const juce::File& possibleFile)
{
    return possibleFile.hasFileExtension ("bmp");
}

juce::Image BMPImageFormat::decodeImage (juce::InputStream& input)
{
    const auto start = input.getPosition();

    // The data offset lives in the file header, which readBMPHeader skips over
    input.setPosition (start + 10);
    const auto dataOffset = int64_t (uint32_t (input.readInt()));

    input.setPosition (start);

    BMPHeader hdr;

    if (! readBMPHeader (input, hdr))
    {
        jassertfalse; // Unsupported or malformed BMP
        return {};
    }

    const auto colours = readBMPPalette (input, hdr);

    juce::Image img (juce::Image::ARGB, hdr.width, hdr.height, true, juce::SoftwareImageType());
    juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

    // The row a source line lands on, which the top down flag flips
    const auto destRow = [&] (int y) { return hdr.topDown ? y : hdr.height - y - 1; };

    input.setPosition (start + dataOffset);

    if (hdr.compression == bmpRLE8 || hdr.compression == bmpRLE4)
    {
        const auto fourBit = hdr.compression == bmpRLE4;

        auto x = 0, y = 0;
        auto finished = false;

        const auto put = [&] (int px, int py, int index)
        {
            if (juce::isPositiveAndBelow (px, hdr.width) && juce::isPositiveAndBelow (py, hdr.height))
                *(juce::PixelARGB*) data.getPixelPointer (px, destRow (py)) = paletteColour (colours, index);
        };

        while (! finished && ! input.isExhausted() && y < hdr.height)
        {
            const auto count = uint8_t (input.readByte());
            const auto value = uint8_t (input.readByte());

            if (count > 0)
            {
                // A run of one colour, or of two alternating ones in the four bit case
                for (int i = 0; i < count; i++, x++)
                    put (x, y, fourBit ? ((i % 2) == 0 ? (value >> 4) : (value & 0x0f)) : value);
            }
            else
            {
                switch (value)
                {
                    case 0:                                 // end of this line
                        x = 0;
                        y++;
                        break;

                    case 1:                                 // end of the image
                        finished = true;
                        break;

                    case 2:                                 // move by a delta
                        x += uint8_t (input.readByte());
                        y += uint8_t (input.readByte());
                        break;

                    default:                                // a literal run, padded to a word
                    {
                        const int literal = value;

                        if (fourBit)
                        {
                            for (int i = 0; i < literal; i += 2)
                            {
                                const auto pair = uint8_t (input.readByte());

                                put (x++, y, pair >> 4);

                                if (i + 1 < literal)
                                    put (x++, y, pair & 0x0f);
                            }

                            if ((((literal + 1) / 2) % 2) != 0)
                                input.readByte();
                        }
                        else
                        {
                            for (int i = 0; i < literal; i++)
                                put (x++, y, uint8_t (input.readByte()));

                            if ((literal % 2) != 0)
                                input.readByte();
                        }

                        break;
                    }
                }
            }
        }

        return img;
    }

    // Uncompressed, of whatever width. Rows are padded out to a multiple of four bytes
    const auto bytesPerRow = ((int (hdr.bitsPerPixel) * hdr.width + 31) / 32) * 4;

    BMPChannel red, green, blue, alpha;

    if (hdr.hasMasks && (hdr.redMask | hdr.greenMask | hdr.blueMask) != 0)
    {
        red.setMask (hdr.redMask);
        green.setMask (hdr.greenMask);
        blue.setMask (hdr.blueMask);
        alpha.setMask (hdr.alphaMask);
    }
    else if (hdr.bitsPerPixel == 16)
    {
        // Without masks a 16 bit bitmap is five bits per channel, the top bit unused
        red.setMask   (0x7c00);
        green.setMask (0x03e0);
        blue.setMask  (0x001f);
    }
    else if (hdr.bitsPerPixel == 32)
    {
        red.setMask   (0x00ff0000);
        green.setMask (0x0000ff00);
        blue.setMask  (0x000000ff);

        // The fourth byte is reserved rather than alpha here, but writers put alpha in it
        // all the same, this module's own among them. It is read as alpha and then thrown
        // away below if no pixel turned out to use it
        alpha.setMask (0xff000000);
    }

    juce::HeapBlock<uint8_t> rowData (size_t (bytesPerRow), true);

    // The fourth byte of an uncompressed 32 bit pixel is reserved rather than alpha, and
    // writers that leave it at zero would otherwise decode to a wholly invisible image, so
    // it only counts as alpha once some pixel puts something there
    auto sawAlpha = false;

    for (int y = 0; y < hdr.height; y++)
    {
        if (input.read (rowData, bytesPerRow) != bytesPerRow)
            break;

        for (int x = 0; x < hdr.width; x++)
        {
            auto* p = (juce::PixelARGB*) data.getPixelPointer (x, destRow (y));

            if (hdr.bitsPerPixel <= 8)
            {
                *p = paletteColour (colours, indexInRow (rowData, x, hdr.bitsPerPixel));
            }
            else if (hdr.bitsPerPixel == 24)
            {
                const auto* d = &rowData[x * 3];
                p->setARGB (255, d[2], d[1], d[0]);
            }
            else if (hdr.bitsPerPixel == 16)
            {
                const auto* d = &rowData[x * 2];
                const auto v = uint32_t (d[0]) | (uint32_t (d[1]) << 8);

                const auto a = alpha.bits > 0 ? alpha.extract (v) : uint8_t (255);
                sawAlpha = sawAlpha || (alpha.bits > 0 && a != 0);

                p->setARGB (a, red.extract (v), green.extract (v), blue.extract (v));
            }
            else
            {
                const auto* d = &rowData[x * 4];
                const auto v = uint32_t (d[0]) | (uint32_t (d[1]) << 8) | (uint32_t (d[2]) << 16) | (uint32_t (d[3]) << 24);

                const auto a = alpha.bits > 0 ? alpha.extract (v) : uint8_t (255);
                sawAlpha = sawAlpha || (alpha.bits > 0 && a != 0);

                p->setARGB (a, red.extract (v), green.extract (v), blue.extract (v));
            }
        }
    }

    if (alpha.bits > 0 && ! sawAlpha)
    {
        // Every pixel claimed to be invisible, which no real image means
        for (int y = 0; y < hdr.height; y++)
            for (int x = 0; x < hdr.width; x++)
                ((juce::PixelARGB*) data.getPixelPointer (x, y))->setAlpha (255);
    }

    return img;
}

bool BMPImageFormat::writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& dst)
{
    juce::Image img = sourceImage.convertedToFormat (juce::Image::ARGB);

    dst.writeByte ('B');
    dst.writeByte ('M');
    dst.writeInt (40 + img.getWidth() * img.getHeight() * 4);
    dst.writeShort (0);
    dst.writeShort (0);
    dst.writeInt (54);
    dst.writeInt (40);
    dst.writeInt (img.getWidth());
    dst.writeInt (img.getHeight());
    dst.writeShort (1);
    dst.writeShort (32);
    dst.writeInt (0);
    dst.writeInt (img.getWidth() * img.getHeight() * 4);
    dst.writeInt (2835);
    dst.writeInt (2835);
    dst.writeInt (0);
    dst.writeInt (0);

    juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);
    for (int y = 0; y < img.getHeight(); y++)
    {
        for (int x = 0; x < img.getWidth(); x++)
        {
            juce::PixelARGB* p = (juce::PixelARGB*)data.getPixelPointer (x, int (img.getHeight() - y - 1));
            dst.writeByte (char (p->getBlue()));
            dst.writeByte (char (p->getGreen()));
            dst.writeByte (char (p->getRed()));
            dst.writeByte (char (p->getAlpha()));
        }
    }

    return true;
}

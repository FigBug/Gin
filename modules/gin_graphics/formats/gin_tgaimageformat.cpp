/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

namespace
{

//==============================================================================
/** The image type field of a TGA header. The run length encoded types are the plain
    ones with bit 3 set.
*/
enum TGAImageType
{
    tgaNone          = 0,
    tgaColourMapped  = 1,
    tgaTrueColour    = 2,
    tgaGreyscale     = 3,
    tgaRLEColourMapped = 9,
    tgaRLETrueColour   = 10,
    tgaRLEGreyscale    = 11
};

constexpr int tgaHeaderSize = 18;

/** The largest image that will be decoded, so a corrupt header cannot ask for a
    preposterous allocation
*/
constexpr int64_t tgaMaxPixels = 1 << 28;

/** Version 2 of the format ends with this, which is the only dependable way to know a
    stream is a TGA rather than something that happens to start plausibly
*/
const char* const tgaFooterSignature = "TRUEVISION-XFILE.";

struct TGAHeader
{
    uint8_t  idLength           = 0;
    uint8_t  colourMapType      = 0;
    uint8_t  imageType          = 0;
    uint16_t colourMapFirst     = 0;
    uint16_t colourMapLength    = 0;
    uint8_t  colourMapDepth     = 0;
    uint16_t width              = 0;
    uint16_t height             = 0;
    uint8_t  pixelDepth         = 0;
    uint8_t  descriptor         = 0;

    bool isRLE() const          { return (imageType & 8) != 0; }
    int  baseType() const       { return imageType & 7; }

    // The descriptor's top two bits say which corner the first pixel belongs to
    bool rightToLeft() const    { return (descriptor & 0x10) != 0; }
    bool topToBottom() const    { return (descriptor & 0x20) != 0; }
};

void readTGAHeader (juce::InputStream& input, TGAHeader& hdr)
{
    hdr.idLength        = uint8_t  (input.readByte());
    hdr.colourMapType   = uint8_t  (input.readByte());
    hdr.imageType       = uint8_t  (input.readByte());
    hdr.colourMapFirst  = uint16_t (input.readShort());
    hdr.colourMapLength = uint16_t (input.readShort());
    hdr.colourMapDepth  = uint8_t  (input.readByte());
    input.skipNextBytes (4);                            // the origin, which says nothing about the pixels
    hdr.width           = uint16_t (input.readShort());
    hdr.height          = uint16_t (input.readShort());
    hdr.pixelDepth      = uint8_t  (input.readByte());
    hdr.descriptor      = uint8_t  (input.readByte());
}

bool tgaHeaderLooksSane (const TGAHeader& hdr)
{
    if (hdr.colourMapType > 1)
        return false;

    switch (hdr.imageType)
    {
        case tgaColourMapped: case tgaTrueColour: case tgaGreyscale:
        case tgaRLEColourMapped: case tgaRLETrueColour: case tgaRLEGreyscale:
            break;
        default:
            return false;
    }

    switch (hdr.pixelDepth)
    {
        case 8: case 15: case 16: case 24: case 32: break;
        default: return false;
    }

    if (hdr.width == 0 || hdr.height == 0)
        return false;

    if (int64_t (hdr.width) * int64_t (hdr.height) > tgaMaxPixels)
        return false;

    // A colour mapped image needs a map, and one without a map should not describe one
    const auto mapped = hdr.baseType() == tgaColourMapped;

    if (mapped && (hdr.colourMapType != 1 || hdr.colourMapLength == 0))
        return false;

    if (hdr.colourMapType == 1)
    {
        switch (hdr.colourMapDepth)
        {
            case 15: case 16: case 24: case 32: break;
            default: return false;
        }
    }

    return true;
}

/** One pixel of the given depth, read from a buffer holding at least that many bytes.
    15 and 16 bit pixels are five bits a channel, the sixteenth bit being alpha.
*/
juce::PixelARGB tgaPixel (const uint8_t* d, int depth, bool greyscale)
{
    switch (depth)
    {
        case 8:
            // The only eight bit samples that reach here are greyscale, an index having
            // been resolved through the colour map by the caller
            juce::ignoreUnused (greyscale);
            return juce::PixelARGB (255, d[0], d[0], d[0]);

        case 15:
        case 16:
        {
            const auto v = uint32_t (d[0]) | (uint32_t (d[1]) << 8);

            const auto scale5 = [] (uint32_t c) { return uint8_t ((c * 255u + 15u) / 31u); };

            const auto r = scale5 ((v >> 10) & 0x1f);
            const auto g = scale5 ((v >>  5) & 0x1f);
            const auto b = scale5 ( v        & 0x1f);

            // At 15 bits the top bit means nothing; at 16 it is a single bit of alpha,
            // which is only believed when the image actually uses it
            const auto a = depth == 16 ? uint8_t ((v & 0x8000) != 0 ? 255 : 0) : uint8_t (255);

            return juce::PixelARGB (a, r, g, b);
        }

        case 24:
            return juce::PixelARGB (255, d[2], d[1], d[0]);

        default:
            return juce::PixelARGB (d[3], d[2], d[1], d[0]);
    }
}

} // namespace

//==============================================================================
juce::String TGAImageFormat::getFormatName()
{
    return "Truevision TGA";
}

bool TGAImageFormat::canUnderstand (juce::InputStream& input)
{
    const auto start = input.getPosition();
    const auto total = input.getTotalLength();

    // Version 2 puts a signature at the very end, which settles it outright
    if (total >= 26)
    {
        input.setPosition (total - 18);

        char sig[18] = {};
        input.read (sig, 17);

        input.setPosition (start);

        if (juce::String (sig) == tgaFooterSignature)
            return true;
    }

    // Otherwise the header has to carry it. Everything in it must be plausible, which
    // is weaker than a magic number but is all the original format offers
    if (total - start < tgaHeaderSize)
        return false;

    TGAHeader hdr;
    readTGAHeader (input, hdr);
    input.setPosition (start);

    return tgaHeaderLooksSane (hdr);
}

bool TGAImageFormat::usesFileExtension (const juce::File& possibleFile)
{
    return possibleFile.hasFileExtension ("tga;targa;tpic;icb;vda;vst");
}

juce::Image TGAImageFormat::decodeImage (juce::InputStream& input)
{
    TGAHeader hdr;
    readTGAHeader (input, hdr);

    if (! tgaHeaderLooksSane (hdr))
    {
        jassertfalse; // Unsupported or malformed TGA
        return {};
    }

    input.skipNextBytes (hdr.idLength);

    const auto mapped    = hdr.baseType() == tgaColourMapped;
    const auto greyscale = hdr.baseType() == tgaGreyscale;

    juce::Array<juce::PixelARGB> colourMap;

    if (hdr.colourMapType == 1)
    {
        const auto entryBytes = (hdr.colourMapDepth + 7) / 8;

        for (int i = 0; i < int (hdr.colourMapLength); i++)
        {
            uint8_t entry[4] = {};

            if (input.read (entry, entryBytes) != entryBytes)
                break;

            colourMap.add (tgaPixel (entry, hdr.colourMapDepth, false));
        }
    }

    // A colour mapped image indexes the map, everything else carries its own pixels
    const auto sampleDepth = mapped ? 8 : int (hdr.pixelDepth);
    const auto sampleBytes = (sampleDepth + 7) / 8;

    juce::Image img (juce::Image::ARGB, hdr.width, hdr.height, true, juce::SoftwareImageType());
    juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

    // The descriptor says which corner the first pixel belongs to
    const auto destX = [&] (int x) { return hdr.rightToLeft() ? hdr.width  - x - 1 : x; };
    const auto destY = [&] (int y) { return hdr.topToBottom() ? y : hdr.height - y - 1; };

    const auto place = [&] (int index, const uint8_t* sample)
    {
        const auto x = index % hdr.width;
        const auto y = index / hdr.width;

        if (y >= hdr.height)
            return;

        auto* p = (juce::PixelARGB*) data.getPixelPointer (destX (x), destY (y));

        if (mapped)
        {
            const auto entry = int (sample[0]) - int (hdr.colourMapFirst);

            *p = juce::isPositiveAndBelow (entry, colourMap.size()) ? colourMap.getUnchecked (entry)
                                                                    : juce::PixelARGB (255, 0, 0, 0);
        }
        else
        {
            *p = tgaPixel (sample, sampleDepth, greyscale);
        }
    };

    const auto pixelCount = int (hdr.width) * int (hdr.height);

    if (hdr.isRLE())
    {
        auto written = 0;

        while (written < pixelCount && ! input.isExhausted())
        {
            const auto packet = uint8_t (input.readByte());
            const auto count  = int (packet & 0x7f) + 1;

            if ((packet & 0x80) != 0)
            {
                // A run: one sample repeated
                uint8_t sample[4] = {};

                if (input.read (sample, sampleBytes) != sampleBytes)
                    break;

                for (int i = 0; i < count && written < pixelCount; i++)
                    place (written++, sample);
            }
            else
            {
                for (int i = 0; i < count && written < pixelCount; i++)
                {
                    uint8_t sample[4] = {};

                    if (input.read (sample, sampleBytes) != sampleBytes)
                        return img;

                    place (written++, sample);
                }
            }
        }
    }
    else
    {
        juce::HeapBlock<uint8_t> row (size_t (hdr.width * sampleBytes), true);

        for (int y = 0; y < int (hdr.height); y++)
        {
            const auto rowBytes = int (hdr.width) * sampleBytes;

            if (input.read (row, rowBytes) != rowBytes)
                break;

            for (int x = 0; x < int (hdr.width); x++)
                place (y * int (hdr.width) + x, &row[x * sampleBytes]);
        }
    }

    // A 16 bit image whose every alpha bit is clear meant them all to be opaque
    if (! mapped && hdr.pixelDepth == 16)
    {
        auto sawAlpha = false;

        for (int y = 0; y < img.getHeight() && ! sawAlpha; y++)
            for (int x = 0; x < img.getWidth(); x++)
                if (((juce::PixelARGB*) data.getPixelPointer (x, y))->getAlpha() != 0)
                {
                    sawAlpha = true;
                    break;
                }

        if (! sawAlpha)
            for (int y = 0; y < img.getHeight(); y++)
                for (int x = 0; x < img.getWidth(); x++)
                    ((juce::PixelARGB*) data.getPixelPointer (x, y))->setAlpha (255);
    }

    return img;
}

bool TGAImageFormat::writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& dst)
{
    const auto img = sourceImage.convertedToFormat (juce::Image::ARGB);

    const auto w = img.getWidth();
    const auto h = img.getHeight();

    if (w <= 0 || h <= 0 || w > 0xffff || h > 0xffff)
        return false;

    dst.writeByte (0);                      // no id field
    dst.writeByte (0);                      // no colour map
    dst.writeByte (tgaTrueColour);
    dst.writeShort (0);                     // colour map origin
    dst.writeShort (0);                     // colour map length
    dst.writeByte (0);                      // colour map depth
    dst.writeShort (0);                     // x origin
    dst.writeShort (0);                     // y origin
    dst.writeShort (short (w));
    dst.writeShort (short (h));
    dst.writeByte (32);
    dst.writeByte (8 | 0x20);               // eight bits of alpha, rows running top to bottom

    juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            auto* p = (juce::PixelARGB*) data.getPixelPointer (x, y);

            dst.writeByte (char (p->getBlue()));
            dst.writeByte (char (p->getGreen()));
            dst.writeByte (char (p->getRed()));
            dst.writeByte (char (p->getAlpha()));
        }
    }

    // The version 2 footer, so this writes something canUnderstand can recognise outright
    dst.writeInt (0);                       // no extension area
    dst.writeInt (0);                       // no developer directory
    dst.write (tgaFooterSignature, 17);
    dst.writeByte (0);

    return true;
}

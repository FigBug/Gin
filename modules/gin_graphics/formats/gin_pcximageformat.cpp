/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

namespace
{

//==============================================================================
constexpr uint8_t pcxMagic       = 0x0a;
constexpr int     pcxHeaderSize  = 128;

/** A 256 colour image keeps its palette in the last 769 bytes of the file, introduced
    by this marker
*/
constexpr uint8_t pcxPaletteMarker = 0x0c;
constexpr int     pcxPaletteSize   = 1 + 256 * 3;

/** The largest image that will be decoded, so a corrupt header cannot ask for a
    preposterous allocation
*/
constexpr int64_t pcxMaxPixels = 1 << 28;

struct PCXHeader
{
    uint8_t  version        = 0;
    uint8_t  encoding       = 0;
    uint8_t  bitsPerPlane   = 0;
    uint16_t xMin           = 0;
    uint16_t yMin           = 0;
    uint16_t xMax           = 0;
    uint16_t yMax           = 0;
    uint8_t  planes         = 0;
    uint16_t bytesPerLine   = 0;

    // The sixteen entry palette in the header, used by the low colour versions
    juce::Array<juce::PixelARGB> egaPalette;

    int width()  const { return int (xMax) - int (xMin) + 1; }
    int height() const { return int (yMax) - int (yMin) + 1; }
};

void readPCXHeader (juce::InputStream& input, PCXHeader& hdr)
{
    input.readByte();                                   // the manufacturer byte, already checked
    hdr.version      = uint8_t  (input.readByte());
    hdr.encoding     = uint8_t  (input.readByte());
    hdr.bitsPerPlane = uint8_t  (input.readByte());
    hdr.xMin         = uint16_t (input.readShort());
    hdr.yMin         = uint16_t (input.readShort());
    hdr.xMax         = uint16_t (input.readShort());
    hdr.yMax         = uint16_t (input.readShort());
    input.skipNextBytes (4);                            // the dpi, which says nothing about the pixels

    for (int i = 0; i < 16; i++)
    {
        const auto r = uint8_t (input.readByte());
        const auto g = uint8_t (input.readByte());
        const auto b = uint8_t (input.readByte());

        hdr.egaPalette.add (juce::PixelARGB (255, r, g, b));
    }

    input.readByte();                                   // reserved
    hdr.planes       = uint8_t  (input.readByte());
    hdr.bytesPerLine = uint16_t (input.readShort());

    input.skipNextBytes (60);                           // palette info, screen size and filler
}

bool pcxHeaderLooksSane (const PCXHeader& hdr)
{
    if (hdr.encoding > 1)
        return false;

    switch (hdr.version)
    {
        case 0: case 2: case 3: case 4: case 5: break;
        default: return false;
    }

    switch (hdr.bitsPerPlane)
    {
        case 1: case 2: case 4: case 8: break;
        default: return false;
    }

    if (hdr.planes < 1 || hdr.planes > 4)
        return false;

    if (hdr.width() <= 0 || hdr.height() <= 0)
        return false;

    if (int64_t (hdr.width()) * int64_t (hdr.height()) > pcxMaxPixels)
        return false;

    if (hdr.bytesPerLine == 0)
        return false;

    // A line has to hold the pixels the dimensions promise
    const auto needed = (hdr.width() * int (hdr.bitsPerPlane) + 7) / 8;

    if (int (hdr.bytesPerLine) < needed)
        return false;

    return true;
}

/** Unpacks one scanline. A byte with its top two bits set is a count for the byte that
    follows; everything else stands for itself. The uncompressed encoding just reads.
*/
bool readPCXScanline (juce::InputStream& input, uint8_t* line, int length, bool rle)
{
    if (! rle)
        return input.read (line, length) == length;

    auto written = 0;

    while (written < length)
    {
        if (input.isExhausted())
            return false;

        const auto b = uint8_t (input.readByte());

        if ((b & 0xc0) == 0xc0)
        {
            const auto count = int (b & 0x3f);
            const auto value = uint8_t (input.readByte());

            for (int i = 0; i < count && written < length; i++)
                line[written++] = value;
        }
        else
        {
            line[written++] = b;
        }
    }

    return true;
}

/** The value of pixel x in one plane of a scanline */
int pcxSample (const uint8_t* plane, int x, int bitsPerPlane)
{
    switch (bitsPerPlane)
    {
        case 1:  return (plane[x / 8] >> (7 - (x % 8))) & 0x01;
        case 2:  return (plane[x / 4] >> (6 - 2 * (x % 4))) & 0x03;
        case 4:  return (plane[x / 2] >> ((x % 2) == 0 ? 4 : 0)) & 0x0f;
        default: return plane[x];
    }
}

} // namespace

//==============================================================================
juce::String PCXImageFormat::getFormatName()
{
    return "ZSoft PCX";
}

bool PCXImageFormat::canUnderstand (juce::InputStream& input)
{
    const auto start = input.getPosition();

    if (uint8_t (input.readByte()) != pcxMagic)
    {
        input.setPosition (start);
        return false;
    }

    input.setPosition (start);

    if (input.getTotalLength() - start < pcxHeaderSize)
        return false;

    PCXHeader hdr;
    readPCXHeader (input, hdr);
    input.setPosition (start);

    return pcxHeaderLooksSane (hdr);
}

bool PCXImageFormat::usesFileExtension (const juce::File& possibleFile)
{
    return possibleFile.hasFileExtension ("pcx");
}

juce::Image PCXImageFormat::decodeImage (juce::InputStream& input)
{
    const auto start = input.getPosition();

    if (uint8_t (input.readByte()) != pcxMagic)
    {
        jassertfalse; // Not a PCX
        return {};
    }

    input.setPosition (start);

    PCXHeader hdr;
    readPCXHeader (input, hdr);

    if (! pcxHeaderLooksSane (hdr))
    {
        jassertfalse; // Unsupported or malformed PCX
        return {};
    }

    const auto w = hdr.width();
    const auto h = hdr.height();

    // A 256 colour image appends its palette to the file, which has to be fetched before
    // the pixels are read since the stream only runs forwards from here
    juce::Array<juce::PixelARGB> vgaPalette;

    if (hdr.planes == 1 && hdr.bitsPerPlane == 8)
    {
        const auto total = input.getTotalLength();

        if (total >= pcxPaletteSize)
        {
            const auto here = input.getPosition();

            input.setPosition (total - pcxPaletteSize);

            if (uint8_t (input.readByte()) == pcxPaletteMarker)
            {
                for (int i = 0; i < 256; i++)
                {
                    const auto r = uint8_t (input.readByte());
                    const auto g = uint8_t (input.readByte());
                    const auto b = uint8_t (input.readByte());

                    vgaPalette.add (juce::PixelARGB (255, r, g, b));
                }
            }

            input.setPosition (here);
        }
    }

    const auto& palette = vgaPalette.isEmpty() ? hdr.egaPalette : vgaPalette;

    juce::Image img (juce::Image::ARGB, w, h, true, juce::SoftwareImageType());
    juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

    const auto rle        = hdr.encoding == 1;
    const auto planes     = int (hdr.planes);
    const auto lineBytes  = int (hdr.bytesPerLine);
    const auto scanBytes  = lineBytes * planes;

    juce::HeapBlock<uint8_t> scan (size_t (scanBytes), true);

    for (int y = 0; y < h; y++)
    {
        // Every plane of a row is stored together, one after the other
        if (! readPCXScanline (input, scan, scanBytes, rle))
            break;

        for (int x = 0; x < w; x++)
        {
            auto* p = (juce::PixelARGB*) data.getPixelPointer (x, y);

            if (planes >= 3 && hdr.bitsPerPlane == 8)
            {
                // One plane each of red, green and blue, and a fourth of alpha if present
                const auto r = scan[x];
                const auto g = scan[lineBytes + x];
                const auto b = scan[2 * lineBytes + x];
                const auto a = planes == 4 ? scan[3 * lineBytes + x] : uint8_t (255);

                p->setARGB (a, r, g, b);
            }
            else
            {
                // The planes together make up the palette index, one bit each in the EGA
                // layouts and the whole index in a 256 colour image
                auto index = 0;

                for (int plane = 0; plane < planes; plane++)
                    index |= pcxSample (&scan[plane * lineBytes], x, hdr.bitsPerPlane) << (plane * hdr.bitsPerPlane);

                if (juce::isPositiveAndBelow (index, palette.size()))
                    *p = palette.getUnchecked (index);
                else
                    p->setARGB (255, 0, 0, 0);
            }
        }
    }

    return img;
}

bool PCXImageFormat::writeImageToStream (const juce::Image& sourceImage, juce::OutputStream& dst)
{
    const auto img = sourceImage.convertedToFormat (juce::Image::ARGB);

    const auto w = img.getWidth();
    const auto h = img.getHeight();

    if (w <= 0 || h <= 0 || w > 0xffff || h > 0xffff)
        return false;

    // Each line of a plane is padded to an even length
    const auto bytesPerLine = w + (w % 2);

    dst.writeByte (char (pcxMagic));
    dst.writeByte (5);                      // version 5, the one with 24 bit colour
    dst.writeByte (1);                      // run length encoded
    dst.writeByte (8);                      // bits per plane
    dst.writeShort (0);                     // xMin
    dst.writeShort (0);                     // yMin
    dst.writeShort (short (w - 1));
    dst.writeShort (short (h - 1));
    dst.writeShort (72);                    // horizontal dpi
    dst.writeShort (72);                    // vertical dpi

    for (int i = 0; i < 48; i++)            // the EGA palette, unused at this depth
        dst.writeByte (0);

    dst.writeByte (0);                      // reserved
    dst.writeByte (3);                      // three planes: red, green and blue
    dst.writeShort (short (bytesPerLine));
    dst.writeShort (1);                     // a colour palette rather than greyscale
    dst.writeShort (0);                     // screen size, which nothing reads
    dst.writeShort (0);

    for (int i = 0; i < 54; i++)            // filler, out to 128 bytes
        dst.writeByte (0);

    juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);

    // One plane at a time, each run written as a count byte and a value
    const auto writePlane = [&] (const uint8_t* line)
    {
        auto x = 0;

        while (x < bytesPerLine)
        {
            const auto value = line[x];
            auto run = 1;

            while (x + run < bytesPerLine && line[x + run] == value && run < 63)
                run++;

            if (run > 1 || (value & 0xc0) == 0xc0)
            {
                dst.writeByte (char (0xc0 | run));
                dst.writeByte (char (value));
            }
            else
            {
                dst.writeByte (char (value));
            }

            x += run;
        }
    };

    juce::HeapBlock<uint8_t> line (size_t (bytesPerLine), true);

    for (int y = 0; y < h; y++)
    {
        for (int plane = 0; plane < 3; plane++)
        {
            for (int x = 0; x < w; x++)
            {
                auto* p = (juce::PixelARGB*) data.getPixelPointer (x, y);

                line[x] = plane == 0 ? p->getRed() : (plane == 1 ? p->getGreen() : p->getBlue());
            }

            if (bytesPerLine > w)
                line[w] = 0;

            writePlane (line);
        }
    }

    return true;
}

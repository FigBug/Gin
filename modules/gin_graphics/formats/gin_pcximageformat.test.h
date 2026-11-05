//==============================================================================
#if GIN_UNIT_TESTS

class PCXImageFormatTests : public juce::UnitTest
{
public:
    PCXImageFormatTests() : juce::UnitTest ("PCX Image Format", "gin_graphics") {}

    void runTest() override
    {
        testFormatName();
        testFileExtension();
        testRoundTrip();
        testTrueColour();
        testPalettes();
        testPlanarEGA();
        testUncompressed();
        testCanUnderstand();
    }

private:
    /** A PCX built to order. The scanline data is given already encoded the way the
        header describes, and a 256 colour palette is appended with its marker.
    */
    static juce::MemoryBlock buildPCX (int w, int h, int bitsPerPlane, int planes,
                                       int bytesPerLine, const juce::MemoryBlock& scanlines,
                                       int encoding = 1,
                                       const juce::Array<juce::Colour>& egaPalette = {},
                                       const juce::Array<juce::Colour>& vgaPalette = {})
    {
        juce::MemoryOutputStream os;

        os.writeByte (0x0a);
        os.writeByte (5);
        os.writeByte (char (encoding));
        os.writeByte (char (bitsPerPlane));
        os.writeShort (0);
        os.writeShort (0);
        os.writeShort (short (w - 1));
        os.writeShort (short (h - 1));
        os.writeShort (72);
        os.writeShort (72);

        for (int i = 0; i < 16; i++)
        {
            const auto c = i < egaPalette.size() ? egaPalette[i] : juce::Colours::black;

            os.writeByte (char (c.getRed()));
            os.writeByte (char (c.getGreen()));
            os.writeByte (char (c.getBlue()));
        }

        os.writeByte (0);
        os.writeByte (char (planes));
        os.writeShort (short (bytesPerLine));
        os.writeShort (1);
        os.writeShort (0);
        os.writeShort (0);

        for (int i = 0; i < 54; i++)
            os.writeByte (0);

        os.write (scanlines.getData(), scanlines.getSize());

        if (! vgaPalette.isEmpty())
        {
            os.writeByte (0x0c);

            for (int i = 0; i < 256; i++)
            {
                const auto c = i < vgaPalette.size() ? vgaPalette[i] : juce::Colours::black;

                os.writeByte (char (c.getRed()));
                os.writeByte (char (c.getGreen()));
                os.writeByte (char (c.getBlue()));
            }
        }

        return os.getMemoryBlock();
    }

    static juce::MemoryBlock bytes (const std::vector<uint8_t>& v)
    {
        return juce::MemoryBlock (v.data(), v.size());
    }

    static juce::Image decode (const juce::MemoryBlock& mb)
    {
        PCXImageFormat format;
        juce::MemoryInputStream in (mb.getData(), mb.getSize(), false);
        return format.decodeImage (in);
    }

    void expectPixel (const juce::Image& img, int x, int y, juce::Colour expected, const juce::String& what)
    {
        if (! img.isValid())
        {
            expect (false, what + ": the image failed to decode");
            return;
        }

        const auto actual = img.getPixelAt (x, y);
        const auto close = std::abs (actual.getRed()   - expected.getRed())   <= 2
                        && std::abs (actual.getGreen() - expected.getGreen()) <= 2
                        && std::abs (actual.getBlue()  - expected.getBlue())  <= 2;

        expect (close, what + ": expected " + expected.toString() + ", got " + actual.toString());
    }

    void testFormatName()
    {
        beginTest ("Format Name");

        PCXImageFormat format;
        expect (format.getFormatName() == "ZSoft PCX", "Should return correct format name");
    }

    void testFileExtension()
    {
        beginTest ("File Extension");

        PCXImageFormat format;
        const auto dir = juce::File::getCurrentWorkingDirectory();

        expect (format.usesFileExtension (dir.getChildFile ("test.pcx")), "Recognises .pcx");
        expect (! format.usesFileExtension (dir.getChildFile ("test.png")), "Rejects .png");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        PCXImageFormat format;

        juce::Image original (juce::Image::ARGB, 5, 3, true);
        original.clear (original.getBounds(), juce::Colours::black);
        original.setPixelAt (0, 0, juce::Colours::red);
        original.setPixelAt (1, 0, juce::Colours::lime);
        original.setPixelAt (2, 0, juce::Colours::blue);
        original.setPixelAt (4, 2, juce::Colours::white);

        juce::MemoryOutputStream out;
        expect (format.writeImageToStream (original, out), "Writes a PCX");

        juce::MemoryInputStream in (out.getData(), out.getDataSize(), false);
        const auto decoded = format.decodeImage (in);

        expect (decoded.isValid(), "The written PCX decodes");
        expect (decoded.getWidth() == 5 && decoded.getHeight() == 3, "Its size survives");

        expectPixel (decoded, 0, 0, juce::Colours::red,   "Round trip red");
        expectPixel (decoded, 1, 0, juce::Colours::lime,  "Round trip green");
        expectPixel (decoded, 2, 0, juce::Colours::blue,  "Round trip blue");
        expectPixel (decoded, 4, 2, juce::Colours::white, "Round trip white, bottom right");
        expectPixel (decoded, 3, 1, juce::Colours::black, "Round trip black");
    }

    void testTrueColour()
    {
        beginTest ("True Colour");

        // Three planes of eight bits: a whole row of red, then of green, then of blue.
        // 0xc1 introduces a run of one, which is how a value with its top bits set is
        // escaped
        auto img = decode (buildPCX (2, 1, 8, 3, 2,
                                     bytes ({ 0xc2, 0xff,       // red plane: two of 255
                                              0xc2, 0x00,       // green plane: two of 0
                                              0xc2, 0x00 })));  // blue plane: two of 0

        expectPixel (img, 0, 0, juce::Colours::red, "A 24 bit red");
        expectPixel (img, 1, 0, juce::Colours::red, "Its neighbour");
    }

    void testPalettes()
    {
        beginTest ("Palettes");

        // 256 colours, the palette trailing the file behind its marker
        juce::Array<juce::Colour> vga;
        vga.add (juce::Colours::red);
        vga.add (juce::Colours::lime);
        vga.add (juce::Colours::blue);

        auto img = decode (buildPCX (3, 1, 8, 1, 3,
                                     bytes ({ 0x00, 0x01, 0x02 }), 1, {}, vga));

        expectPixel (img, 0, 0, juce::Colours::red,  "The first VGA palette entry");
        expectPixel (img, 1, 0, juce::Colours::lime, "The second");
        expectPixel (img, 2, 0, juce::Colours::blue, "The third");
    }

    void testPlanarEGA()
    {
        beginTest ("Planar EGA");

        // Four planes of one bit each make a four bit index into the header's palette.
        // Pixel 0 takes bit 0 only, pixel 1 takes bits 0 and 1
        juce::Array<juce::Colour> ega;

        for (int i = 0; i < 16; i++)
            ega.add (juce::Colour::fromRGB (uint8_t (i * 16), 0, 0));

        auto img = decode (buildPCX (2, 1, 1, 4, 1,
                                     bytes ({ 0xc1, 0xc0,       // plane 0: both pixels set
                                              0xc1, 0x40,       // plane 1: the second pixel
                                              0xc1, 0x00,       // plane 2: neither
                                              0xc1, 0x00 }),    // plane 3: neither
                                     1, ega));

        expectPixel (img, 0, 0, ega[1], "A planar index of one");
        expectPixel (img, 1, 0, ega[3], "A planar index of three");
    }

    void testUncompressed()
    {
        beginTest ("Uncompressed");

        // Encoding zero means the scanlines are stored as they are
        juce::Array<juce::Colour> vga { juce::Colours::red, juce::Colours::lime };

        auto img = decode (buildPCX (2, 1, 8, 1, 2, bytes ({ 0x00, 0x01 }), 0, {}, vga));

        expectPixel (img, 0, 0, juce::Colours::red,  "An uncompressed first pixel");
        expectPixel (img, 1, 0, juce::Colours::lime, "An uncompressed second pixel");
    }

    void testCanUnderstand()
    {
        beginTest ("Can Understand");

        PCXImageFormat format;

        juce::Image img (juce::Image::ARGB, 2, 2, true);
        img.clear (img.getBounds(), juce::Colours::red);

        juce::MemoryOutputStream out;
        format.writeImageToStream (img, out);

        juce::MemoryInputStream written (out.getData(), out.getDataSize(), false);
        expect (format.canUnderstand (written), "Recognises its own output");

        const auto png = bytes ({ 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
                                  0, 0, 0, 13, 'I', 'H', 'D', 'R', 0, 0, 0, 1, 0, 0, 0, 1 });
        juce::MemoryInputStream pngIn (png.getData(), png.getSize(), false);
        expect (! format.canUnderstand (pngIn), "Turns away a PNG");

        // The right first byte is not enough on its own
        juce::MemoryBlock pretend (200);
        pretend.fillWith (0);
        pretend[0] = char (0x0a);
        juce::MemoryInputStream pretendIn (pretend.getData(), pretend.getSize(), false);
        expect (! format.canUnderstand (pretendIn), "Turns away a file that only starts like one");
    }
};

static PCXImageFormatTests pcxImageFormatTests;

#endif

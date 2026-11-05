//==============================================================================
#if GIN_UNIT_TESTS

class TGAImageFormatTests : public juce::UnitTest
{
public:
    TGAImageFormatTests() : juce::UnitTest ("TGA Image Format", "gin_graphics") {}

    void runTest() override
    {
        testFormatName();
        testFileExtension();
        testRoundTrip();
        testTrueColour();
        testSixteenBit();
        testGreyscale();
        testColourMapped();
        testRunLengthEncoded();
        testOrigin();
        testCanUnderstand();
        testReferenceFile();
    }

private:
    /** A TGA built to order, so each variant the decoder claims to read can be handed
        to it. The pixel data is given already in the format the header describes.
    */
    static juce::MemoryBlock buildTGA (int w, int h, int imageType, int pixelDepth,
                                       const juce::MemoryBlock& pixels,
                                       uint8_t descriptor = 0,
                                       const juce::Array<juce::Colour>& colourMap = {},
                                       int colourMapDepth = 24)
    {
        juce::MemoryOutputStream os;

        os.writeByte (0);                                           // no id field
        os.writeByte (colourMap.isEmpty() ? 0 : 1);
        os.writeByte (char (imageType));
        os.writeShort (0);                                          // colour map origin
        os.writeShort (short (colourMap.size()));
        os.writeByte (char (colourMap.isEmpty() ? 0 : colourMapDepth));
        os.writeShort (0);                                          // x origin
        os.writeShort (0);                                          // y origin
        os.writeShort (short (w));
        os.writeShort (short (h));
        os.writeByte (char (pixelDepth));
        os.writeByte (char (descriptor));

        for (auto c : colourMap)
        {
            os.writeByte (char (c.getBlue()));
            os.writeByte (char (c.getGreen()));
            os.writeByte (char (c.getRed()));

            if (colourMapDepth == 32)
                os.writeByte (char (c.getAlpha()));
        }

        os.write (pixels.getData(), pixels.getSize());

        return os.getMemoryBlock();
    }

    static juce::MemoryBlock bytes (const std::vector<uint8_t>& v)
    {
        return juce::MemoryBlock (v.data(), v.size());
    }

    static juce::Image decode (const juce::MemoryBlock& mb)
    {
        TGAImageFormat format;
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
                        && std::abs (actual.getBlue()  - expected.getBlue())  <= 2
                        && std::abs (actual.getAlpha() - expected.getAlpha()) <= 2;

        expect (close, what + ": expected " + expected.toString() + ", got " + actual.toString());
    }

    void testFormatName()
    {
        beginTest ("Format Name");

        TGAImageFormat format;
        expect (format.getFormatName() == "Truevision TGA", "Should return correct format name");
    }

    void testFileExtension()
    {
        beginTest ("File Extension");

        TGAImageFormat format;
        const auto dir = juce::File::getCurrentWorkingDirectory();

        expect (format.usesFileExtension (dir.getChildFile ("test.tga")), "Recognises .tga");
        expect (format.usesFileExtension (dir.getChildFile ("test.targa")), "Recognises .targa");
        expect (! format.usesFileExtension (dir.getChildFile ("test.png")), "Rejects .png");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        TGAImageFormat format;

        juce::Image original (juce::Image::ARGB, 4, 3, true);
        original.setPixelAt (0, 0, juce::Colours::red);
        original.setPixelAt (1, 0, juce::Colours::lime);
        original.setPixelAt (2, 0, juce::Colours::blue);
        original.setPixelAt (3, 2, juce::Colours::white);

        juce::MemoryOutputStream out;
        expect (format.writeImageToStream (original, out), "Writes a TGA");

        juce::MemoryInputStream in (out.getData(), out.getDataSize(), false);
        const auto decoded = format.decodeImage (in);

        expect (decoded.isValid(), "The written TGA decodes");
        expect (decoded.getWidth() == 4 && decoded.getHeight() == 3, "Its size survives");

        expectPixel (decoded, 0, 0, juce::Colours::red,   "Round trip red");
        expectPixel (decoded, 1, 0, juce::Colours::lime,  "Round trip green");
        expectPixel (decoded, 2, 0, juce::Colours::blue,  "Round trip blue");
        expectPixel (decoded, 3, 2, juce::Colours::white, "Round trip white, bottom right");
    }

    void testTrueColour()
    {
        beginTest ("True Colour");

        // 24 bit, stored blue first and bottom up
        {
            auto img = decode (buildTGA (2, 1, 2, 24, bytes ({ 0x00, 0x00, 0xff,
                                                               0xff, 0x00, 0x00 })));

            expectPixel (img, 0, 0, juce::Colours::red,  "A 24 bit red");
            expectPixel (img, 1, 0, juce::Colours::blue, "A 24 bit blue");
        }

        // 32 bit carries alpha
        {
            auto img = decode (buildTGA (1, 1, 2, 32, bytes ({ 0x00, 0x00, 0xff, 0x80 })));
            expectPixel (img, 0, 0, juce::Colours::red.withAlpha (128.0f / 255.0f), "A 32 bit alpha");
        }
    }

    void testSixteenBit()
    {
        beginTest ("Sixteen Bit");

        // Five bits a channel, and the brightest value has to reach 255
        auto img = decode (buildTGA (2, 1, 2, 16, bytes ({ 0x00, 0xfc, 0x1f, 0x80 })));

        expectPixel (img, 0, 0, juce::Colour::fromRGB (255, 0, 0), "A 16 bit red");
        expectPixel (img, 1, 0, juce::Colour::fromRGB (0, 0, 255), "A 16 bit blue");
    }

    void testGreyscale()
    {
        beginTest ("Greyscale");

        auto img = decode (buildTGA (2, 1, 3, 8, bytes ({ 0x00, 0xff })));

        expectPixel (img, 0, 0, juce::Colours::black, "A greyscale black");
        expectPixel (img, 1, 0, juce::Colours::white, "A greyscale white");
    }

    void testColourMapped()
    {
        beginTest ("Colour Mapped");

        const juce::Array<juce::Colour> map { juce::Colours::red, juce::Colours::lime, juce::Colours::blue };

        auto img = decode (buildTGA (3, 1, 1, 8, bytes ({ 0x00, 0x01, 0x02 }), 0, map));

        expectPixel (img, 0, 0, juce::Colours::red,  "A mapped red");
        expectPixel (img, 1, 0, juce::Colours::lime, "A mapped green");
        expectPixel (img, 2, 0, juce::Colours::blue, "A mapped blue");
    }

    void testRunLengthEncoded()
    {
        beginTest ("Run Length Encoded");

        // A packet with its top bit set repeats one pixel, otherwise it introduces that
        // many literal ones. Four pixels: two of red, then blue and green
        auto img = decode (buildTGA (4, 1, 10, 24,
                                     bytes ({ 0x81, 0x00, 0x00, 0xff,            // a run of two reds
                                              0x01, 0xff, 0x00, 0x00,            // two literals
                                                    0x00, 0xff, 0x00 }),
                                     0x20));                                     // top to bottom

        expectPixel (img, 0, 0, juce::Colours::red,  "An RLE run, first pixel");
        expectPixel (img, 1, 0, juce::Colours::red,  "An RLE run, second pixel");
        expectPixel (img, 2, 0, juce::Colours::blue, "An RLE literal, first pixel");
        expectPixel (img, 3, 0, juce::Colour::fromRGB (0, 255, 0), "An RLE literal, second pixel");
    }

    void testOrigin()
    {
        beginTest ("Origin");

        const auto pixels = bytes ({ 0x00, 0x00, 0xff,      // red
                                     0xff, 0x00, 0x00 });   // blue

        // Bottom up by default, which is the format's origin at the lower left
        auto bottomUp = decode (buildTGA (1, 2, 2, 24, pixels, 0));
        expectPixel (bottomUp, 0, 1, juce::Colours::red,  "The first row of a bottom up TGA is the bottom");
        expectPixel (bottomUp, 0, 0, juce::Colours::blue, "The second is the top");

        // Bit five of the descriptor flips it
        auto topDown = decode (buildTGA (1, 2, 2, 24, pixels, 0x20));
        expectPixel (topDown, 0, 0, juce::Colours::red,  "The first row of a top down TGA is the top");
        expectPixel (topDown, 0, 1, juce::Colours::blue, "The second is the bottom");

        // Bit four mirrors the columns
        auto rightToLeft = decode (buildTGA (2, 1, 2, 24, pixels, 0x10));
        expectPixel (rightToLeft, 1, 0, juce::Colours::red,  "The first pixel of a right to left TGA is the last");
        expectPixel (rightToLeft, 0, 0, juce::Colours::blue, "The second is the first");
    }

    void testCanUnderstand()
    {
        beginTest ("Can Understand");

        TGAImageFormat format;

        // What this format writes carries the version 2 footer, which settles it
        juce::Image img (juce::Image::ARGB, 2, 2, true);
        img.clear (img.getBounds(), juce::Colours::red);

        juce::MemoryOutputStream out;
        format.writeImageToStream (img, out);

        juce::MemoryInputStream written (out.getData(), out.getDataSize(), false);
        expect (format.canUnderstand (written), "Recognises its own output by the footer");

        // A header without the footer still has to be recognised on its own merits
        const auto plain = buildTGA (2, 1, 2, 24, bytes ({ 0, 0, 0, 0, 0, 0 }));
        juce::MemoryInputStream plainIn (plain.getData(), plain.getSize(), false);
        expect (format.canUnderstand (plainIn), "Recognises a footerless TGA by its header");

        // And something that is plainly not a TGA has to be turned away
        const auto png = bytes ({ 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
                                  0, 0, 0, 13, 'I', 'H', 'D', 'R', 0, 0, 0, 1, 0, 0, 0, 1 });
        juce::MemoryInputStream pngIn (png.getData(), png.getSize(), false);
        expect (! format.canUnderstand (pngIn), "Turns away a PNG");
    }

    //==============================================================================
    /** A file written by something other than this code. Produced by macOS sips from a
        4x2 PNG, and it happens to be a run length encoded, top down, 24 bit TGA.
    */
    void testReferenceFile()
    {
        beginTest ("Reference File");

        static const unsigned char sipsTGA[]
        {
            0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 2, 0, 24, 32, 3, 0, 0, 255, 0, 255, 0, 255, 
            0, 0, 255, 255, 255, 3, 0, 0, 0, 128, 128, 128, 0, 0, 255, 255, 0, 0
        };

        const juce::Colour expected[8]
        {
            juce::Colours::red, juce::Colour::fromRGB (0, 255, 0), juce::Colours::blue, juce::Colours::white,
            juce::Colours::black, juce::Colour::fromRGB (128, 128, 128), juce::Colours::red, juce::Colours::blue
        };

        TGAImageFormat format;
        juce::MemoryInputStream in (sipsTGA, sizeof (sipsTGA), false);
        const auto img = format.decodeImage (in);

        expect (img.isValid(), "A TGA from another encoder decodes");
        expect (img.getWidth() == 4 && img.getHeight() == 2, "Its size is read correctly");

        for (int i = 0; i < 8; i++)
            expectPixel (img, i % 4, i / 4, expected[i], "Reference TGA pixel " + juce::String (i));
    }
};

static TGAImageFormatTests tgaImageFormatTests;

#endif

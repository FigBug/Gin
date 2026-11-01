//==============================================================================
#if GIN_UNIT_TESTS

class BMPImageFormatTests : public juce::UnitTest
{
public:
    BMPImageFormatTests() : juce::UnitTest ("BMP Image Format", "gin_graphics") {}

    void runTest() override
    {
        testFormatName();
        testFileExtension();
        testCanUnderstand();
        testRoundTrip();
        testDifferentSizes();
        testPixelAccuracy();
        testIndexedDepths();
        testSixteenBit();
        testThirtyTwoBit();
        testRunLengthEncoded();
        testTopDown();
        testCoreHeader();
        testRejectsUnsupported();
        testReferenceFile();
        testAlphaRoundTrip();
    }

private:
    void testFormatName()
    {
        beginTest ("Format Name");

        BMPImageFormat format;
        expect (format.getFormatName() == "Windows Bitmap", "Should return correct format name");
    }

    void testFileExtension()
    {
        beginTest ("File Extension");

        BMPImageFormat format;

        juce::File bmpFile = juce::File::getCurrentWorkingDirectory().getChildFile ("test.bmp");
        juce::File pngFile = juce::File::getCurrentWorkingDirectory().getChildFile ("test.png");
        juce::File jpgFile = juce::File::getCurrentWorkingDirectory().getChildFile ("test.jpg");

        expect (format.usesFileExtension (bmpFile), "Should recognize .bmp extension");
        expect (!format.usesFileExtension (pngFile), "Should not recognize .png extension");
        expect (!format.usesFileExtension (jpgFile), "Should not recognize .jpg extension");
    }

    void testCanUnderstand()
    {
        beginTest ("Can Understand");

        BMPImageFormat format;

        // Create a minimal BMP header
        juce::MemoryOutputStream validBMP;
        validBMP.writeByte ('B');
        validBMP.writeByte ('M');

        juce::MemoryInputStream validInput (validBMP.getData(), validBMP.getDataSize(), false);
        expect (format.canUnderstand (validInput), "Should recognize valid BMP header");

        // Test invalid header
        juce::MemoryOutputStream invalidBMP;
        invalidBMP.writeByte ('P');
        invalidBMP.writeByte ('N');

        juce::MemoryInputStream invalidInput (invalidBMP.getData(), invalidBMP.getDataSize(), false);
        expect (!format.canUnderstand (invalidInput), "Should reject invalid header");
    }

    void testRoundTrip()
    {
        beginTest ("Round Trip");

        BMPImageFormat format;

        // Create a simple test image
        juce::Image original (juce::Image::ARGB, 10, 10, true);
        juce::Graphics g (original);
        g.fillAll (juce::Colours::red);

        // Write to stream
        juce::MemoryOutputStream output;
        bool writeSuccess = format.writeImageToStream (original, output);
        expect (writeSuccess, "Should write image successfully");
        expect (output.getDataSize() > 0, "Should produce output data");

        // Read back
        juce::MemoryInputStream input (output.getData(), output.getDataSize(), false);
        juce::Image decoded = format.decodeImage (input);

        expect (decoded.isValid(), "Decoded image should be valid");
        expect (decoded.getWidth() == original.getWidth(), "Width should match");
        expect (decoded.getHeight() == original.getHeight(), "Height should match");
    }

    void testDifferentSizes()
    {
        beginTest ("Different Sizes");

        BMPImageFormat format;

        // Test various image sizes
        int sizes[] = { 1, 2, 10, 50, 100 };

        for (int size : sizes)
        {
            juce::Image img (juce::Image::ARGB, size, size, true);
            img.clear (img.getBounds(), juce::Colours::blue);

            juce::MemoryOutputStream output;
            format.writeImageToStream (img, output);

            juce::MemoryInputStream input (output.getData(), output.getDataSize(), false);
            juce::Image decoded = format.decodeImage (input);

            expect (decoded.isValid(), juce::String ("Image of size ") + juce::String (size) + " should be valid");
            expect (decoded.getWidth() == size, "Width should match for size " + juce::String (size));
            expect (decoded.getHeight() == size, "Height should match for size " + juce::String (size));
        }
    }

    void testPixelAccuracy()
    {
        beginTest ("Pixel Accuracy");

        BMPImageFormat format;

        // Create image with specific colors
        juce::Image original (juce::Image::ARGB, 3, 3, true);

        original.setPixelAt (0, 0, juce::Colours::red);
        original.setPixelAt (1, 0, juce::Colours::green);
        original.setPixelAt (2, 0, juce::Colours::blue);
        original.setPixelAt (0, 1, juce::Colours::white);
        original.setPixelAt (1, 1, juce::Colours::black);
        original.setPixelAt (2, 1, juce::Colours::yellow);

        // Write and read back
        juce::MemoryOutputStream output;
        format.writeImageToStream (original, output);

        juce::MemoryInputStream input (output.getData(), output.getDataSize(), false);
        juce::Image decoded = format.decodeImage (input);

        // Check specific pixels (allowing for some tolerance due to format conversion)
        auto checkPixel = [&] (int x, int y, juce::Colour expected, const juce::String& name)
        {
            auto actual = decoded.getPixelAt (x, y);
            int tolerance = 2;

            bool rMatch = std::abs (actual.getRed() - expected.getRed()) <= tolerance;
            bool gMatch = std::abs (actual.getGreen() - expected.getGreen()) <= tolerance;
            bool bMatch = std::abs (actual.getBlue() - expected.getBlue()) <= tolerance;

            expect (rMatch && gMatch && bMatch,
                    name + " pixel should match (expected " + expected.toString() +
                    ", got " + actual.toString() + ")");
        };

        checkPixel (0, 0, juce::Colours::red, "Red");
        checkPixel (1, 0, juce::Colours::green, "Green");
        checkPixel (2, 0, juce::Colours::blue, "Blue");
        checkPixel (0, 1, juce::Colours::white, "White");
        checkPixel (1, 1, juce::Colours::black, "Black");
    }

    //==============================================================================
    /** A bitmap with a plain 40 byte info header, built to order so each variant the
        decoder claims to read can be handed to it
    */
    static juce::MemoryBlock buildBMP (int w, int h, int bpp, uint32_t compression,
                                       const juce::Array<juce::Colour>& palette,
                                       const juce::MemoryBlock& pixels,
                                       bool topDown = false,
                                       const uint32_t* masks = nullptr)
    {
        juce::MemoryOutputStream os;

        const auto maskBytes   = masks != nullptr ? (masks[3] != 0 ? 16 : 12) : 0;
        const auto dataOffset  = 14 + 40 + maskBytes + palette.size() * 4;

        os.writeByte ('B');
        os.writeByte ('M');
        os.writeInt (dataOffset + int (pixels.getSize()));
        os.writeShort (0);
        os.writeShort (0);
        os.writeInt (dataOffset);

        os.writeInt (40);
        os.writeInt (w);
        os.writeInt (topDown ? -h : h);
        os.writeShort (1);
        os.writeShort (short (bpp));
        os.writeInt (int (compression));
        os.writeInt (int (pixels.getSize()));
        os.writeInt (2835);
        os.writeInt (2835);
        os.writeInt (palette.size());
        os.writeInt (0);

        if (masks != nullptr)
        {
            os.writeInt (int (masks[0]));
            os.writeInt (int (masks[1]));
            os.writeInt (int (masks[2]));

            if (masks[3] != 0)
                os.writeInt (int (masks[3]));
        }

        for (auto c : palette)
        {
            os.writeByte (char (c.getBlue()));
            os.writeByte (char (c.getGreen()));
            os.writeByte (char (c.getRed()));
            os.writeByte (0);
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
        BMPImageFormat format;
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

    void testIndexedDepths()
    {
        beginTest ("Indexed Depths");

        const juce::Array<juce::Colour> four { juce::Colours::black, juce::Colours::red,
                                               juce::Colours::green, juce::Colours::blue };

        // One bit, eight pixels alternating between the two palette entries, the row
        // padded out to four bytes
        {
            const juce::Array<juce::Colour> two { juce::Colours::black, juce::Colours::white };
            auto img = decode (buildBMP (8, 1, 1, 0, two, bytes ({ 0xaa, 0, 0, 0 })));

            expect (img.getWidth() == 8, "A one bit bitmap is eight pixels wide");

            for (int x = 0; x < 8; x++)
                expectPixel (img, x, 0, (x % 2) == 0 ? juce::Colours::white : juce::Colours::black,
                             "One bit pixel " + juce::String (x));
        }

        // Four bit, two indices to a byte
        {
            auto img = decode (buildBMP (4, 1, 4, 0, four, bytes ({ 0x01, 0x23, 0, 0 })));

            for (int x = 0; x < 4; x++)
                expectPixel (img, x, 0, four[x], "Four bit pixel " + juce::String (x));
        }

        // Eight bit, one index to a byte
        {
            auto img = decode (buildBMP (4, 1, 8, 0, four, bytes ({ 0x00, 0x01, 0x02, 0x03 })));

            for (int x = 0; x < 4; x++)
                expectPixel (img, x, 0, four[x], "Eight bit pixel " + juce::String (x));
        }
    }

    void testSixteenBit()
    {
        beginTest ("Sixteen Bit");

        // Without masks the channels are five bits each, and the brightest value of a
        // channel has to reach 255 rather than stopping short
        {
            auto img = decode (buildBMP (2, 1, 16, 0, {}, bytes ({ 0x00, 0x7c, 0x1f, 0x00 })));

            expectPixel (img, 0, 0, juce::Colours::red,  "A 555 red");
            expectPixel (img, 1, 0, juce::Colours::blue, "A 555 blue");
        }

        // With masks, the common 565 layout
        {
            const uint32_t masks[4] = { 0xf800, 0x07e0, 0x001f, 0 };
            auto img = decode (buildBMP (2, 1, 16, 3, {}, bytes ({ 0x00, 0xf8, 0xe0, 0x07 }), false, masks));

            // Every bit of the channel set is full brightness, not CSS green's half
            expectPixel (img, 0, 0, juce::Colour::fromRGB (255, 0, 0), "A 565 red");
            expectPixel (img, 1, 0, juce::Colour::fromRGB (0, 255, 0), "A 565 green");
        }
    }

    void testThirtyTwoBit()
    {
        beginTest ("Thirty Two Bit");

        // The fourth byte of an uncompressed 32 bit pixel is reserved, and writers that
        // leave it at zero must not decode to an invisible image
        {
            auto img = decode (buildBMP (1, 1, 32, 0, {}, bytes ({ 0x00, 0x00, 0xff, 0x00 })));
            expectPixel (img, 0, 0, juce::Colours::red, "A reserved byte of zero is not transparency");
        }

        // An explicit alpha mask is alpha, including where it really is zero
        {
            const uint32_t masks[4] = { 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };
            auto img = decode (buildBMP (2, 1, 32, 6, {},
                                         bytes ({ 0x00, 0x00, 0xff, 0x80,
                                                  0x00, 0x00, 0xff, 0x00 }), false, masks));

            expectPixel (img, 0, 0, juce::Colours::red.withAlpha (128.0f / 255.0f), "A masked alpha of 128");

            // Once some pixel carries alpha the mask is believed, so a zero really is
            // transparent, and a transparent pixel has no colour left to read back
            expect (img.getPixelAt (1, 0).getAlpha() == 0, "A masked alpha of zero stays transparent");
        }
    }

    void testRunLengthEncoded()
    {
        beginTest ("Run Length Encoded");

        const juce::Array<juce::Colour> four { juce::Colours::black, juce::Colours::red,
                                               juce::Colours::green, juce::Colours::blue };

        // Eight bit runs, a literal run, and the end of line and end of file markers. The
        // rows run bottom up, so the first line encoded is the last line of the image
        {
            auto img = decode (buildBMP (4, 2, 8, 1, four,
                                         bytes ({ 0x02, 0x01,                      // two of red
                                                  0x02, 0x02,                      // two of green
                                                  0x00, 0x00,                      // end of line
                                                  0x00, 0x04, 0x03, 0x03, 0x01, 0x01,  // a literal run, already even
                                                  0x00, 0x01 })));                 // end of file

            expectPixel (img, 0, 1, juce::Colours::red,   "RLE8 run, first pixel");
            expectPixel (img, 1, 1, juce::Colours::red,   "RLE8 run, second pixel");
            expectPixel (img, 2, 1, juce::Colours::green, "RLE8 second run");
            expectPixel (img, 0, 0, juce::Colours::blue,  "RLE8 literal, first pixel");
            expectPixel (img, 3, 0, juce::Colours::red,   "RLE8 literal, last pixel");
        }

        // Four bit runs alternate between the two indices packed in the byte
        {
            auto img = decode (buildBMP (4, 1, 4, 2, four,
                                         bytes ({ 0x04, 0x12, 0x00, 0x01 })));

            expectPixel (img, 0, 0, juce::Colours::red,   "RLE4 even pixel");
            expectPixel (img, 1, 0, juce::Colours::green, "RLE4 odd pixel");
            expectPixel (img, 2, 0, juce::Colours::red,   "RLE4 even pixel again");
            expectPixel (img, 3, 0, juce::Colours::green, "RLE4 odd pixel again");
        }
    }

    void testTopDown()
    {
        beginTest ("Top Down");

        // A negative height means the first row encoded is the top one. Each row of a 1px
        // wide 24 bit image pads out to four bytes
        auto topDown = decode (buildBMP (1, 2, 24, 0, {},
                                         bytes ({ 0x00, 0x00, 0xff, 0x00,
                                                  0xff, 0x00, 0x00, 0x00 }), true));

        expectPixel (topDown, 0, 0, juce::Colours::red,  "The first row of a top down bitmap is the top");
        expectPixel (topDown, 0, 1, juce::Colours::blue, "The second is the bottom");

        auto bottomUp = decode (buildBMP (1, 2, 24, 0, {},
                                          bytes ({ 0x00, 0x00, 0xff, 0x00,
                                                   0xff, 0x00, 0x00, 0x00 }), false));

        expectPixel (bottomUp, 0, 1, juce::Colours::red,  "The first row of a bottom up bitmap is the bottom");
        expectPixel (bottomUp, 0, 0, juce::Colours::blue, "The second is the top");
    }

    void testCoreHeader()
    {
        beginTest ("Core Header");

        // The OS/2 header is twelve bytes, its dimensions are shorts and its palette
        // entries are three bytes rather than four
        juce::MemoryOutputStream os;

        const auto dataOffset = 14 + 12 + 2 * 3;

        os.writeByte ('B');
        os.writeByte ('M');
        os.writeInt (dataOffset + 4);
        os.writeShort (0);
        os.writeShort (0);
        os.writeInt (dataOffset);
        os.writeInt (12);
        os.writeShort (2);
        os.writeShort (1);
        os.writeShort (1);
        os.writeShort (8);

        for (auto c : { juce::Colours::red, juce::Colours::green })
        {
            os.writeByte (char (c.getBlue()));
            os.writeByte (char (c.getGreen()));
            os.writeByte (char (c.getRed()));
        }

        os.writeByte (0);
        os.writeByte (1);
        os.writeByte (0);
        os.writeByte (0);

        auto img = decode (os.getMemoryBlock());

        expect (img.isValid() && img.getWidth() == 2, "An OS/2 header decodes");
        expectPixel (img, 0, 0, juce::Colours::red,   "Its first palette entry");
        expectPixel (img, 1, 0, juce::Colours::green, "Its second");
    }

    void testRejectsUnsupported()
    {
        beginTest ("Rejects Unsupported");

        // A BMP wrapping a whole JPEG or PNG, and a nonsense bit depth, have to come back
        // invalid rather than being decoded as something else
        expect (! decode (buildBMP (2, 2, 24, 4, {}, bytes ({ 0, 0, 0, 0 }))).isValid(), "A JPEG payload is refused");
        expect (! decode (buildBMP (2, 2, 24, 5, {}, bytes ({ 0, 0, 0, 0 }))).isValid(), "A PNG payload is refused");
        expect (! decode (buildBMP (2, 2, 7,  0, {}, bytes ({ 0, 0, 0, 0 }))).isValid(), "An odd bit depth is refused");
        expect (! decode (buildBMP (0, 2, 24, 0, {}, bytes ({ 0, 0, 0, 0 }))).isValid(), "A zero width is refused");
    }

    /** What this module writes is a 32 bit BI_RGB bitmap with alpha in the reserved byte,
        so its own transparency has to survive a round trip
    */
    void testAlphaRoundTrip()
    {
        beginTest ("Alpha Round Trip");

        BMPImageFormat format;

        juce::Image original (juce::Image::ARGB, 3, 1, true);
        original.setPixelAt (0, 0, juce::Colours::red);
        original.setPixelAt (1, 0, juce::Colours::red.withAlpha (0.5f));
        original.setPixelAt (2, 0, juce::Colours::red.withAlpha (0.0f));

        juce::MemoryOutputStream out;
        format.writeImageToStream (original, out);

        juce::MemoryInputStream in (out.getData(), out.getDataSize(), false);
        const auto decoded = format.decodeImage (in);

        expect (decoded.isValid(), "The written BMP decodes");
        expect (decoded.getPixelAt (0, 0).getAlpha() == 255, "An opaque pixel stays opaque");
        expect (std::abs (int (decoded.getPixelAt (1, 0).getAlpha()) - 128) <= 1, "A half transparent pixel keeps its alpha");
        expect (decoded.getPixelAt (2, 0).getAlpha() == 0, "A transparent pixel stays transparent");

        // An image with no alpha at all must not come back wholly invisible
        juce::Image opaque (juce::Image::ARGB, 2, 1, true);
        opaque.clear (opaque.getBounds(), juce::Colours::blue);

        juce::MemoryOutputStream out2;
        format.writeImageToStream (opaque, out2);

        juce::MemoryInputStream in2 (out2.getData(), out2.getDataSize(), false);
        expect (format.decodeImage (in2).getPixelAt (0, 0).getAlpha() == 255, "A fully opaque image stays visible");
    }

    //==============================================================================
    /** A file written by something other than this code, so the decoder is held against a
        real encoder's output rather than only against a builder written alongside it.
        Produced by macOS sips from a 4x2 PNG, and it happens to be a top down 24 bit BMP.
    */
    void testReferenceFile()
    {
        beginTest ("Reference File");

        static const unsigned char sipsBMP[]
        {
            66, 77, 78, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0, 40, 0, 0, 0, 4, 0, 0, 0, 254, 255, 255, 
            255, 1, 0, 24, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 0, 0, 0, 128, 128, 128, 0, 0, 255, 
            255, 0, 0
        };

        BMPImageFormat format;
        juce::MemoryInputStream in (sipsBMP, sizeof (sipsBMP), false);
        const auto img = format.decodeImage (in);

        expect (img.isValid(), "A BMP from another encoder decodes");
        expect (img.getWidth() == 4 && img.getHeight() == 2, "Its size is read correctly");

        for (int i = 0; i < 8; i++)
            expectPixel (img, i % 4, i / 4, referencePixel (i), "Reference BMP pixel " + juce::String (i));
    }

    /** The eight pixels of the reference image, which every reference file holds */
    static juce::Colour referencePixel (int i)
    {
        const juce::Colour pixels[8]
        {
            juce::Colours::red, juce::Colour::fromRGB (0, 255, 0), juce::Colours::blue, juce::Colours::white,
            juce::Colours::black, juce::Colour::fromRGB (128, 128, 128), juce::Colours::red, juce::Colours::blue
        };

        return pixels[i];
    }
};

static BMPImageFormatTests bmpImageFormatTests;

#endif

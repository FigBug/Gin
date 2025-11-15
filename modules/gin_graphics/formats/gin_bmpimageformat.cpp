/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

struct BMPHeader
{
    uint16_t        magic;
    uint32_t        fileSize;
    uint16_t        reserved1;
    uint16_t        reserved2;
    uint32_t        dataOffset;
    uint32_t        headerSize;
    int32_t         width;
    int32_t         height;
    uint16_t        planes;
    uint16_t        bitsPerPixel;
    uint32_t        compression;
    uint32_t        imageDataSize;
    int32_t         hPixelsPerMeter;
    int32_t         vPixelsPerMeter;
    uint32_t        coloursUsed;
    uint32_t        coloursRequired;
};

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
    BMPHeader hdr;
    hdr.magic           = uint16_t (input.readShort());
    hdr.fileSize        = uint32_t (input.readInt());
    hdr.reserved1       = uint16_t (input.readShort());
    hdr.reserved2       = uint16_t (input.readShort());
    hdr.dataOffset      = uint32_t (input.readInt());
    hdr.headerSize      = uint32_t (input.readInt());
    hdr.width           = int32_t (input.readInt());
    hdr.height          = int32_t (input.readInt());
    hdr.planes          = uint16_t (input.readShort());
    hdr.bitsPerPixel    = uint16_t (input.readShort());
    hdr.compression     = uint32_t (input.readInt());
    hdr.imageDataSize   = uint32_t (input.readInt());
    hdr.hPixelsPerMeter = int32_t (input.readInt());
    hdr.vPixelsPerMeter = int32_t (input.readInt());
    hdr.coloursUsed     = uint32_t (input.readInt());
    hdr.coloursRequired = uint32_t (input.readInt());

    if (hdr.compression != 0 || (hdr.bitsPerPixel != 8 && hdr.bitsPerPixel != 24 && hdr.bitsPerPixel != 32))
    {
        jassertfalse; // Unsupported BMP format
        return juce::Image();
    }

    if (hdr.bitsPerPixel == 8 && hdr.coloursUsed == 0)
        hdr.coloursUsed = 256;

    juce::Array<juce::PixelARGB> colourTable;

    for (int i = 0; i < int (hdr.coloursUsed); i++)
    {
        auto b = uint8_t (input.readByte());
        auto g = uint8_t (input.readByte());
        auto r = uint8_t (input.readByte());
        input.readByte();

        colourTable.add (juce::PixelARGB (255, r, g, b));
    }

    bool bottomUp = hdr.height < 0;
    hdr.height = std::abs (hdr.height);

    juce::Image img (juce::Image::ARGB, int (hdr.width), int (hdr.height), true);
    juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

    input.setPosition (hdr.dataOffset);

    int bytesPerPixel = hdr.bitsPerPixel / 8;
    int bytesPerRow = int (std::floor ((hdr.bitsPerPixel * hdr.width + 31) / 32.0) * 4);

    auto rowData = new uint8_t[size_t (bytesPerRow)];
    for (int y = 0; y < int (hdr.height); y++)
    {
        input.read (rowData, bytesPerRow);

        for (int x = 0; x < int (hdr.width); x++)
        {
            uint8_t* d = &rowData[x * bytesPerPixel];

            juce::PixelARGB* p = (juce::PixelARGB*)data.getPixelPointer (x, int (bottomUp ? y : hdr.height - y - 1));
            if (hdr.bitsPerPixel == 8)
                *p = colourTable[d[0]];
            else
                p->setARGB (bytesPerPixel == 4 ? d[3] : 255, d[2], d[1], d[0]);
        }
    }
    delete[] rowData;

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

//==============================================================================
#if JUCE_UNIT_TESTS

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
};

static BMPImageFormatTests bmpImageFormatTests;

#endif

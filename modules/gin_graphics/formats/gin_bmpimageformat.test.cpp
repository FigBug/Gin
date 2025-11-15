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

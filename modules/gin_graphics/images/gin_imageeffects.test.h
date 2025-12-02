/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class ImageEffectsTests : public juce::UnitTest
{
public:
    ImageEffectsTests() : juce::UnitTest ("Image Effects", "gin_graphics") {}

    void runTest() override
    {
        testGrayscale();
        testInvert();
        testSepia();
        testBrightnessContrast();
        testGamma();
        testVignette();
        testSolidColor();
    }

private:
    juce::Image createTestImage (int width, int height)
    {
        juce::Image img (juce::Image::ARGB, width, height, true);
        juce::Graphics g (img);

        // Create a gradient for testing
        juce::ColourGradient gradient (juce::Colours::black, 0, 0,
                                       juce::Colours::white, float (width), 0, false);
        g.setGradientFill (gradient);
        g.fillAll();

        return img;
    }

    void testGrayscale()
    {
        beginTest ("Grayscale");

        auto img = createTestImage (10, 10);

        // Set a known color (red)
        img.clear (img.getBounds(), juce::Colours::red);

        applyGreyScale (img);

        // After grayscale, RGB channels should be equal
        auto pixel = img.getPixelAt (5, 5);
        expect (pixel.getRed() == pixel.getGreen() &&
                pixel.getGreen() == pixel.getBlue(),
                "Grayscale should make RGB channels equal");
    }

    void testInvert()
    {
        beginTest ("Invert");

        auto img = createTestImage (10, 10);
        img.clear (img.getBounds(), juce::Colour (100, 150, 200));

        auto originalPixel = img.getPixelAt (5, 5);

        applyInvert (img);

        auto invertedPixel = img.getPixelAt (5, 5);

        // Inverted colors should sum to 255
        expectEquals (int (originalPixel.getRed() + invertedPixel.getRed()), 255,
                     "Red channel should invert correctly");
        expectEquals (int (originalPixel.getGreen() + invertedPixel.getGreen()), 255,
                     "Green channel should invert correctly");
        expectEquals (int (originalPixel.getBlue() + invertedPixel.getBlue()), 255,
                     "Blue channel should invert correctly");
    }

    void testSepia()
    {
        beginTest ("Sepia");

        auto img = createTestImage (10, 10);
        img.clear (img.getBounds(), juce::Colours::blue);

        applySepia (img);

        auto pixel = img.getPixelAt (5, 5);

        // Sepia should create warm brown tones (red > green > blue)
        expect (pixel.getRed() >= pixel.getGreen(), "Sepia should have red >= green");
        expect (pixel.getGreen() >= pixel.getBlue(), "Sepia should have green >= blue");
    }

    void testBrightnessContrast()
    {
        beginTest ("Brightness Contrast");

        auto img = createTestImage (10, 10);
        img.clear (img.getBounds(), juce::Colour (128, 128, 128));

        auto originalPixel = img.getPixelAt (5, 5);

        // Increase brightness
        applyBrightnessContrast (img, 50.0f, 0.0f);
        auto brighterPixel = img.getPixelAt (5, 5);

        expect (brighterPixel.getBrightness() > originalPixel.getBrightness(),
                "Brightness should increase");

        // Reset and test contrast
        img.clear (img.getBounds(), juce::Colour (128, 128, 128));
        applyBrightnessContrast (img, 0.0f, 50.0f);

        // Image should still be valid
        expect (img.isValid(), "Image should remain valid after processing");
    }

    void testGamma()
    {
        beginTest ("Gamma");

        auto img = createTestImage (10, 10);
        img.clear (img.getBounds(), juce::Colour (128, 128, 128));

        auto originalPixel = img.getPixelAt (5, 5);

        // Gamma < 1 should brighten
        applyGamma (img, 0.5f);
        auto brightenedPixel = img.getPixelAt (5, 5);

        expect (brightenedPixel.getBrightness() > originalPixel.getBrightness(),
                "Gamma < 1 should brighten image");

        // Reset and test gamma > 1 (should darken)
        img.clear (img.getBounds(), juce::Colour (128, 128, 128));
        applyGamma (img, 2.0f);
        auto darkenedPixel = img.getPixelAt (5, 5);

        expect (darkenedPixel.getBrightness() < originalPixel.getBrightness(),
                "Gamma > 1 should darken image");
    }

    void testVignette()
    {
        beginTest ("Vignette");

        auto img = createTestImage (100, 100);
        img.clear (img.getBounds(), juce::Colours::white);

        applyVignette (img, 0.5f, 0.8f, 0.5f);

        // Center should be brighter than edges
        auto centerPixel = img.getPixelAt (50, 50);
        auto edgePixel = img.getPixelAt (5, 5);

        expect (centerPixel.getBrightness() > edgePixel.getBrightness(),
                "Vignette should darken edges more than center");
    }

    void testSolidColor()
    {
        beginTest ("Solid Color");

        auto img = createTestImage (10, 10);

        juce::Colour testColor (100, 150, 200);
        applyColour (img, testColor);

        // All pixels should be the test color
        for (int y = 0; y < img.getHeight(); y++)
        {
            for (int x = 0; x < img.getWidth(); x++)
            {
                auto pixel = img.getPixelAt (x, y);
                expect (pixel.getRed() == testColor.getRed() &&
                       pixel.getGreen() == testColor.getGreen() &&
                       pixel.getBlue() == testColor.getBlue(),
                       "All pixels should be the solid color");
            }
        }
    }
};

static ImageEffectsTests imageEffectsTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

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
        testTransparencyPreserved();
        testSoftenOpaque();
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

    /** Weighting the neighbours by their alpha has to leave a fully opaque image exactly where
        the plain average left it, which is what every caller softening one already sees
    */
    void testSoftenOpaque()
    {
        beginTest ("Soften Opaque");

        juce::Image img (juce::Image::ARGB, 8, 8, true);

        {
            juce::Graphics g (img);
            g.setGradientFill (juce::ColourGradient (juce::Colours::black, 0, 0,
                                                     juce::Colours::white, 8, 8, false));
            g.fillAll();
        }

        const auto src = img.createCopy();
        applySoften (img);

        auto worst = 0;

        juce::Image::BitmapData srcData (src, juce::Image::BitmapData::readOnly);
        juce::Image::BitmapData outData (img, juce::Image::BitmapData::readOnly);

        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                auto expected = 0;

                for (int m = -1; m <= 1; m++)
                    for (int n = -1; n <= 1; n++)
                        expected += ((juce::PixelARGB*) srcData.getPixelPointer (juce::jlimit (0, 7, x + m),
                                                                                juce::jlimit (0, 7, y + n)))->getRed();

                auto* out = (juce::PixelARGB*) outData.getPixelPointer (x, y);

                expect (out->getAlpha() == 255, "An opaque pixel stays opaque");
                worst = std::max (worst, std::abs (int (out->getRed()) - expected / 9));
            }
        }

        expectEquals (worst, 0, "Softening an opaque image is still the plain 3x3 average");
    }

    //==============================================================================
    /** An ARGB image's components are premultiplied, so a stored component may never exceed
        the pixel's alpha, and a fully transparent pixel has to stay fully transparent. An
        effect that reads, changes and writes the components without accounting for that
        leaves colour behind in pixels meant to be invisible, which the compositor then adds:
        the halo of https://github.com/FigBug/Gin/issues/24
    */
    void testTransparencyPreserved()
    {
        beginTest ("Transparency Preserved");

        struct Effect { const char* name; std::function<void (juce::Image&)> apply; };

        juce::ColourGradient grad (juce::Colours::red, 0, 0, juce::Colours::blue, 16, 0, false);

        const std::vector<Effect> effects
        {
            { "applyGreyScale",              [] (juce::Image& i) { applyGreyScale (i); } },
            { "applyInvert",                 [] (juce::Image& i) { applyInvert (i); } },
            { "applySepia",                  [] (juce::Image& i) { applySepia (i); } },
            { "applyGamma",                  [] (juce::Image& i) { applyGamma (i, 2.0f); } },
            { "applyContrast",               [] (juce::Image& i) { applyContrast (i, 50.0f); } },
            { "applyBrightnessContrast",     [] (juce::Image& i) { applyBrightnessContrast (i, 50.0f, 0.0f); } },
            { "applyHueSaturationLightness", [] (juce::Image& i) { applyHueSaturationLightness (i, 30.0f, 20.0f, 20.0f); } },
            { "applyVignette",               [] (juce::Image& i) { applyVignette (i, 0.5f, 0.5f, 0.5f); } },
            { "applySoften",                 [] (juce::Image& i) { applySoften (i); } },
            { "applySharpen",                [] (juce::Image& i) { applySharpen (i); } },
            { "applyGradientMap",            [&] (juce::Image& i) { applyGradientMap (i, grad); } },
            { "applyColour",                 [] (juce::Image& i) { applyColour (i, juce::Colours::white.withAlpha (0.25f)); } },
        };

        // An effect can clamp its way past one arrangement, so transparency is checked both
        // beside a solid edge and surrounded by one
        const juce::Rectangle<int> hole { 6, 1, 4, 2 };

        struct Shape
        {
            const char* name;
            std::function<void (juce::Image&)> paint;
            std::function<bool (int, int)> shouldBeClear;
        };

        const std::vector<Shape> shapes
        {
            { "half",
              [] (juce::Image& i)
              {
                  juce::Graphics g (i);
                  g.setColour (juce::Colours::orange);
                  g.fillRect (8, 0, 8, 4);
              },
              [] (int x, int) { return x < 8; } },

            { "hole",
              [&] (juce::Image& i)
              {
                  {
                      juce::Graphics g (i);
                      g.setColour (juce::Colours::orange);
                      g.fillAll();
                  }

                  i.clear (hole);
              },
              [&] (int x, int y) { return hole.contains (x, y); } },
        };

        for (auto& e : effects)
        {
            for (auto& shape : shapes)
            {
                juce::Image img (juce::Image::ARGB, 16, 4, true);
                shape.paint (img);

                e.apply (img);

                auto transparentStayed = true;
                auto premultipliedValid = true;
                auto solidPartHasColour = false;

                juce::Image::BitmapData data (img, juce::Image::BitmapData::readOnly);

                for (int y = 0; y < img.getHeight(); y++)
                {
                    for (int x = 0; x < img.getWidth(); x++)
                    {
                        auto* px = (juce::PixelARGB*) data.getPixelPointer (x, y);

                        const auto a = int (px->getAlpha());
                        const auto hi = std::max ({ int (px->getRed()), int (px->getGreen()), int (px->getBlue()) });

                        if (hi > a)
                            premultipliedValid = false;

                        // applyColour fills the whole image by design, alpha included
                        if (shape.shouldBeClear (x, y) && std::string (e.name) != "applyColour" && (a != 0 || hi != 0))
                            transparentStayed = false;

                        if (! shape.shouldBeClear (x, y) && hi > 0)
                            solidPartHasColour = true;
                    }
                }

                const auto what = juce::String (e.name) + " (" + shape.name + ")";

                expect (premultipliedValid, what + " leaves no component above its pixel's alpha");
                expect (transparentStayed, what + " leaves fully transparent pixels transparent");
                expect (solidPartHasColour, what + " still did something to the solid part");
            }
        }
    }

};

static ImageEffectsTests imageEffectsTests;

#endif

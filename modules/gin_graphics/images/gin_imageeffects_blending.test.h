/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class BlendingTests : public juce::UnitTest
{
public:
    BlendingTests() : juce::UnitTest ("Blending", "gin_graphics") {}

    void runTest() override
    {
        testChannelBlendFunctions();
        testBlendModes();
        testBlendWithAlpha();
        testBlendColor();
    }

private:
    void testChannelBlendFunctions()
    {
        beginTest ("Channel Blend Functions");

        // Test normal blend
        expectEquals ((int) channelBlendNormal (100, 200), 100, "Normal should return first value");

        // Test lighten
        expectEquals ((int) channelBlendLighten (100, 200), 200, "Lighten should return brighter value");
        expectEquals ((int) channelBlendLighten (200, 100), 200, "Lighten should return brighter value");

        // Test darken
        expectEquals ((int) channelBlendDarken (100, 200), 100, "Darken should return darker value");
        expectEquals ((int) channelBlendDarken (200, 100), 100, "Darken should return darker value");

        // Test multiply
        auto mul = channelBlendMultiply (128, 128);
        expect (mul < 128, "Multiply should darken mid-tones");

        // Test add
        expectEquals ((int) channelBlendAdd (100, 100), 200, "Add should sum values");
        expectEquals ((int) channelBlendAdd (200, 100), 255, "Add should clamp to 255");

        // Test average
        expectEquals ((int) channelBlendAverage (100, 200), 150, "Average should be mean of values");

        // Test screen (should lighten)
        auto screen = channelBlendScreen (128, 128);
        expect (screen > 128, "Screen should lighten mid-tones");

        // Test difference
        expectEquals ((int) channelBlendDifference (200, 150), 50, "Difference should be absolute difference");
        expectEquals ((int) channelBlendDifference (150, 200), 50, "Difference should be absolute difference");
    }

    void testBlendModes()
    {
        beginTest ("Blend Modes");

        // Create two test images
        juce::Image base (juce::Image::ARGB, 10, 10, true);
        juce::Image overlay (juce::Image::ARGB, 10, 10, true);

        base.clear (base.getBounds(), juce::Colour (128, 128, 128));
        overlay.clear (overlay.getBounds(), juce::Colour (200, 100, 50));

        // Test multiply blend (should darken)
        juce::Image result = base.createCopy();
        applyBlend (result, overlay, Multiply, 1.0f, juce::Point<int> (0, 0));

        auto pixel = result.getPixelAt (5, 5);
        expect (pixel.getRed() < 128, "Multiply should darken red channel");

        // Test add blend (should brighten)
        result = base.createCopy();
        applyBlend (result, overlay, Add, 1.0f, juce::Point<int> (0, 0));

        pixel = result.getPixelAt (5, 5);
        expect (pixel.getRed() > 128, "Add should brighten red channel");

        // Test screen blend (should lighten)
        result = base.createCopy();
        applyBlend (result, overlay, Screen, 1.0f, juce::Point<int> (0, 0));

        pixel = result.getPixelAt (5, 5);
        expect (pixel.getBrightness() > base.getPixelAt (5, 5).getBrightness(),
                "Screen should lighten image");
    }

    void testBlendWithAlpha()
    {
        beginTest ("Blend With Alpha");

        juce::Image base (juce::Image::ARGB, 10, 10, true);
        juce::Image overlay (juce::Image::ARGB, 10, 10, true);

        base.clear (base.getBounds(), juce::Colours::white);
        overlay.clear (overlay.getBounds(), juce::Colours::black);

        auto originalPixel = base.getPixelAt (5, 5);

        // Blend with 0% alpha should leave base unchanged
        juce::Image result = base.createCopy();
        applyBlend (result, overlay, Normal, 0.0f, juce::Point<int> (0, 0));

        auto pixel = result.getPixelAt (5, 5);
        expectEquals (pixel.getRed(), originalPixel.getRed(), "0% alpha should not change base");
        expectEquals (pixel.getGreen(), originalPixel.getGreen(), "0% alpha should not change base");
        expectEquals (pixel.getBlue(), originalPixel.getBlue(), "0% alpha should not change base");

        // Blend with 50% alpha should create mid-tone
        result = base.createCopy();
        applyBlend (result, overlay, Normal, 0.5f, juce::Point<int> (0, 0));

        pixel = result.getPixelAt (5, 5);
        expect (pixel.getRed() < 255 && pixel.getRed() > 0, "50% alpha should create blend");
    }

    void testBlendColor()
    {
        beginTest ("Blend Color");

        juce::Image img (juce::Image::ARGB, 10, 10, true);
        img.clear (img.getBounds(), juce::Colours::white);

        // Blend black with multiply should darken
        applyBlend (img, Multiply, juce::Colours::black);

        auto pixel = img.getPixelAt (5, 5);
        expect (pixel.getBrightness() < 1.0f, "Multiply with black should darken");

        // Reset and blend with add
        img.clear (img.getBounds(), juce::Colour (128, 128, 128));
        applyBlend (img, Add, juce::Colour (50, 50, 50));

        pixel = img.getPixelAt (5, 5);
        expect (pixel.getRed() > 128, "Add should brighten");
    }
};

static BlendingTests blendingTests;

#endif

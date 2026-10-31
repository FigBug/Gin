/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class MipMapTests : public juce::UnitTest
{
public:
    MipMapTests() : juce::UnitTest ("MipMap", "gin_graphics") {}

    void runTest() override
    {
        testLevels();
        testLevelChoice();
        testAverage();
        testEquality();
        testBlend();
    }

private:
    static juce::Image solid (int w, int h, juce::Colour c)
    {
        juce::Image img (juce::Image::ARGB, w, h, true);
        juce::Graphics g (img);
        g.fillAll (c);
        return img;
    }

    void testLevels()
    {
        beginTest ("Levels");

        MipMap empty;
        expect (empty.isNull());
        expectEquals (empty.numMipMaps(), 0);
        expect (empty.getImage().isNull());
        expect (empty.getImageFor (10, 10).isNull());

        MipMap m (solid (256, 128, juce::Colours::red));

        // 256x128, 128x64, 64x32, 32x16, then 16 is not above 16
        expectEquals (m.numMipMaps(), 4);
        expectEquals (m.getWidth(), 256);
        expectEquals (m.getHeight(), 128);
        expect (m.getBounds() == juce::Rectangle<int> (256, 128));
        expectEquals (m.getNumBytesOfData(), (256 * 128 + 128 * 64 + 64 * 32 + 32 * 16) * 4);
    }

    void testLevelChoice()
    {
        beginTest ("Level Choice");

        MipMap m (solid (256, 256, juce::Colours::red));

        expectEquals (m.getImageFor (256, 256).getWidth(), 256);
        expectEquals (m.getImageFor (300, 300).getWidth(), 256, "Larger than the source: the source");
        expectEquals (m.getImageFor (200, 200).getWidth(), 256, "Never a level smaller than asked");
        expectEquals (m.getImageFor (128, 128).getWidth(), 128);
        expectEquals (m.getImageFor (100, 100).getWidth(), 128);
        expectEquals (m.getImageFor (32, 32).getWidth(), 32);
        expectEquals (m.getImageFor (1, 1).getWidth(), 16, "Below the last level: the last");
        expectEquals (m.getImageFor (0, 0).getWidth(), 16, "An empty size: the last");
    }

    void testAverage()
    {
        beginTest ("Average");

        juce::Image img (juce::Image::ARGB, 64, 64, true);
        {
            juce::Image::BitmapData data (img, juce::Image::BitmapData::writeOnly);

            for (int y = 0; y < 64; y++)
                for (int x = 0; x < 64; x++)
                    data.setPixelColour (x, y, (x + y) % 2 == 0 ? juce::Colours::white : juce::Colours::black);
        }

        MipMap m (img);
        auto half = m.getImageFor (32, 32);

        expectEquals (half.getWidth(), 32);

        // A checkerboard averages to the rounded mid grey
        const auto c = half.getPixelAt (5, 7);
        expectEquals (int (c.getRed()), 128);
        expectEquals (int (c.getGreen()), 128);
        expectEquals (int (c.getAlpha()), 255);
    }

    void testEquality()
    {
        beginTest ("Equality");

        MipMap a (solid (64, 64, juce::Colours::red));
        MipMap b = a;
        MipMap c (solid (64, 64, juce::Colours::red));

        expect (a == b, "A copy shares the levels");
        expect (! (a == c), "Another image is another MipMap, even with the same pixels");
    }

    void testBlend()
    {
        beginTest ("Blend");

        MipMap red (solid (32, 32, juce::Colours::red));
        MipMap blue (solid (32, 32, juce::Colours::blue));

        auto blendAt = [&] (float p)
        {
            juce::Image out (juce::Image::ARGB, 32, 32, true);
            {
                juce::Graphics g (out);
                drawBlend (g, red, blue, p, { 0.0f, 0.0f, 32.0f, 32.0f });
            }
            return out.getPixelAt (16, 16);
        };

        expect (blendAt (0.0f) == juce::Colours::red, "0 draws only the first");
        expect (blendAt (1.0f) == juce::Colours::blue, "1 draws only the second");

        const auto mid = blendAt (0.5f);
        expectWithinAbsoluteError (int (mid.getRed()), 128, 2, "The second at half over the first");
        expectWithinAbsoluteError (int (mid.getBlue()), 128, 2);
        expectEquals (int (mid.getAlpha()), 255, "Opaque throughout");
    }
};

static MipMapTests mipMapTests;

#endif

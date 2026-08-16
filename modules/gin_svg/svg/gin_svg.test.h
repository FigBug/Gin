/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
#if GIN_UNIT_TESTS

class SVGTests : public juce::UnitTest
{
public:
    SVGTests() : juce::UnitTest ("SVG", "gin_svg") {}

    void runTest() override
    {
        testRenderToImageSize();
        testRenderToImageContent();
        testRenderToImageBackground();
        testRenderToPath();
        testRenderToPathFitsTargetBounds();
        testInvalidSvgIsSafe();
    }

private:
    const juce::String filledSquare =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100\" height=\"100\" viewBox=\"0 0 100 100\">"
        "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\" fill=\"#ff0000\"/></svg>";

    const juce::String smallCircle =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"100\" height=\"100\" viewBox=\"0 0 100 100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\" fill=\"#00ff00\"/></svg>";

    void testRenderToImageSize()
    {
        beginTest ("Render To Image Size");

        for (auto engine : { SVG::Engine::juce, SVG::Engine::nanosvg })
        {
            auto img = SVG::renderToImage (filledSquare, 64, 48, engine);
            expect (img.isValid(), "Image should be valid");
            expectEquals (img.getWidth(), 64, "Width matches request");
            expectEquals (img.getHeight(), 48, "Height matches request");
        }
    }

    void testRenderToImageContent()
    {
        beginTest ("Render To Image Content");

        // A square that fills the whole view box, rendered into a square image,
        // should paint the centre pixel with the fill colour.
        auto img = SVG::renderToImage (filledSquare, 64, 64, SVG::Engine::juce);

        auto c = img.getPixelAt (32, 32);
        expect (c.getRed()   > 200, "Centre is red");
        expect (c.getGreen() <  60, "Centre has little green");
        expect (c.getBlue()  <  60, "Centre has little blue");
        expect (c.getAlpha() > 200, "Centre is opaque");
    }

    void testRenderToImageBackground()
    {
        beginTest ("Render To Image Background");

        // Small centred shape, so the corners are just background.
        auto img = SVG::renderToImage (smallCircle, 64, 64, juce::Colours::blue, SVG::Engine::juce);

        auto c = img.getPixelAt (1, 1);
        expect (c.getBlue()  > 200, "Corner is blue");
        expect (c.getRed()   <  60, "Corner has little red");
        expect (c.getGreen() <  60, "Corner has little green");
        expect (c.getAlpha() > 200, "Background is opaque");
    }

    void testRenderToPath()
    {
        beginTest ("Render To Path");

        auto path = SVG::renderToPath (filledSquare);
        expect (! path.isEmpty(), "Path should not be empty");

        auto b = path.getBounds();
        expect (b.getWidth()  > 0.0f, "Path has width");
        expect (b.getHeight() > 0.0f, "Path has height");
    }

    void testRenderToPathFitsTargetBounds()
    {
        beginTest ("Render To Path Fits Target Bounds");

        juce::Rectangle<float> target (0.0f, 0.0f, 50.0f, 50.0f);
        auto path = SVG::renderToPath (filledSquare, target);

        expect (! path.isEmpty(), "Path should not be empty");

        auto b = path.getBounds();
        expect (target.expanded (0.5f).contains (b), "Path fits within the target bounds");

        // Square content fitted into a square target should roughly fill it
        expectWithinAbsoluteError (b.getWidth(),  50.0f, 2.0f, "Fitted width");
        expectWithinAbsoluteError (b.getHeight(), 50.0f, 2.0f, "Fitted height");
    }

    void testInvalidSvgIsSafe()
    {
        beginTest ("Invalid SVG Is Handled Safely");

        auto img = SVG::renderToImage ("not svg at all", 32, 32);
        expect (img.getWidth() == 32 && img.getHeight() == 32, "Still returns a sized image");

        auto path = SVG::renderToPath ("not svg at all");
        expect (path.isEmpty(), "Invalid SVG yields an empty path");
    }
};

static SVGTests svgTests;

#endif

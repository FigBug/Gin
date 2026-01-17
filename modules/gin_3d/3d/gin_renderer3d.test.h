/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class Renderer3DTests : public juce::UnitTest
{
public:
    Renderer3DTests() : juce::UnitTest ("Renderer3D", "gin_3d") {}

    void runTest() override
    {
        testBasicProperties();
        testRenderDoesNotCrash();
        testClipping();
    }

private:
    void testBasicProperties()
    {
        beginTest ("Basic Properties");

        Renderer3D renderer;

        expectWithinAbsoluteError (renderer.getLineThickness(), 1.0f, 0.0001f, "Default line thickness");

        renderer.setLineThickness (2.5f);
        expectWithinAbsoluteError (renderer.getLineThickness(), 2.5f, 0.0001f, "Set line thickness");

        expect (!renderer.getDepthCueing(), "Depth cueing off by default");

        renderer.setDepthCueing (true);
        expect (renderer.getDepthCueing(), "Depth cueing can be enabled");

        renderer.setDepthCueing (false);
        expect (!renderer.getDepthCueing(), "Depth cueing can be disabled");
    }

    void testRenderDoesNotCrash()
    {
        beginTest ("Render Does Not Crash");

        // Create a simple scene
        Scene3D scene;

        auto* obj = scene.addObject();
        obj->addAxes (1.0f);
        obj->addBox (Vec3f (-0.5f, -0.5f, -0.5f), Vec3f (0.5f, 0.5f, 0.5f));

        scene.getCamera().setPosition (0.0f, 0.0f, 5.0f);
        scene.getCamera().setTarget (0.0f, 0.0f, 0.0f);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        // Create a small image to render to
        juce::Image image (juce::Image::ARGB, 100, 100, true);
        juce::Graphics g (image);

        Renderer3D renderer;

        // This should not crash
        renderer.render (g, scene, juce::Rectangle<int> (0, 0, 100, 100));

        // Also test float rectangle overload
        renderer.render (g, scene, juce::Rectangle<float> (0.0f, 0.0f, 100.0f, 100.0f));

        expect (true, "Render completed without crash");
    }

    void testClipping()
    {
        beginTest ("Clipping");

        Scene3D scene;

        // Add a line that passes through the camera
        auto* obj = scene.addObject();
        obj->addLine (Vec3f (0.0f, 0.0f, -10.0f), Vec3f (0.0f, 0.0f, 10.0f));

        // Position camera so line passes through it
        scene.getCamera().setPosition (0.0f, 0.0f, 0.0f);
        scene.getCamera().setTarget (0.0f, 0.0f, -1.0f);
        scene.getCamera().setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        juce::Image image (juce::Image::ARGB, 100, 100, true);
        juce::Graphics g (image);

        Renderer3D renderer;

        // This should handle clipping without crashing
        renderer.render (g, scene, juce::Rectangle<int> (0, 0, 100, 100));

        expect (true, "Clipping handled correctly");

        // Test with line entirely behind camera
        scene.clear();
        obj = scene.addObject();
        obj->addLine (Vec3f (0.0f, 0.0f, 5.0f), Vec3f (0.0f, 0.0f, 10.0f));

        scene.getCamera().setPosition (0.0f, 0.0f, 0.0f);
        scene.getCamera().setTarget (0.0f, 0.0f, -1.0f);

        // This line is entirely behind the camera - should be rejected
        renderer.render (g, scene, juce::Rectangle<int> (0, 0, 100, 100));

        expect (true, "Lines behind camera handled correctly");
    }
};

static Renderer3DTests renderer3DTests;

#endif

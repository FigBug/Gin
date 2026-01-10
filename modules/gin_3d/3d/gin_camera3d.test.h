/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class Camera3DTests : public juce::UnitTest
{
public:
    Camera3DTests() : juce::UnitTest ("Camera3D", "gin_3d") {}

    void runTest() override
    {
        testDefaultState();
        testPositionAndTarget();
        testPerspective();
        testMatrices();
        testOrbit();
        testZoom();
        testPan();
    }

private:
    void testDefaultState()
    {
        beginTest ("Default State");

        Camera3D camera;

        Vec3f pos = camera.getPosition();
        expectWithinAbsoluteError (pos.x, 0.0f, 0.0001f, "Default position x");
        expectWithinAbsoluteError (pos.y, 0.0f, 0.0001f, "Default position y");
        expectWithinAbsoluteError (pos.z, 5.0f, 0.0001f, "Default position z");

        Vec3f target = camera.getTarget();
        expectWithinAbsoluteError (target.x, 0.0f, 0.0001f, "Default target x");
        expectWithinAbsoluteError (target.y, 0.0f, 0.0001f, "Default target y");
        expectWithinAbsoluteError (target.z, 0.0f, 0.0001f, "Default target z");

        Vec3f up = camera.getUpVector();
        expectWithinAbsoluteError (up.x, 0.0f, 0.0001f, "Default up x");
        expectWithinAbsoluteError (up.y, 1.0f, 0.0001f, "Default up y");
        expectWithinAbsoluteError (up.z, 0.0f, 0.0001f, "Default up z");
    }

    void testPositionAndTarget()
    {
        beginTest ("Position and Target");

        Camera3D camera;

        camera.setPosition (10.0f, 20.0f, 30.0f);
        Vec3f pos = camera.getPosition();
        expectWithinAbsoluteError (pos.x, 10.0f, 0.0001f, "Set position x");
        expectWithinAbsoluteError (pos.y, 20.0f, 0.0001f, "Set position y");
        expectWithinAbsoluteError (pos.z, 30.0f, 0.0001f, "Set position z");

        camera.setPosition (Vec3f (5.0f, 5.0f, 5.0f));
        pos = camera.getPosition();
        expectWithinAbsoluteError (pos.x, 5.0f, 0.0001f, "Set position with Vec3 x");

        camera.setTarget (1.0f, 2.0f, 3.0f);
        Vec3f target = camera.getTarget();
        expectWithinAbsoluteError (target.x, 1.0f, 0.0001f, "Set target x");
        expectWithinAbsoluteError (target.y, 2.0f, 0.0001f, "Set target y");
        expectWithinAbsoluteError (target.z, 3.0f, 0.0001f, "Set target z");

        camera.setTarget (Vec3f (0.0f, 0.0f, 0.0f));
        target = camera.getTarget();
        expectWithinAbsoluteError (target.x, 0.0f, 0.0001f, "Set target with Vec3");

        camera.setUpVector (Vec3f (0.0f, 0.0f, 1.0f));
        Vec3f up = camera.getUpVector();
        expectWithinAbsoluteError (up.z, 1.0f, 0.0001f, "Set up vector");
    }

    void testPerspective()
    {
        beginTest ("Perspective");

        Camera3D camera;
        camera.setPerspective (60.0f, 16.0f / 9.0f, 0.5f, 500.0f);

        // Getting the projection matrix should not throw
        const Mat4f& proj = camera.getProjectionMatrix();

        // Basic sanity check - projection matrix should have w perspective divide
        expectWithinAbsoluteError (proj.m[2][3], -1.0f, 0.0001f, "Perspective w divide element");
        expectWithinAbsoluteError (proj.m[3][3], 0.0f, 0.0001f, "Perspective corner element");
    }

    void testMatrices()
    {
        beginTest ("Matrices");

        Camera3D camera;
        camera.setPosition (0.0f, 0.0f, 10.0f);
        camera.setTarget (0.0f, 0.0f, 0.0f);
        camera.setPerspective (45.0f, 1.0f, 0.1f, 100.0f);

        const Mat4f& view = camera.getViewMatrix();
        const Mat4f& proj = camera.getProjectionMatrix();
        Mat4f viewProj = camera.getViewProjectionMatrix();

        // View matrix should transform camera position to origin
        Vec3f camPosInView = view.transformPoint (camera.getPosition());
        expectWithinAbsoluteError (camPosInView.x, 0.0f, 0.0001f, "Camera at view origin x");
        expectWithinAbsoluteError (camPosInView.y, 0.0f, 0.0001f, "Camera at view origin y");
        expectWithinAbsoluteError (camPosInView.z, 0.0f, 0.0001f, "Camera at view origin z");

        // View-projection should combine both matrices
        Mat4f expected = proj * view;
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                expectWithinAbsoluteError (viewProj.m[i][j], expected.m[i][j], 0.0001f,
                    "ViewProj[" + juce::String(i) + "][" + juce::String(j) + "]");
            }
        }
    }

    void testOrbit()
    {
        beginTest ("Orbit");

        Camera3D camera;
        camera.setPosition (0.0f, 0.0f, 5.0f);
        camera.setTarget (0.0f, 0.0f, 0.0f);

        float initialDistance = (camera.getPosition() - camera.getTarget()).length();

        // Orbit horizontally
        camera.orbit (juce::MathConstants<float>::pi / 4.0f, 0.0f);

        Vec3f newPos = camera.getPosition();
        float newDistance = (newPos - camera.getTarget()).length();

        // Distance from target should remain the same
        expectWithinAbsoluteError (newDistance, initialDistance, 0.0001f, "Distance preserved after orbit");

        // Position should have changed
        expect (std::abs (newPos.x) > 0.1f, "X position changed after horizontal orbit");

        // Orbit vertically
        Camera3D camera2;
        camera2.setPosition (0.0f, 0.0f, 5.0f);
        camera2.setTarget (0.0f, 0.0f, 0.0f);

        camera2.orbit (0.0f, juce::MathConstants<float>::pi / 6.0f);

        Vec3f newPos2 = camera2.getPosition();
        expect (newPos2.y > 0.1f, "Y position changed after vertical orbit");
    }

    void testZoom()
    {
        beginTest ("Zoom");

        Camera3D camera;
        camera.setPosition (0.0f, 0.0f, 10.0f);
        camera.setTarget (0.0f, 0.0f, 0.0f);

        float initialDistance = (camera.getPosition() - camera.getTarget()).length();
        expectWithinAbsoluteError (initialDistance, 10.0f, 0.0001f, "Initial distance");

        // Zoom in
        camera.zoom (3.0f);

        float newDistance = (camera.getPosition() - camera.getTarget()).length();
        expectWithinAbsoluteError (newDistance, 7.0f, 0.0001f, "Distance after zoom in");

        // Zoom out
        camera.zoom (-5.0f);

        newDistance = (camera.getPosition() - camera.getTarget()).length();
        expectWithinAbsoluteError (newDistance, 12.0f, 0.0001f, "Distance after zoom out");

        // Extreme zoom in - should clamp to minimum
        camera.zoom (100.0f);
        newDistance = (camera.getPosition() - camera.getTarget()).length();
        expect (newDistance >= 0.1f, "Zoom should clamp to minimum distance");
    }

    void testPan()
    {
        beginTest ("Pan");

        Camera3D camera;
        camera.setPosition (0.0f, 0.0f, 5.0f);
        camera.setTarget (0.0f, 0.0f, 0.0f);

        Vec3f initialPos = camera.getPosition();
        Vec3f initialTarget = camera.getTarget();

        camera.pan (2.0f, 0.0f);

        Vec3f newPos = camera.getPosition();
        Vec3f newTarget = camera.getTarget();

        // Both position and target should have moved by the same amount
        Vec3f posOffset = newPos - initialPos;
        Vec3f targetOffset = newTarget - initialTarget;

        expectWithinAbsoluteError (posOffset.x, targetOffset.x, 0.0001f, "Pan moves position and target equally x");
        expectWithinAbsoluteError (posOffset.y, targetOffset.y, 0.0001f, "Pan moves position and target equally y");
        expectWithinAbsoluteError (posOffset.z, targetOffset.z, 0.0001f, "Pan moves position and target equally z");

        // Distance between camera and target should be preserved
        float initialDist = (initialPos - initialTarget).length();
        float newDist = (newPos - newTarget).length();
        expectWithinAbsoluteError (newDist, initialDist, 0.0001f, "Pan preserves camera-target distance");
    }
};

static Camera3DTests camera3DTests;

#endif

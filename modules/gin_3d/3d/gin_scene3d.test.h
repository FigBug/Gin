/*==============================================================================

 Copyright 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class Scene3DTests : public juce::UnitTest
{
public:
    Scene3DTests() : juce::UnitTest ("Scene3D", "gin_3d") {}

    void runTest() override
    {
        testLineSegment();
        testObject3D();
        testObject3DShapes();
        testObject3DTransform();
        testScene3D();
    }

private:
    void testLineSegment()
    {
        beginTest ("LineSegment3D");

        LineSegment3D defaultLine;
        expectWithinAbsoluteError (defaultLine.start.x, 0.0f, 0.0001f, "Default start x");
        expectWithinAbsoluteError (defaultLine.end.x, 0.0f, 0.0001f, "Default end x");

        LineSegment3D lineVec (Vec3f (1.0f, 2.0f, 3.0f), Vec3f (4.0f, 5.0f, 6.0f), juce::Colours::red);
        expectWithinAbsoluteError (lineVec.start.x, 1.0f, 0.0001f, "Vec3 constructor start x");
        expectWithinAbsoluteError (lineVec.end.z, 6.0f, 0.0001f, "Vec3 constructor end z");
        expect (lineVec.colour == juce::Colours::red, "Line colour");

        LineSegment3D lineCoords (0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, juce::Colours::blue);
        expectWithinAbsoluteError (lineCoords.start.x, 0.0f, 0.0001f, "Coords constructor start");
        expectWithinAbsoluteError (lineCoords.end.x, 1.0f, 0.0001f, "Coords constructor end");
        expect (lineCoords.colour == juce::Colours::blue, "Coords line colour");
    }

    void testObject3D()
    {
        beginTest ("Object3D Basic");

        Object3D obj;

        expectEquals ((int) obj.getNumLines(), 0, "Empty object has no lines");
        expect (obj.isVisible(), "Object visible by default");

        obj.addLine (Vec3f (0.0f, 0.0f, 0.0f), Vec3f (1.0f, 0.0f, 0.0f), juce::Colours::white);
        expectEquals ((int) obj.getNumLines(), 1, "One line after addLine");

        LineSegment3D seg (Vec3f (0.0f, 0.0f, 0.0f), Vec3f (0.0f, 1.0f, 0.0f));
        obj.addLine (seg);
        expectEquals ((int) obj.getNumLines(), 2, "Two lines after adding segment");

        const auto& lines = obj.getLines();
        expectEquals ((int) lines.size(), 2, "getLines returns correct size");

        obj.clear();
        expectEquals ((int) obj.getNumLines(), 0, "Clear removes all lines");

        obj.setVisible (false);
        expect (!obj.isVisible(), "Object can be made invisible");

        obj.setVisible (true);
        expect (obj.isVisible(), "Object can be made visible again");
    }

    void testObject3DShapes()
    {
        beginTest ("Object3D Shapes");

        Object3D obj;

        // Triangle
        obj.addTriangle (Vec3f (0.0f, 0.0f, 0.0f), Vec3f (1.0f, 0.0f, 0.0f), Vec3f (0.5f, 1.0f, 0.0f));
        expectEquals ((int) obj.getNumLines(), 3, "Triangle has 3 lines");

        obj.clear();

        // Box
        obj.addBox (Vec3f (-1.0f, -1.0f, -1.0f), Vec3f (1.0f, 1.0f, 1.0f));
        expectEquals ((int) obj.getNumLines(), 12, "Box has 12 edges");

        obj.clear();

        // Grid
        obj.addGrid (10.0f, 5);
        expectEquals ((int) obj.getNumLines(), 12, "5x5 grid has 12 lines (6 + 6)");

        obj.clear();

        // Axes
        obj.addAxes (5.0f);
        expectEquals ((int) obj.getNumLines(), 3, "Axes have 3 lines");

        // Verify axis colours
        const auto& axisLines = obj.getLines();
        expect (axisLines[0].colour == juce::Colours::red, "X axis is red");
        expect (axisLines[1].colour == juce::Colours::green, "Y axis is green");
        expect (axisLines[2].colour == juce::Colours::blue, "Z axis is blue");
    }

    void testObject3DTransform()
    {
        beginTest ("Object3D Transform");

        Object3D obj;

        // Default transform is identity
        const Mat4f& identity = obj.getTransform();
        expectWithinAbsoluteError (identity.m[0][0], 1.0f, 0.0001f, "Default transform is identity");

        // Set position
        obj.setPosition (Vec3f (5.0f, 10.0f, 15.0f));
        Vec3f pos = obj.getPosition();
        expectWithinAbsoluteError (pos.x, 5.0f, 0.0001f, "Position x");
        expectWithinAbsoluteError (pos.y, 10.0f, 0.0001f, "Position y");
        expectWithinAbsoluteError (pos.z, 15.0f, 0.0001f, "Position z");

        // Set custom transform
        Mat4f customTransform = Mat4f::scale (2.0f);
        obj.setTransform (customTransform);

        const Mat4f& retrieved = obj.getTransform();
        expectWithinAbsoluteError (retrieved.m[0][0], 2.0f, 0.0001f, "Custom transform preserved");
    }

    void testScene3D()
    {
        beginTest ("Scene3D");

        Scene3D scene;

        expectEquals ((int) scene.getNumObjects(), 0, "Empty scene has no objects");

        // Add objects
        Object3D* obj1 = scene.addObject();
        expect (obj1 != nullptr, "addObject returns valid pointer");
        expectEquals ((int) scene.getNumObjects(), 1, "One object after add");

        Object3D* obj2 = scene.addObject();
        expectEquals ((int) scene.getNumObjects(), 2, "Two objects after second add");

        // Get objects
        expect (scene.getObject (0) == obj1, "getObject(0) returns first object");
        expect (scene.getObject (1) == obj2, "getObject(1) returns second object");
        expect (scene.getObject (100) == nullptr, "Out of bounds returns nullptr");

        // Modify object through pointer
        obj1->addLine (Vec3f (0.0f, 0.0f, 0.0f), Vec3f (1.0f, 0.0f, 0.0f));
        expectEquals ((int) scene.getObject (0)->getNumLines(), 1, "Object modification persists");

        // Remove object
        scene.removeObject (0);
        expectEquals ((int) scene.getNumObjects(), 1, "One object after remove");
        expect (scene.getObject (0) == obj2, "Second object moved to index 0");

        // Camera access
        Camera3D& camera = scene.getCamera();
        camera.setPosition (10.0f, 10.0f, 10.0f);

        const Scene3D& constScene = scene;
        const Camera3D& constCamera = constScene.getCamera();
        expectWithinAbsoluteError (constCamera.getPosition().x, 10.0f, 0.0001f, "Const camera access");

        // Get all objects (const)
        const auto& objects = constScene.getObjects();
        expectEquals ((int) objects.size(), 1, "getObjects returns correct count");

        // Clear scene
        scene.clear();
        expectEquals ((int) scene.getNumObjects(), 0, "Scene cleared");
    }
};

static Scene3DTests scene3DTests;

#endif

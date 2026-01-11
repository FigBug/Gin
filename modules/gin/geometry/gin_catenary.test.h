/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class CatenaryTests : public juce::UnitTest
{
public:
    CatenaryTests() : juce::UnitTest ("Catenary", "gin") {}

    void runTest() override
    {
        testBasicCatenary();
        testSymmetricCatenary();
        testVertexPosition();
        testEndpoints();
        testDifferentSlackAmounts();
        testScreenCoordinates();
        testScreenCoordsSymmetric();
        testScreenCoordsVertex();
        testScreenCoordsEndpoints();
        testScreenCoordsMathCoordsEquivalence();
    }

private:
    void testBasicCatenary()
    {
        beginTest ("Basic Catenary Creation");

        // Create a catenary between two points
        Catenary cat (0.0f, 0.0f, 10.0f, 0.0f, 2.0f);

        // The curve should sag below the endpoints (lower Y value in standard coords)
        float midY = cat.calcY (5.0f);
        expect (midY < 0.0f, "Catenary should sag below endpoints (lower Y value)");

        // Endpoints should be close to specified values
        float startY = cat.calcY (0.0f);
        float endY = cat.calcY (10.0f);
        expectWithinAbsoluteError (startY, 0.0f, 0.01f, "Start Y should be near 0");
        expectWithinAbsoluteError (endY, 0.0f, 0.01f, "End Y should be near 0");
    }

    void testSymmetricCatenary()
    {
        beginTest ("Symmetric Catenary");

        // Horizontal catenary should be symmetric
        Catenary cat (0.0f, 0.0f, 10.0f, 0.0f, 3.0f);

        float y1 = cat.calcY (2.5f);
        float y2 = cat.calcY (7.5f);
        expectWithinAbsoluteError (y1, y2, 0.001f, "Symmetric points should have same Y");

        float y3 = cat.calcY (1.0f);
        float y4 = cat.calcY (9.0f);
        expectWithinAbsoluteError (y3, y4, 0.001f, "Symmetric points should have same Y");
    }

    void testVertexPosition()
    {
        beginTest ("Vertex Position");

        // Horizontal catenary - vertex should be at midpoint X
        Catenary cat (0.0f, 0.0f, 10.0f, 0.0f, 2.0f);
        auto [vx, vy] = cat.getVertex();

        expectWithinAbsoluteError (vx, 5.0f, 0.01f, "Vertex X should be at midpoint");
        expect (vy < 0.0f, "Vertex Y should be below endpoints");

        // Vertex should be the lowest point (lowest Y in standard coords)
        float leftY = cat.calcY (4.0f);
        float rightY = cat.calcY (6.0f);
        expect (vy <= leftY && vy <= rightY, "Vertex should be lowest point");
    }

    void testEndpoints()
    {
        beginTest ("Different Endpoint Heights");

        // Test with endpoints at different heights
        Catenary cat (0.0f, 0.0f, 10.0f, 5.0f, 2.0f);

        float startY = cat.calcY (0.0f);
        float endY = cat.calcY (10.0f);

        expectWithinAbsoluteError (startY, 0.0f, 0.01f, "Start Y should match");
        expectWithinAbsoluteError (endY, 5.0f, 0.01f, "End Y should match");
    }

    void testDifferentSlackAmounts()
    {
        beginTest ("Different Slack Amounts");

        // More slack should create more sag (lower Y value)
        Catenary cat1 (0.0f, 0.0f, 10.0f, 0.0f, 1.0f);
        Catenary cat2 (0.0f, 0.0f, 10.0f, 0.0f, 5.0f);

        float sag1 = cat1.calcY (5.0f);
        float sag2 = cat2.calcY (5.0f);

        expect (sag2 < sag1, "More slack should create more sag (lower Y)");
    }

    void testScreenCoordinates()
    {
        beginTest ("Screen Coordinates - Basic");

        // In screen coords, Y increases downward (0,0 at top-left)
        // So the cable should sag to HIGHER Y values (visually downward)
        Catenary cat (0.0f, 100.0f, 200.0f, 100.0f, 20.0f, 1, true);

        float midY = cat.calcY (100.0f);
        expect (midY > 100.0f, "Screen coords: catenary should sag to higher Y (downward on screen)");

        // Endpoints should still be correct
        float startY = cat.calcY (0.0f);
        float endY = cat.calcY (200.0f);
        expectWithinAbsoluteError (startY, 100.0f, 0.1f, "Screen coords: Start Y should match");
        expectWithinAbsoluteError (endY, 100.0f, 0.1f, "Screen coords: End Y should match");
    }

    void testScreenCoordsSymmetric()
    {
        beginTest ("Screen Coordinates - Symmetric");

        // Horizontal catenary in screen coords should still be symmetric
        Catenary cat (50.0f, 200.0f, 250.0f, 200.0f, 30.0f, 1, true);

        float y1 = cat.calcY (100.0f);
        float y2 = cat.calcY (200.0f);
        expectWithinAbsoluteError (y1, y2, 0.01f, "Screen coords: Symmetric points should have same Y");

        float y3 = cat.calcY (75.0f);
        float y4 = cat.calcY (225.0f);
        expectWithinAbsoluteError (y3, y4, 0.01f, "Screen coords: Symmetric points should have same Y");
    }

    void testScreenCoordsVertex()
    {
        beginTest ("Screen Coordinates - Vertex");

        // Horizontal catenary in screen coords - vertex should be at midpoint X
        // and be the highest Y value (lowest point visually)
        Catenary cat (0.0f, 100.0f, 100.0f, 100.0f, 15.0f, 1, true);
        auto [vx, vy] = cat.getVertex();

        expectWithinAbsoluteError (vx, 50.0f, 0.1f, "Screen coords: Vertex X should be at midpoint");
        expect (vy > 100.0f, "Screen coords: Vertex Y should be greater than endpoints (hanging down)");

        // Vertex should be the highest Y point in screen coords
        float leftY = cat.calcY (40.0f);
        float rightY = cat.calcY (60.0f);
        expect (vy >= leftY && vy >= rightY, "Screen coords: Vertex should be highest Y point");
    }

    void testScreenCoordsEndpoints()
    {
        beginTest ("Screen Coordinates - Different Heights");

        // Test with endpoints at different heights in screen coordinates
        // Point at (0, 50) and (100, 150) - second point is lower on screen
        Catenary cat (0.0f, 50.0f, 100.0f, 150.0f, 10.0f, 1, true);

        float startY = cat.calcY (0.0f);
        float endY = cat.calcY (100.0f);

        expectWithinAbsoluteError (startY, 50.0f, 0.1f, "Screen coords: Start Y should match");
        expectWithinAbsoluteError (endY, 150.0f, 0.1f, "Screen coords: End Y should match");

        // The sag should still go downward (higher Y values) from the line between endpoints
        float midY = cat.calcY (50.0f);
        float lineY = 100.0f;  // midpoint on straight line would be Y=100
        expect (midY > lineY, "Screen coords: Catenary should sag below the line (higher Y)");
    }

    void testScreenCoordsMathCoordsEquivalence()
    {
        beginTest ("Screen vs Math Coordinates Equivalence");

        // Create equivalent catenaries in both coordinate systems
        // Math coords: points at (0, 0) and (10, 0)
        // Screen coords: points at (0, 100) and (10, 100) - same relative positions
        Catenary mathCat (0.0f, 0.0f, 10.0f, 0.0f, 2.0f, 1, false);
        Catenary screenCat (0.0f, 100.0f, 10.0f, 100.0f, 2.0f, 1, true);

        // The sag amount should be the same magnitude, just in opposite Y directions
        float mathMidY = mathCat.calcY (5.0f);
        float screenMidY = screenCat.calcY (5.0f);

        // Math: sags down (negative Y from 0), Screen: sags down (positive Y from 100)
        float mathSag = 0.0f - mathMidY;        // positive value = amount of sag
        float screenSag = screenMidY - 100.0f;  // positive value = amount of sag

        expectWithinAbsoluteError (mathSag, screenSag, 0.001f,
            "Sag magnitude should be identical in both coordinate systems");

        // Test vertices have same relative offset
        auto [mathVx, mathVy] = mathCat.getVertex();
        auto [screenVx, screenVy] = screenCat.getVertex();

        expectWithinAbsoluteError (mathVx, screenVx, 0.001f,
            "Vertex X should be identical in both systems");

        float mathVertexOffset = 0.0f - mathVy;
        float screenVertexOffset = screenVy - 100.0f;

        expectWithinAbsoluteError (mathVertexOffset, screenVertexOffset, 0.001f,
            "Vertex Y offset should be identical in both systems");
    }
};

static CatenaryTests catenaryTests;

#endif

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

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
};

static CatenaryTests catenaryTests;

#endif

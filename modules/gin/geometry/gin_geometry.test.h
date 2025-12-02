/*==============================================================================

 Copyright 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if GIN_UNIT_TESTS

class GinGeometryTests : public juce::UnitTest
{
public:
    GinGeometryTests() : juce::UnitTest ("Gin Geometry", "gin") {}

    void runTest() override
    {
        testEllipse();
        testPolygon();
        testLineSolving();
        testSquareFunction();
    }

private:
    void testEllipse()
    {
        beginTest ("Ellipse");

        // Create ellipse centered at (5, 5) with radii 10 and 5
        Ellipse<float> ellipse (5.0f, 5.0f, 10.0f, 5.0f);

        // Test public members
        expectWithinAbsoluteError (ellipse.x, 5.0f, 0.0001f, "Center X should be 5");
        expectWithinAbsoluteError (ellipse.y, 5.0f, 0.0001f, "Center Y should be 5");
        expectWithinAbsoluteError (ellipse.a, 10.0f, 0.0001f, "Semi-major axis should be 10");
        expectWithinAbsoluteError (ellipse.b, 5.0f, 0.0001f, "Semi-minor axis should be 5");

        // Test pointAtAngle
        auto rightPoint = ellipse.pointAtAngle (0.0f);
        expectWithinAbsoluteError (rightPoint.x, 15.0f, 0.1f, "Right point X should be center + a");
        expectWithinAbsoluteError (rightPoint.y, 5.0f, 0.1f, "Right point Y should be center Y");

        auto topPoint = ellipse.pointAtAngle (juce::MathConstants<float>::halfPi);
        expectWithinAbsoluteError (topPoint.x, 5.0f, 0.1f, "Top point X should be center X");
        expectWithinAbsoluteError (topPoint.y, 10.0f, 0.1f, "Top point Y should be center + b");

        // Test isPointInside
        expect (ellipse.isPointInside (juce::Point<float> (5.0f, 5.0f)), "Center should be inside");
        expect (!ellipse.isPointOutside (juce::Point<float> (5.0f, 5.0f)), "Center should not be outside");

        // Test isPointOutside
        expect (ellipse.isPointOutside (juce::Point<float> (100.0f, 100.0f)), "Far point should be outside");
        expect (!ellipse.isPointInside (juce::Point<float> (100.0f, 100.0f)), "Far point should not be inside");

        // Create ellipse centered at origin
        Ellipse<float> ellipse2 (8.0f, 6.0f);
        expectWithinAbsoluteError (ellipse2.x, 0.0f, 0.0001f, "Default center X should be 0");
        expectWithinAbsoluteError (ellipse2.y, 0.0f, 0.0001f, "Default center Y should be 0");
        expectWithinAbsoluteError (ellipse2.a, 8.0f, 0.0001f, "Semi-major axis should be 8");
        expectWithinAbsoluteError (ellipse2.b, 6.0f, 0.0001f, "Semi-minor axis should be 6");
    }

    void testPolygon()
    {
        beginTest ("Polygon");

        Polygon<float> polygon;

        // Add triangle vertices
        polygon.points.add (juce::Point<float> (0.0f, 0.0f));
        polygon.points.add (juce::Point<float> (10.0f, 0.0f));
        polygon.points.add (juce::Point<float> (5.0f, 10.0f));

        expect (polygon.points.size() == 3, "Triangle should have 3 vertices");

        // Verify points are stored correctly
        expectWithinAbsoluteError (polygon.points[0].x, 0.0f, 0.0001f, "First vertex X");
        expectWithinAbsoluteError (polygon.points[0].y, 0.0f, 0.0001f, "First vertex Y");
        expectWithinAbsoluteError (polygon.points[1].x, 10.0f, 0.0001f, "Second vertex X");
        expectWithinAbsoluteError (polygon.points[1].y, 0.0f, 0.0001f, "Second vertex Y");
        expectWithinAbsoluteError (polygon.points[2].x, 5.0f, 0.0001f, "Third vertex X");
        expectWithinAbsoluteError (polygon.points[2].y, 10.0f, 0.0001f, "Third vertex Y");
    }

    void testLineSolving()
    {
        beginTest ("Line Solving");

        float m, b;

        // Test solving line from two points: y = x (slope 1, intercept 0)
        bool success = solveLine (0.0f, 0.0f, 10.0f, 10.0f, m, b);
        expect (success, "Should successfully solve non-vertical line");
        expectWithinAbsoluteError (m, 1.0f, 0.0001f, "Slope should be 1");
        expectWithinAbsoluteError (b, 0.0f, 0.0001f, "Intercept should be 0");

        // Test horizontal line: y = 5
        success = solveLine (0.0f, 5.0f, 10.0f, 5.0f, m, b);
        expect (success, "Should successfully solve horizontal line");
        expectWithinAbsoluteError (m, 0.0f, 0.0001f, "Horizontal line slope should be 0");
        expectWithinAbsoluteError (b, 5.0f, 0.0001f, "Horizontal line intercept should be 5");

        // Test line with slope 2, intercept 3: y = 2x + 3
        success = solveLine (0.0f, 3.0f, 1.0f, 5.0f, m, b);
        expect (success, "Should successfully solve sloped line");
        expectWithinAbsoluteError (m, 2.0f, 0.0001f, "Slope should be 2");
        expectWithinAbsoluteError (b, 3.0f, 0.0001f, "Intercept should be 3");

        // Test vertical line (should fail)
        success = solveLine (5.0f, 0.0f, 5.0f, 10.0f, m, b);
        expect (!success, "Should fail to solve vertical line");
    }

    void testSquareFunction()
    {
        beginTest ("Square Function");

        expectEquals (square (5), 25, "square(5) should be 25");
        expectEquals (square (-3), 9, "square(-3) should be 9");
        expectWithinAbsoluteError (square (2.5f), 6.25f, 0.0001f, "square(2.5) should be 6.25");
        expectEquals (square (0), 0, "square(0) should be 0");
    }
};

static GinGeometryTests ginGeometryTests;

#endif

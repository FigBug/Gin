#include <JuceHeader.h>

//==============================================================================
class GinGeometryTests : public juce::UnitTest
{
public:
    GinGeometryTests() : juce::UnitTest ("Gin Geometry", "gin") {}

    void runTest() override
    {
        testEllipse();
        testPolygon();
        testLineSolving();
    }

private:
    void testEllipse()
    {
        beginTest ("Ellipse");

        gin::Ellipse ellipse (juce::Point<float> (0, 0), 10.0f, 5.0f);

        // Test center
        auto center = ellipse.getCentre();
        expectWithinAbsoluteError (center.x, 0.0f, 0.0001f, "Center X should be 0");
        expectWithinAbsoluteError (center.y, 0.0f, 0.0001f, "Center Y should be 0");

        // Test radii
        expectWithinAbsoluteError (ellipse.getRadiusX(), 10.0f, 0.0001f, "Radius X should be 10");
        expectWithinAbsoluteError (ellipse.getRadiusY(), 5.0f, 0.0001f, "Radius Y should be 5");

        // Test points on ellipse
        auto topPoint = ellipse.getPointAt (juce::MathConstants<float>::halfPi);
        expectWithinAbsoluteError (topPoint.x, 0.0f, 0.0001f, "Top point X should be 0");
        expectWithinAbsoluteError (topPoint.y, 5.0f, 0.0001f, "Top point Y should be radius Y");

        auto rightPoint = ellipse.getPointAt (0.0f);
        expectWithinAbsoluteError (rightPoint.x, 10.0f, 0.0001f, "Right point X should be radius X");
        expectWithinAbsoluteError (rightPoint.y, 0.0f, 0.0001f, "Right point Y should be 0");

        // Test path generation
        juce::Path path = ellipse.toPath();
        expect (!path.isEmpty(), "Path should not be empty");
    }

    void testPolygon()
    {
        beginTest ("Polygon");

        gin::Polygon polygon;

        // Add triangle vertices
        polygon.addVertex (juce::Point<float> (0, 0));
        polygon.addVertex (juce::Point<float> (10, 0));
        polygon.addVertex (juce::Point<float> (5, 10));

        expect (polygon.getNumVertices() == 3, "Triangle should have 3 vertices");

        // Test bounds
        auto bounds = polygon.getBounds();
        expectWithinAbsoluteError (bounds.getX(), 0.0f, 0.0001f, "Bounds X should be 0");
        expectWithinAbsoluteError (bounds.getY(), 0.0f, 0.0001f, "Bounds Y should be 0");
        expectWithinAbsoluteError (bounds.getWidth(), 10.0f, 0.0001f, "Bounds width should be 10");
        expectWithinAbsoluteError (bounds.getHeight(), 10.0f, 0.0001f, "Bounds height should be 10");

        // Test centroid of triangle
        auto centroid = polygon.getCentroid();
        expectWithinAbsoluteError (centroid.x, 5.0f, 0.1f, "Centroid X should be approximately 5");
        expectWithinAbsoluteError (centroid.y, 3.333f, 0.1f, "Centroid Y should be approximately 3.33");

        // Test point in polygon
        expect (polygon.contains (juce::Point<float> (5, 5)), "Center point should be inside triangle");
        expect (!polygon.contains (juce::Point<float> (20, 20)), "Far point should be outside triangle");
        expect (!polygon.contains (juce::Point<float> (-5, -5)), "Negative point should be outside triangle");
    }

    void testLineSolving()
    {
        beginTest ("Line Solving");

        // Test solving line from two points
        auto line1 = gin::lineFrom2Points (juce::Point<float> (0, 0), juce::Point<float> (10, 10));

        // Line should be y = x, so slope = 1, intercept = 0
        expectWithinAbsoluteError (line1.getA(), 1.0f, 0.0001f, "Slope should be 1");
        expectWithinAbsoluteError (line1.getB(), 0.0f, 0.0001f, "Intercept should be 0");

        // Test horizontal line
        auto hLine = gin::lineFrom2Points (juce::Point<float> (0, 5), juce::Point<float> (10, 5));
        expectWithinAbsoluteError (hLine.getA(), 0.0f, 0.0001f, "Horizontal line slope should be 0");
        expectWithinAbsoluteError (hLine.getB(), 5.0f, 0.0001f, "Horizontal line intercept should be 5");

        // Test line with slope 2, intercept 3
        auto line2 = gin::lineFrom2Points (juce::Point<float> (0, 3), juce::Point<float> (1, 5));
        expectWithinAbsoluteError (line2.getA(), 2.0f, 0.0001f, "Slope should be 2");
        expectWithinAbsoluteError (line2.getB(), 3.0f, 0.0001f, "Intercept should be 3");

        // Test intersection of perpendicular lines
        auto vLine = gin::lineFrom2Points (juce::Point<float> (5, 0), juce::Point<float> (5, 10));
        auto hLine2 = gin::lineFrom2Points (juce::Point<float> (0, 7), juce::Point<float> (10, 7));

        auto intersection = gin::lineIntersection (vLine, hLine2);
        expectWithinAbsoluteError (intersection.x, 5.0f, 0.1f, "Intersection X should be 5");
        expectWithinAbsoluteError (intersection.y, 7.0f, 0.1f, "Intersection Y should be 7");
    }
};

static GinGeometryTests ginGeometryTests;

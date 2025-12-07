/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Represents a point in 3D space. */
struct Point3D
{
    Vec3f position;
    float size { 1.0f };
    juce::Colour colour { juce::Colours::white };

    Point3D() = default;
    Point3D (const Vec3f& pos, float sz = 1.0f, juce::Colour c = juce::Colours::white)
        : position (pos), size (sz), colour (c) {}
    Point3D (float x, float y, float z, float sz = 1.0f, juce::Colour c = juce::Colours::white)
        : position (x, y, z), size (sz), colour (c) {}
};

/** Represents a line segment in 3D space. */
struct LineSegment3D
{
    Vec3f start;
    Vec3f end;
    juce::Colour colour { juce::Colours::white };

    LineSegment3D() = default;
    LineSegment3D (const Vec3f& s, const Vec3f& e, juce::Colour c = juce::Colours::white)
        : start (s), end (e), colour (c) {}
    LineSegment3D (float x1, float y1, float z1, float x2, float y2, float z2, juce::Colour c = juce::Colours::white)
        : start (x1, y1, z1), end (x2, y2, z2), colour (c) {}
};

/** A 3D object that contains a collection of points and line segments with a transform. */
class Object3D
{
public:
    Object3D() = default;

    /** Clears all points and line segments. */
    void clear() { points.clear(); lines.clear(); }

    /** Adds a point. */
    void addPoint (const Point3D& point) { points.push_back (point); }
    void addPoint (const Vec3f& position, float size = 1.0f, juce::Colour colour = juce::Colours::white)
    {
        points.emplace_back (position, size, colour);
    }

    /** Adds a line segment. */
    void addLine (const LineSegment3D& line) { lines.push_back (line); }
    void addLine (const Vec3f& start, const Vec3f& end, juce::Colour colour = juce::Colours::white)
    {
        lines.emplace_back (start, end, colour);
    }

    /** Adds multiple lines forming a triangle (wireframe). */
    void addTriangle (const Vec3f& a, const Vec3f& b, const Vec3f& c, juce::Colour colour = juce::Colours::white)
    {
        addLine (a, b, colour);
        addLine (b, c, colour);
        addLine (c, a, colour);
    }

    /** Adds multiple lines forming a box (wireframe). */
    void addBox (const Vec3f& min, const Vec3f& max, juce::Colour colour = juce::Colours::white)
    {
        Vec3f corners[8] = {
            { min.x, min.y, min.z },
            { max.x, min.y, min.z },
            { max.x, max.y, min.z },
            { min.x, max.y, min.z },
            { min.x, min.y, max.z },
            { max.x, min.y, max.z },
            { max.x, max.y, max.z },
            { min.x, max.y, max.z }
        };

        // Bottom face
        addLine (corners[0], corners[1], colour);
        addLine (corners[1], corners[2], colour);
        addLine (corners[2], corners[3], colour);
        addLine (corners[3], corners[0], colour);

        // Top face
        addLine (corners[4], corners[5], colour);
        addLine (corners[5], corners[6], colour);
        addLine (corners[6], corners[7], colour);
        addLine (corners[7], corners[4], colour);

        // Vertical edges
        addLine (corners[0], corners[4], colour);
        addLine (corners[1], corners[5], colour);
        addLine (corners[2], corners[6], colour);
        addLine (corners[3], corners[7], colour);
    }

    /** Adds lines forming a grid on the XZ plane. */
    void addGrid (float size, int divisions, juce::Colour colour = juce::Colours::grey)
    {
        float halfSize = size / 2.0f;
        float step = size / float (divisions);

        for (int i = 0; i <= divisions; ++i)
        {
            float pos = -halfSize + i * step;
            addLine ({ pos, 0, -halfSize }, { pos, 0, halfSize }, colour);
            addLine ({ -halfSize, 0, pos }, { halfSize, 0, pos }, colour);
        }
    }

    /** Adds coordinate axis lines. */
    void addAxes (float length, float thickness = 1.0f)
    {
        juce::ignoreUnused (thickness);
        addLine ({ 0, 0, 0 }, { length, 0, 0 }, juce::Colours::red);
        addLine ({ 0, 0, 0 }, { 0, length, 0 }, juce::Colours::green);
        addLine ({ 0, 0, 0 }, { 0, 0, length }, juce::Colours::blue);
    }

    /** Sets the object's transform matrix. */
    void setTransform (const Mat4f& t) { transform = t; }

    /** Gets the object's transform matrix. */
    const Mat4f& getTransform() const { return transform; }

    /** Sets the object's position. */
    void setPosition (const Vec3f& pos)
    {
        transform.m[3][0] = pos.x;
        transform.m[3][1] = pos.y;
        transform.m[3][2] = pos.z;
    }

    /** Gets the object's position. */
    Vec3f getPosition() const
    {
        return { transform.m[3][0], transform.m[3][1], transform.m[3][2] };
    }

    /** Gets all points. */
    const std::vector<Point3D>& getPoints() const { return points; }

    /** Gets the number of points. */
    size_t getNumPoints() const { return points.size(); }

    /** Gets all line segments. */
    const std::vector<LineSegment3D>& getLines() const { return lines; }

    /** Gets the number of line segments. */
    size_t getNumLines() const { return lines.size(); }

    /** Sets the default colour for new lines. */
    void setColour (juce::Colour c) { defaultColour = c; }

    /** Visibility. */
    void setVisible (bool v) { visible = v; }
    bool isVisible() const { return visible; }

private:
    std::vector<Point3D> points;
    std::vector<LineSegment3D> lines;
    Mat4f transform;
    juce::Colour defaultColour { juce::Colours::white };
    bool visible = true;
};

/** A scene containing multiple 3D objects and a camera. */
class Scene3D
{
public:
    Scene3D() = default;

    /** Clears all objects from the scene. */
    void clear() { objects.clear(); }

    /** Adds an object to the scene and returns a pointer to it. */
    Object3D* addObject()
    {
        objects.push_back (std::make_unique<Object3D>());
        return objects.back().get();
    }

    /** Gets an object by index. */
    Object3D* getObject (size_t index)
    {
        return index < objects.size() ? objects[index].get() : nullptr;
    }

    /** Gets the number of objects. */
    size_t getNumObjects() const { return objects.size(); }

    /** Removes an object by index. */
    void removeObject (size_t index)
    {
        if (index < objects.size())
            objects.erase (objects.begin() + static_cast<std::ptrdiff_t> (index));
    }

    /** Gets the camera. */
    Camera3D& getCamera() { return camera; }
    const Camera3D& getCamera() const { return camera; }

    /** Gets all objects (for rendering). */
    const std::vector<std::unique_ptr<Object3D>>& getObjects() const { return objects; }

private:
    std::vector<std::unique_ptr<Object3D>> objects;
    Camera3D camera;
};

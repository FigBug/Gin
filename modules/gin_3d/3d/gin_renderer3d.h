/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Renders a 3D scene to a juce::Graphics context as wireframes. */
class Renderer3D
{
public:
    Renderer3D() = default;

    /** Renders the scene to the given graphics context.
        @param g        The graphics context to render to
        @param scene    The 3D scene to render
        @param bounds   The area to render into
    */
    void render (juce::Graphics& g, const Scene3D& scene, juce::Rectangle<int> bounds)
    {
        render (g, scene, bounds.toFloat());
    }

    void render (juce::Graphics& g, const Scene3D& scene, juce::Rectangle<float> bounds)
    {
        const auto& camera = scene.getCamera();
        Mat4f viewProj = camera.getViewProjectionMatrix();

        float halfWidth = bounds.getWidth() / 2.0f;
        float halfHeight = bounds.getHeight() / 2.0f;
        float centreX = bounds.getCentreX();
        float centreY = bounds.getCentreY();

        for (const auto& obj : scene.getObjects())
        {
            if (! obj->isVisible())
                continue;

            Mat4f modelViewProj = viewProj * obj->getTransform();

            // Render points
            for (const auto& point : obj->getPoints())
            {
                auto clipPos = modelViewProj.project (point.position);

                // Skip if behind the camera
                if (clipPos[3] <= 0)
                    continue;

                // Perspective divide
                float x = clipPos[0] / clipPos[3];
                float y = clipPos[1] / clipPos[3];

                // Convert to screen coordinates
                float screenX = centreX + x * halfWidth;
                float screenY = centreY - y * halfHeight;

                // Draw the point as a filled ellipse
                g.setColour (point.colour);
                float halfSize = point.size / 2.0f;
                g.fillEllipse (screenX - halfSize, screenY - halfSize, point.size, point.size);
            }

            // Render lines
            for (const auto& line : obj->getLines())
            {
                // Transform vertices to clip space
                auto clipStart = modelViewProj.project (line.start);
                auto clipEnd = modelViewProj.project (line.end);

                // Simple near-plane clipping
                if (clipStart[3] <= 0 && clipEnd[3] <= 0)
                    continue;

                // If one vertex is behind the camera, clip the line
                if (clipStart[3] <= 0 || clipEnd[3] <= 0)
                {
                    if (! clipLine (clipStart, clipEnd))
                        continue;
                }

                // Perspective divide
                float x1 = clipStart[0] / clipStart[3];
                float y1 = clipStart[1] / clipStart[3];
                float x2 = clipEnd[0] / clipEnd[3];
                float y2 = clipEnd[1] / clipEnd[3];

                // Convert to screen coordinates
                float screenX1 = centreX + x1 * halfWidth;
                float screenY1 = centreY - y1 * halfHeight;
                float screenX2 = centreX + x2 * halfWidth;
                float screenY2 = centreY - y2 * halfHeight;

                // Draw the line
                g.setColour (line.colour);
                g.drawLine (screenX1, screenY1, screenX2, screenY2, lineThickness);
            }
        }
    }

    /** Sets the line thickness for rendering. */
    void setLineThickness (float thickness) { lineThickness = thickness; }
    float getLineThickness() const { return lineThickness; }

    /** Enables/disables depth-based line dimming. */
    void setDepthCueing (bool enabled) { depthCueing = enabled; }
    bool getDepthCueing() const { return depthCueing; }

private:
    /** Clips a line against the near plane. Returns false if the line is completely behind. */
    bool clipLine (std::array<float, 4>& start, std::array<float, 4>& end)
    {
        const float nearPlane = 0.001f;

        // Both behind - reject
        if (start[3] <= nearPlane && end[3] <= nearPlane)
            return false;

        // Both in front - accept
        if (start[3] > nearPlane && end[3] > nearPlane)
            return true;

        // One behind, one in front - clip
        float t = (nearPlane - start[3]) / (end[3] - start[3]);

        if (start[3] <= nearPlane)
        {
            // Start is behind, interpolate to find clip point
            start[0] = start[0] + t * (end[0] - start[0]);
            start[1] = start[1] + t * (end[1] - start[1]);
            start[2] = start[2] + t * (end[2] - start[2]);
            start[3] = nearPlane;
        }
        else
        {
            // End is behind, interpolate to find clip point
            end[0] = start[0] + t * (end[0] - start[0]);
            end[1] = start[1] + t * (end[1] - start[1]);
            end[2] = start[2] + t * (end[2] - start[2]);
            end[3] = nearPlane;
        }

        return true;
    }

    float lineThickness = 1.0f;
    bool depthCueing = false;
};

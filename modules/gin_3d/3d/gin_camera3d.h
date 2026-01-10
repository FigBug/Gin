/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** A 3D camera that can be positioned and oriented in space. */
class Camera3D
{
public:
    Camera3D() = default;

    /** Sets the camera position. */
    void setPosition (const Vec3f& pos) { position = pos; viewDirty = true; }
    void setPosition (float x, float y, float z) { position = { x, y, z }; viewDirty = true; }

    /** Gets the camera position. */
    Vec3f getPosition() const { return position; }

    /** Sets the point the camera is looking at. */
    void setTarget (const Vec3f& t) { target = t; viewDirty = true; }
    void setTarget (float x, float y, float z) { target = { x, y, z }; viewDirty = true; }

    /** Gets the target point. */
    Vec3f getTarget() const { return target; }

    /** Sets the up vector. */
    void setUpVector (const Vec3f& u) { up = u; viewDirty = true; }

    /** Gets the up vector. */
    Vec3f getUpVector() const { return up; }

    /** Sets up a perspective projection.
        @param fovYDegrees  Field of view in degrees
        @param aspectRatio  Width / height
        @param nearPlane    Near clipping plane distance
        @param farPlane     Far clipping plane distance
    */
    void setPerspective (float fovYDegrees, float aspectRatio, float nearPlane, float farPlane)
    {
        fovY = fovYDegrees * (juce::MathConstants<float>::pi / 180.0f);
        aspect = aspectRatio;
        nearZ = nearPlane;
        farZ = farPlane;
        projectionDirty = true;
    }

    /** Gets the view matrix (world to camera space). */
    const Mat4f& getViewMatrix() const
    {
        if (viewDirty)
        {
            viewMatrix = Mat4f::lookAt (position, target, up);
            viewDirty = false;
        }
        return viewMatrix;
    }

    /** Gets the projection matrix. */
    const Mat4f& getProjectionMatrix() const
    {
        if (projectionDirty)
        {
            projectionMatrix = Mat4f::perspective (fovY, aspect, nearZ, farZ);
            projectionDirty = false;
        }
        return projectionMatrix;
    }

    /** Gets the combined view-projection matrix. */
    Mat4f getViewProjectionMatrix() const
    {
        return getProjectionMatrix() * getViewMatrix();
    }

    /** Orbits the camera around the target point.
        @param deltaYaw     Horizontal rotation in radians
        @param deltaPitch   Vertical rotation in radians
    */
    void orbit (float deltaYaw, float deltaPitch)
    {
        Vec3f offset = position - target;
        float radius = offset.length();

        // Calculate current angles
        float currentPitch = std::asin (offset.y / radius);
        float currentYaw = std::atan2 (offset.x, offset.z);

        // Apply deltas
        currentYaw += deltaYaw;
        currentPitch += deltaPitch;

        // Clamp pitch to avoid gimbal lock
        float maxPitch = juce::MathConstants<float>::pi / 2.0f - 0.01f;
        currentPitch = juce::jlimit (-maxPitch, maxPitch, currentPitch);

        // Calculate new position
        position.x = target.x + radius * std::cos (currentPitch) * std::sin (currentYaw);
        position.y = target.y + radius * std::sin (currentPitch);
        position.z = target.z + radius * std::cos (currentPitch) * std::cos (currentYaw);

        viewDirty = true;
    }

    /** Zooms the camera (moves towards/away from target).
        @param delta    Positive zooms in, negative zooms out
    */
    void zoom (float delta)
    {
        Vec3f offset = position - target;
        float radius = offset.length();
        radius = std::max (0.1f, radius - delta);

        offset = offset.normalized() * radius;
        position = target + offset;
        viewDirty = true;
    }

    /** Pans the camera (moves both position and target).
        @param deltaX   Horizontal movement
        @param deltaY   Vertical movement
    */
    void pan (float deltaX, float deltaY)
    {
        Vec3f forward = (target - position).normalized();
        Vec3f right = forward.cross (up).normalized();
        Vec3f actualUp = right.cross (forward);

        Vec3f movement = right * deltaX + actualUp * deltaY;
        position += movement;
        target += movement;
        viewDirty = true;
    }

private:
    Vec3f position { 0, 0, 5 };
    Vec3f target { 0, 0, 0 };
    Vec3f up { 0, 1, 0 };

    float fovY = juce::MathConstants<float>::pi / 4.0f;  // 45 degrees
    float aspect = 1.0f;
    float nearZ = 0.1f;
    float farZ = 100.0f;

    mutable Mat4f viewMatrix;
    mutable Mat4f projectionMatrix;
    mutable bool viewDirty = true;
    mutable bool projectionDirty = true;
};

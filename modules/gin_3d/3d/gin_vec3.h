/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** A 3D vector class for representing points and directions in 3D space. */
template <typename T>
struct Vec3
{
    T x = 0, y = 0, z = 0;

    Vec3() = default;
    Vec3 (T x_, T y_, T z_) : x (x_), y (y_), z (z_) {}

    Vec3 operator+ (const Vec3& other) const { return { x + other.x, y + other.y, z + other.z }; }
    Vec3 operator- (const Vec3& other) const { return { x - other.x, y - other.y, z - other.z }; }
    Vec3 operator* (T scalar) const { return { x * scalar, y * scalar, z * scalar }; }
    Vec3 operator/ (T scalar) const { return { x / scalar, y / scalar, z / scalar }; }

    Vec3& operator+= (const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vec3& operator-= (const Vec3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vec3& operator*= (T scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vec3& operator/= (T scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    Vec3 operator-() const { return { -x, -y, -z }; }

    bool operator== (const Vec3& other) const { return juce::exactlyEqual (x, other.x) && juce::exactlyEqual (y, other.y) && juce::exactlyEqual (z, other.z); }
    bool operator!= (const Vec3& other) const { return !(*this == other); }

    T dot (const Vec3& other) const { return x * other.x + y * other.y + z * other.z; }

    Vec3 cross (const Vec3& other) const
    {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    T length() const { return std::sqrt (x * x + y * y + z * z); }
    T lengthSquared() const { return x * x + y * y + z * z; }

    Vec3 normalized() const
    {
        T len = length();
        if (len > 0)
            return *this / len;
        return *this;
    }

    void normalize()
    {
        T len = length();
        if (len > 0)
            *this /= len;
    }

    /** Linear interpolation between this vector and another. */
    Vec3 lerp (const Vec3& other, T t) const
    {
        return *this + (other - *this) * t;
    }
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

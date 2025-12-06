/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** A 4x4 matrix class for 3D transformations.
    Stored in column-major order for OpenGL compatibility.
*/
template <typename T>
struct Mat4
{
    // Column-major storage: m[column][row]
    std::array<std::array<T, 4>, 4> m;

    Mat4()
    {
        // Initialize to identity matrix
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? T (1) : T (0);
    }

    /** Creates an identity matrix. */
    static Mat4 identity()
    {
        return Mat4();
    }

    /** Creates a translation matrix. */
    static Mat4 translation (T x, T y, T z)
    {
        Mat4 result;
        result.m[3][0] = x;
        result.m[3][1] = y;
        result.m[3][2] = z;
        return result;
    }

    static Mat4 translation (const Vec3<T>& v)
    {
        return translation (v.x, v.y, v.z);
    }

    /** Creates a scaling matrix. */
    static Mat4 scale (T x, T y, T z)
    {
        Mat4 result;
        result.m[0][0] = x;
        result.m[1][1] = y;
        result.m[2][2] = z;
        return result;
    }

    static Mat4 scale (T s)
    {
        return scale (s, s, s);
    }

    /** Creates a rotation matrix around the X axis (angle in radians). */
    static Mat4 rotationX (T angle)
    {
        Mat4 result;
        T c = std::cos (angle);
        T s = std::sin (angle);
        result.m[1][1] = c;
        result.m[1][2] = s;
        result.m[2][1] = -s;
        result.m[2][2] = c;
        return result;
    }

    /** Creates a rotation matrix around the Y axis (angle in radians). */
    static Mat4 rotationY (T angle)
    {
        Mat4 result;
        T c = std::cos (angle);
        T s = std::sin (angle);
        result.m[0][0] = c;
        result.m[0][2] = -s;
        result.m[2][0] = s;
        result.m[2][2] = c;
        return result;
    }

    /** Creates a rotation matrix around the Z axis (angle in radians). */
    static Mat4 rotationZ (T angle)
    {
        Mat4 result;
        T c = std::cos (angle);
        T s = std::sin (angle);
        result.m[0][0] = c;
        result.m[0][1] = s;
        result.m[1][0] = -s;
        result.m[1][1] = c;
        return result;
    }

    /** Creates a perspective projection matrix.
        @param fovY     Field of view in the Y direction (radians)
        @param aspect   Aspect ratio (width / height)
        @param nearZ    Near clipping plane
        @param farZ     Far clipping plane
    */
    static Mat4 perspective (T fovY, T aspect, T nearZ, T farZ)
    {
        Mat4 result;
        T tanHalfFov = std::tan (fovY / T (2));

        result.m[0][0] = T (1) / (aspect * tanHalfFov);
        result.m[1][1] = T (1) / tanHalfFov;
        result.m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
        result.m[2][3] = T (-1);
        result.m[3][2] = -(T (2) * farZ * nearZ) / (farZ - nearZ);
        result.m[3][3] = T (0);

        return result;
    }

    /** Creates an orthographic projection matrix. */
    static Mat4 orthographic (T left, T right, T bottom, T top, T nearZ, T farZ)
    {
        Mat4 result;
        result.m[0][0] = T (2) / (right - left);
        result.m[1][1] = T (2) / (top - bottom);
        result.m[2][2] = T (-2) / (farZ - nearZ);
        result.m[3][0] = -(right + left) / (right - left);
        result.m[3][1] = -(top + bottom) / (top - bottom);
        result.m[3][2] = -(farZ + nearZ) / (farZ - nearZ);
        return result;
    }

    /** Creates a look-at view matrix. */
    static Mat4 lookAt (const Vec3<T>& eye, const Vec3<T>& target, const Vec3<T>& up)
    {
        Vec3<T> f = (target - eye).normalized();
        Vec3<T> r = f.cross (up).normalized();
        Vec3<T> u = r.cross (f);

        Mat4 result;
        result.m[0][0] = r.x;
        result.m[1][0] = r.y;
        result.m[2][0] = r.z;
        result.m[0][1] = u.x;
        result.m[1][1] = u.y;
        result.m[2][1] = u.z;
        result.m[0][2] = -f.x;
        result.m[1][2] = -f.y;
        result.m[2][2] = -f.z;
        result.m[3][0] = -r.dot (eye);
        result.m[3][1] = -u.dot (eye);
        result.m[3][2] = f.dot (eye);

        return result;
    }

    /** Matrix multiplication. */
    Mat4 operator* (const Mat4& other) const
    {
        Mat4 result;
        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                result.m[col][row] = T (0);
                for (int k = 0; k < 4; ++k)
                    result.m[col][row] += m[k][row] * other.m[col][k];
            }
        }
        return result;
    }

    /** Transform a 3D point (applies full transformation including translation). */
    Vec3<T> transformPoint (const Vec3<T>& v) const
    {
        T w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];
        return {
            (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) / w,
            (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) / w,
            (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) / w
        };
    }

    /** Transform a 3D vector (applies rotation/scale only, no translation). */
    Vec3<T> transformVector (const Vec3<T>& v) const
    {
        return {
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
        };
    }

    /** Project a point and return clip-space coordinates (x, y, z, w). */
    std::array<T, 4> project (const Vec3<T>& v) const
    {
        return {
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0],
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1],
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2],
            m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3]
        };
    }
};

using Mat4f = Mat4<float>;
using Mat4d = Mat4<double>;

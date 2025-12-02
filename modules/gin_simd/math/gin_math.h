#pragma once

/**
    SIMD-accelerated math utilities for audio DSP.

    The math namespace provides optimized mathematical functions with both
    scalar (single float) and SIMD (vectorized) implementations. When SIMD
    is available (GIN_HAS_SIMD), the same function names work with both
    float and mipp::Reg<float> types, automatically selecting the appropriate
    implementation.

    Available Functions:
    - pow2, pow4, pow8: Fast integer power functions
    - lerp: Linear interpolation
    - min, max: Minimum/maximum of two values
    - minVal, maxVal: Extract min/max from SIMD register

    All functions are inlined for maximum performance. SIMD versions process
    4 floats simultaneously on SSE/AVX systems.

    @see mipp::Reg
*/
namespace math
{
/**
    Computes the square of a value (v²).

    @param v Input value
    @return v * v
*/
inline float pow2 (float v) { return v * v; }

/**
    Computes the fourth power of a value (v⁴).

    @param v Input value
    @return v * v * v * v
*/
inline float pow4 (float v) { return v * v * v * v; }

/**
    Computes the eighth power of a value (v⁸).

    @param v Input value
    @return v⁸
*/
inline float pow8 (float v) { return v * v * v * v * v * v * v * v; }

#if GIN_HAS_SIMD
/** SIMD version: Computes the square of 4 values simultaneously. */
inline mipp::Reg<float> pow2 (mipp::Reg<float> v) { return v * v; }
/** SIMD version: Computes the fourth power of 4 values simultaneously. */
inline mipp::Reg<float> pow4 (mipp::Reg<float> v) { return v * v * v * v; }
/** SIMD version: Computes the eighth power of 4 values simultaneously. */
inline mipp::Reg<float> pow8 (mipp::Reg<float> v) { return v * v * v * v * v * v * v * v; }
#endif

/**
    Linear interpolation between two values.

    @param a Start value (when t=0)
    @param b End value (when t=1)
    @param t Interpolation factor (typically 0.0 to 1.0)
    @return Interpolated value: a + t * (b - a)
*/
inline float lerp (float a, float b, float t)  { return a + t * (b - a); }

#if GIN_HAS_SIMD
/** SIMD version: Linear interpolation of 4 value pairs simultaneously. */
inline mipp::Reg<float> lerp (mipp::Reg<float> a, mipp::Reg<float> b, mipp::Reg<float> t)  { return a + t * (b - a); }
#endif

/**
    Returns the minimum of two values.

    @param a First value
    @param b Second value
    @return The smaller of a and b
*/
inline float min (float a, float b) { return std::min (a, b); }

#if GIN_HAS_SIMD
/** SIMD version: Returns minimum of 4 value pairs simultaneously. */
inline mipp::Reg<float> min (mipp::Reg<float> a, mipp::Reg<float> b) { return a.min (b); }
#endif

/**
    Returns the maximum of two values.

    @param a First value
    @param b Second value
    @return The larger of a and b
*/
inline float max (float a, float b) { return std::max (a, b); }

#if GIN_HAS_SIMD
/** SIMD version: Returns maximum of 4 value pairs simultaneously. */
inline mipp::Reg<float> max (mipp::Reg<float> a, mipp::Reg<float> b) { return a.max (b); }
#endif

/**
    Returns the minimum value (scalar version just returns the input).

    @param a Input value
    @return a
*/
inline float minVal (float a) { return a; }

#if GIN_HAS_SIMD
/**
    Extracts the minimum value from a SIMD register.

    Finds the smallest of the 4 floats in the SIMD register.

    @param a SIMD register containing 4 floats
    @return The minimum of the 4 values
*/
inline float minVal (mipp::Reg<float> a)
{
    float vals[4];
    a.store (vals);
    return std::min ({vals[0], vals[1], vals[2], vals[3]});
}
#endif

/**
    Returns the maximum value (scalar version just returns the input).

    @param a Input value
    @return a
*/
inline float maxVal (float a) { return a; }

#if GIN_HAS_SIMD
/**
    Extracts the maximum value from a SIMD register.

    Finds the largest of the 4 floats in the SIMD register.

    @param a SIMD register containing 4 floats
    @return The maximum of the 4 values
*/
inline float maxVal (mipp::Reg<float> a)
{
    float vals[4];
    a.store (vals);
    return std::max ({vals[0], vals[1], vals[2], vals[3]});
}
#endif

}

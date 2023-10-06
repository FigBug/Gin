#pragma once

namespace math
{
inline float pow2 (float v) { return v * v; }
inline float pow4 (float v) { return v * v * v * v; }
inline float pow8 (float v) { return v * v * v * v * v * v * v * v; }

inline mipp::Reg<float> pow2 (mipp::Reg<float> v) { return v * v; }
inline mipp::Reg<float> pow4 (mipp::Reg<float> v) { return v * v * v * v; }
inline mipp::Reg<float> pow8 (mipp::Reg<float> v) { return v * v * v * v * v * v * v * v; }

inline float lerp (float a, float b, float t)  { return a + t * (b - a); }

inline mipp::Reg<float> lerp (mipp::Reg<float> a, mipp::Reg<float> b, mipp::Reg<float> t)  { return a + t * (b - a); }

inline float min (float a, float b) { return std::min (a, b); }
inline mipp::Reg<float> min (mipp::Reg<float> a, mipp::Reg<float> b) { return a.min (b); }

inline float max (float a, float b) { return std::max (a, b); }
inline mipp::Reg<float> max (mipp::Reg<float> a, mipp::Reg<float> b) { return a.max (b); }

inline float minVal (float a) { return a; }
inline float minVal (mipp::Reg<float> a)
{
    float vals[4];
    a.store (vals);
    return std::min ({vals[0], vals[1], vals[2], vals[3]});
}

inline float maxVal (float a) { return a; }
inline float maxVal (mipp::Reg<float> a)
{
    float vals[4];
    a.store (vals);
    return std::max ({vals[0], vals[1], vals[2], vals[3]});
}

}

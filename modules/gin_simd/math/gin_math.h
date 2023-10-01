#pragma once

namespace math
{
inline float pow2 (float v) { return v * v; }
inline float pow4 (float v) { return v * v * v * v; }
inline float pow8 (float v) { return v * v * v * v * v * v * v * v; }

inline mipp::Reg<float> pow2 (mipp::Reg<float> v) { return v * v; }
inline mipp::Reg<float> pow4 (mipp::Reg<float> v) { return v * v * v * v; }
inline mipp::Reg<float> pow8 (mipp::Reg<float> v) { return v * v * v * v * v * v * v * v; }

inline float lerp (float t, float a, float b)  { return a + t * (b - a); }

inline mipp::Reg<float> lerp (mipp::Reg<float> t, mipp::Reg<float> a, mipp::Reg<float> b)  { return a + t * (b - a); }

inline float min (float a, float b) { return std::min (a, b); }
inline mipp::Reg<float> min (mipp::Reg<float> a, mipp::Reg<float> b) { return a.min (b); }

}

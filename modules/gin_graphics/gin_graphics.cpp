/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include <memory>
#include <cmath>

#include "gin_graphics.h"

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #pragma clang diagnostic ignored "-Wfloat-equal"
 #pragma clang diagnostic ignored "-Wcast-align"
#elif _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4127)
 #pragma warning (disable: 4456)
 #pragma warning (disable: 4457)
 #pragma warning (disable: 4244)
#endif

#include "3rdparty/avir/avir.h"
#include "3rdparty/avir/lancir.h"
#if JUCE_INTEL
 #include "3rdparty/avir/avir_float4_sse.h"
#endif

#if __clang__
 #pragma clang diagnostic pop
#elif _MSC_VER
 #pragma warning (pop)
#endif

namespace gin
{
#include "formats/gin_bmpimageformat.cpp"

#include "images/gin_imageeffects.cpp"
#include "images/gin_imageeffects_blending.cpp"
#include "images/gin_imageeffects_stackblur.cpp"
}

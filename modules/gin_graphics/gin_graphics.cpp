/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#ifdef  _WIN32
 #include <Windows.h>
 #include <ctime>
#endif

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
#elif _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4127)
 #pragma warning (disable: 4456)
 #pragma warning (disable: 4457)
 #pragma warning (disable: 4244)
#endif

#include "gin_graphics.h"

#include "3rdparty/avir/avir.h"
#if JUCE_INTEL
 #include "3rdparty/avir/avir_float4_sse.h"
#endif

#if __clang__
 #pragma clang diagnostic pop
#elif _MSC_VER
 #pragma warning (pop)
#endif

// clang on windows uses msvc name mangling for compatibility
#if !JUCE_WINDOWS
 #include <cxxabi.h>
#endif

#ifdef __APPLE__
 #import <Foundation/Foundation.h>
 #include <sys/stat.h>
 #include <sys/time.h>
#endif

#if __has_include("BinaryData.h")
 #include "BinaryData.h"
#endif

namespace gin
{
#include "formats/gin_bmpimageformat.cpp"
#include "formats/gin_webpimageformat.cpp"

#include "images/gin_imageeffects.cpp"
#include "images/gin_imageeffects_blending.cpp"
#include "images/gin_imageeffects_stackblur.cpp"
}

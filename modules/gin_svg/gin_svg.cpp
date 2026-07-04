/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include <cstring>
#include <cmath>
#include <cstdlib>

#include "gin_svg.h"

#if __clang__
 JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wconversion", "-Wshadow", "-Wunused-parameter", "-Wfloat-equal",
                                     "-Wcast-align", "-Wcomma", "-Wunused-function", "-Wunused-variable",
                                     "-Wdeprecated-declarations", "-Wimplicit-float-conversion",
                                     "-Wfloat-conversion", "-Wimplicit-int-conversion",
                                     "-Wsign-conversion", "-Wshorten-64-to-32",
                                     "-Wdouble-promotion", "-Wmissing-field-initializers")
#elif _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4127)
 #pragma warning (disable: 4244)
 #pragma warning (disable: 4456)
 #pragma warning (disable: 4457)
 #pragma warning (disable: 4701)
 #pragma warning (disable: 4702)
#endif

#define NANOSVG_IMPLEMENTATION
#include "3rdparty/nanosvg/nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "3rdparty/nanosvg/nanosvgrast.h"

#if __clang__
 JUCE_END_IGNORE_WARNINGS_GCC_LIKE
#elif _MSC_VER
 #pragma warning (pop)
#endif

namespace gin
{
#include "svg/gin_svg.cpp"
}

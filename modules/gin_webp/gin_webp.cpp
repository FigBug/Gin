/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include <memory>
#include <cmath>

#include "gin_webp.h"

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

#if __clang__
 #pragma clang diagnostic pop
#elif _MSC_VER
 #pragma warning (pop)
#endif

namespace gin
{
#include "formats/gin_webpimageformat.cpp"
}

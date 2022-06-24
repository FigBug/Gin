/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef  _WIN32
 #include <Windows.h>
 #include <ctime>
#endif

//==============================================================================

#include "gin_metadata.h"

//==============================================================================

#if __clang__
 #pragma clang diagnostic push
 #if __has_warning("-Wnull-pointer-subtraction")
  #pragma clang diagnostic ignored "-Wnull-pointer-subtraction"
 #endif
#endif

namespace gin
{

#include "metadata/gin_libjpegpng.cpp"
#include "metadata/gin_commentmetadata.cpp"
#include "metadata/gin_exifmetadata.cpp"
#include "metadata/gin_imagemetadata.cpp"
#include "metadata/gin_iptcmetadata.cpp"
#include "metadata/gin_xmpmetadata.cpp"

}

#if __clang__
 #pragma clang diagnostic pop
#endif

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

#include "gin_gui.h"

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

#include "utilities/gin_util.cpp"
#include "utilities/gin_asyncutilities.cpp"
#include "utilities/gin_openstreetmaps.cpp"
#include "utilities/gin_elevatedfilecopy.cpp"
#include "utilities/gin_layout.cpp"

#include "images/gin_imageutilities.cpp"

#include "components/gin_componentgrid.cpp"
#include "components/gin_componentviewer.cpp"
#include "components/gin_ginlookandfeel.cpp"
#include "components/gin_mapviewer.cpp"
#include "components/gin_propertycomponents.cpp"
#include "components/gin_singlelinetexteditor.cpp"

}

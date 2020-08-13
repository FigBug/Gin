/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#ifdef  _WIN32
 #include <Windows.h>
 #include <ctime>
#else
 #include <sys/mman.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <semaphore.h>
 #include <ctime>
#endif

#ifdef __linux__
 #include <sys/inotify.h>
 #include <limits.h>
 #include <unistd.h>
 #include <sys/stat.h>
 #include <sys/time.h>
#endif

#ifdef __APPLE__
 #import <Foundation/Foundation.h>
 #include <sys/stat.h>
 #include <sys/time.h>
#endif

#if __clang__ || __GNUC__
 #include <cxxabi.h>
#endif

#include <cfloat>

#include "gin.h"

//==============================================================================

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#if _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4100)
 #pragma warning (disable: 4127)
 #pragma warning (disable: 4456)
 #pragma warning (disable: 4457)
 #pragma warning (disable: 4244)
#endif

#include "3rdparty/avir/avir.h"
#if JUCE_INTEL
 #include "3rdparty/avir/avir_float4_sse.h"
#endif

#if __clang__
 #pragma clang diagnostic pop
#endif

#if _MSC_VER
 #pragma warning (pop)
#endif

namespace gin
{

#include "utilities/gin_asyncutilities.cpp"
#include "utilities/gin_downloadmanager.cpp"
#include "utilities/gin_elevatedfilecopy.cpp"
#include "utilities/gin_filesystemwatcher.cpp"
#include "utilities/gin_fileutilities.cpp"
#include "utilities/gin_integrator.cpp"
#include "utilities/gin_leastsquaresregression.cpp"
#include "utilities/gin_linearregression.cpp"
#include "utilities/gin_openstreetmaps.cpp"
#include "utilities/gin_messagepack.cpp"
#include "utilities/gin_realtimeasyncupdater.cpp"
#include "utilities/gin_plist.cpp"
#include "utilities/gin_sharedmemory.cpp"
#include "utilities/gin_spline.cpp"
#include "utilities/gin_systemsemaphore.cpp"
#include "utilities/gin_threading.cpp"
#include "utilities/gin_util.cpp"
#include "utilities/gin_valuetreeobject.cpp"
#include "utilities/gin_valuetreeutilities.cpp"

#include "images/gin_imageeffects.cpp"
#include "images/gin_imageutilities.cpp"
#include "images/gin_imageeffects_blending.cpp"
#include "images/gin_imageeffects_stackblur.cpp"
#include "images/gin_bmpimageformat.cpp"

#include "components/gin_componentviewer.cpp"
#include "components/gin_ginlookandfeel.cpp"
#include "components/gin_mapviewer.cpp"
#include "components/gin_propertycomponents.cpp"
#include "components/gin_singlelinetexteditor.cpp"

}

#include "utilities/gin_equationparser.cpp"

#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#include "3rdparty/muParser/muParser.cpp"
#include "3rdparty/muParser/muParserBase.cpp"
#include "3rdparty/muParser/muParserBytecode.cpp"
#include "3rdparty/muParser/muParserCallback.cpp"
#include "3rdparty/muParser/muParserError.cpp"
#include "3rdparty/muParser/muParserTokenReader.cpp"

#if __clang__
 #pragma clang diagnostic pop
#endif

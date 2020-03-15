/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

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
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "utilities/asyncutilities.cpp"
#include "utilities/downloadmanager.cpp"
#include "utilities/elevatedfilecopy.cpp"
#include "utilities/filesystemwatcher.cpp"
#include "utilities/fileutilities.cpp"
#include "utilities/integrator.cpp"
#include "utilities/leastsquaresregression.cpp"
#include "utilities/linearregression.cpp"
#include "utilities/openstreetmaps.cpp"
#include "utilities/messagepack.cpp"
#include "utilities/realtimeasyncupdater.cpp"
#include "utilities/plist.cpp"
#include "utilities/sharedmemory.cpp"
#include "utilities/spline.cpp"
#include "utilities/systemsemaphore.cpp"
#include "utilities/threading.cpp"
#include "utilities/util.cpp"
#include "utilities/valuetreeobject.cpp"
#include "utilities/valuetreeutilities.cpp"

#include "images/imageeffects.cpp"
#include "images/imageutilities.cpp"
#include "images/imageeffects_blending.cpp"
#include "images/imageeffects_stackblur.cpp"
#include "images/bmpimageformat.cpp"

#include "components/componentviewer.cpp"
#include "components/ginlookandfeel.cpp"
#include "components/mapviewer.cpp"
#include "components/propertycomponents.cpp"
#include "components/singlelinetexteditor.cpp"

}

#include "utilities/equationparser.cpp"

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

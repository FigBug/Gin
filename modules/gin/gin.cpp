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
#endif
#ifdef __linux__
#include <sys/inotify.h>
#include <limits.h>
#include <unistd.h>
#endif
#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

#ifndef  _WIN32
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#endif

#include <cfloat>

#include "gin.h"

//==============================================================================
namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "dsp/audioutil.cpp"
    
#include "utilities/filesystemwatcher.cpp"
#include "utilities/integrator.cpp"
#include "utilities/leastsquaresregression.cpp"
#include "utilities/linearregression.cpp"
#include "utilities/openstreetmaps.cpp"
#include "utilities/sharedmemory.cpp"
#include "utilities/spline.cpp"
#include "utilities/systemsemaphore.cpp"
#include "utilities/threading.cpp"
#include "utilities/util.cpp"
#include "utilities/valuetreeobject.cpp"
    
#include "images/imageeffects.cpp"
#include "images/bmpimageformat.cpp"
    
#include "metadata/libjpegpng.cpp"
#include "metadata/commentmetadata.cpp"
#include "metadata/exifmetadata.cpp"
#include "metadata/imagemetadata.cpp"
#include "metadata/iptcmetadata.cpp"
#include "metadata/xmpmetadata.cpp"

#include "components/mapviewer.cpp"
#include "components/propertycomponents.cpp"
#include "components/plugincomponents.cpp"
#include "components/pluginlookandfeel.cpp"
#include "components/singlelinetexteditor.cpp"

#include "plugin/processor.cpp"
#include "plugin/plugineditor.cpp"
#include "plugin/program.cpp"
#include "plugin/parameter.cpp"

}

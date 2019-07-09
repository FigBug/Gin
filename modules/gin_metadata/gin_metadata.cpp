/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

#ifdef  _WIN32
#include <Windows.h>
#include <ctime>
#endif

//==============================================================================

#include "gin_metadata.h"

//==============================================================================


namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "metadata/libjpegpng.cpp"
#include "metadata/commentmetadata.cpp"
#include "metadata/exifmetadata.cpp"
#include "metadata/imagemetadata.cpp"
#include "metadata/iptcmetadata.cpp"
#include "metadata/xmpmetadata.cpp"

}

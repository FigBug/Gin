/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================
#include "juceex.h"

//==============================================================================
namespace juceex
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;
    
#include "juceex.h"
    
#include "utilities/integrator.cpp"
#include "utilities/leastsquaresregression.cpp"
#include "utilities/linearregression.cpp"
//#include "utilities/mapviewer.cpp"
#include "utilities/openstreetmaps.cpp"
#include "utilities/spline.cpp"
#include "utilities/util.cpp"

}

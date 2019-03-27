/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================

#include "gin_dsp.h"

//==============================================================================

namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "dsp/adsr.cpp"
#include "dsp/audioutil.cpp"

}

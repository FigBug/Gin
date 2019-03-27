/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================

#include "gin_plugin.h"

//==============================================================================

namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "plugin/processor.cpp"
#include "plugin/plugineditor.cpp"
#include "plugin/program.cpp"
#include "plugin/parameter.cpp"

#include "components/plugincomponents.cpp"
#include "components/pluginlookandfeel.cpp"

}

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include "gin_plugin.h"

//==============================================================================

namespace gin
{

#include "components/gin_resources.h"

#include "plugin/gin_modmatrix.cpp"
#include "plugin/gin_noteduration.cpp"
#include "plugin/gin_processor.cpp"
#include "plugin/gin_program.cpp"
#include "plugin/gin_parameter.cpp"

#include "components/gin_plugineditor.cpp"
#include "components/gin_adsrcomponent.cpp"
#include "components/gin_gateeffectcomponent.cpp"
#include "components/gin_lfocomponent.cpp"
#include "components/gin_steplfocomponent.cpp"
#include "components/gin_plugincomponents.cpp"
#include "components/gin_knob.cpp"
#include "components/gin_pluginlookandfeel.cpp"
#include "components/gin_copperlookandfeel.cpp"
#include "components/gin_resources.cpp"

}

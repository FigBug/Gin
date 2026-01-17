/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include "gin_plugin.h"

//==============================================================================

#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>

namespace gin
{

#include "lookandfeel/gin_resources.h"

#include "plugin/gin_modmatrix.cpp"
#include "plugin/gin_midilearn.cpp"
#include "plugin/gin_noteduration.cpp"
#include "plugin/gin_processor.cpp"
#include "plugin/gin_program.cpp"
#include "plugin/gin_parameter.cpp"

#include "components/gin_assets.cpp"
#include "components/gin_plugineditor.cpp"
#include "components/gin_patchbrowser.cpp"
#include "components/gin_adsrcomponent.cpp"
#include "components/gin_gateeffectcomponent.cpp"
#include "components/gin_lfocomponent.cpp"
#include "components/gin_msegcomponent.cpp"
#include "components/gin_steplfocomponent.cpp"
#include "components/gin_readout.cpp"
#include "components/gin_paramcomponent.cpp"
#include "components/gin_horizontalfader.cpp"
#include "components/gin_switch.cpp"
#include "components/gin_select.cpp"
#include "components/gin_midilearnoverlay.cpp"
#include "components/gin_knob.cpp"
#include "components/gin_fader.cpp"
#include "components/gin_modoverlay.cpp"

#include "lookandfeel/gin_images.cpp"
#include "lookandfeel/gin_pluginlookandfeel.cpp"
#include "lookandfeel/gin_copperlookandfeel.cpp"
#include "lookandfeel/gin_resources.cpp"

}

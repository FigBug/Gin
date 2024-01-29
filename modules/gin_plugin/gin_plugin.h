/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                   gin_plugin
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin Plugin Utilities
  description:          Gin Plugin Utilities
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         gin gin_dsp gin_gui gin_graphics juce_audio_utils juce_audio_processors

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_plugin
 /* If you fail to make sure that all your compile units are building JUCE with the same set of
    option flags, then there's a risk that different compile units will treat the classes as having
    different memory layouts, leading to very nasty memory corruption errors when they all get
    linked together. That's why it's best to always include the Introjucer-generated AppConfig.h
    file before any juce headers.

    Note that if you do have an AppConfig.h file and hit this warning, it means that it doesn't
    contain the JUCE_MODULE_AVAILABLE_xxx flags, which are necessary for some inter-module
    functionality to work correctly. In that case, you should either rebuild your AppConfig.h with
    the latest introjucer, or fix it manually to contain these flags.
 */
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

//==============================================================================
/** Config: BUILD_INTERNAL_PLUGINS
 Set this to 1 if you don't want a plugin, but you want to compile
 the plugin directly into your host app.
 */
#ifndef BUILD_INTERNAL_PLUGINS
 #define BUILD_INTERNAL_PLUGINS 0
#endif

//==============================================================================
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <gin/gin.h>
#include <gin_gui/gin_gui.h>
#include <gin_dsp/gin_dsp.h>
#include <gin_graphics/gin_graphics.h>

#include <map>

namespace juce
{
class StandaloneFilterWindow;
}

namespace gin
{

bool wantsAccessibleKeyboard (juce::Component&);

#include "plugin/gin_noteduration.h"
#include "plugin/gin_processor.h"
#include "plugin/gin_program.h"
#include "plugin/gin_parameter.h"
#include "plugin/gin_smoothedparameter.h"
#include "plugin/gin_modmatrix.h"

#include "components/gin_assets.h"
#include "components/gin_pluginalertwindow.h"
#include "components/gin_multiparamcomponent.h"
#include "components/gin_modulation.h"
#include "components/gin_adsrcomponent.h"
#include "components/gin_gateeffectcomponent.h"
#include "components/gin_lfocomponent.h"
#include "components/gin_msegcomponent.h"
#include "components/gin_steplfocomponent.h"
#include "components/gin_plugincomponents.h"
#include "components/gin_knob.h"

#include "lookandfeel/gin_images.h"
#include "lookandfeel/gin_copperlookandfeel.h"
#include "lookandfeel/gin_resources.h"

#include "components/gin_headers.h"
#include "components/gin_parambox.h"
#include "components/gin_patchbrowser.h"
#include "components/gin_plugineditor.h"
#include "components/gin_controlbox.h"
#include "components/gin_scaledplugineditor.h"
#include "components/gin_standaloneapp.h"

}

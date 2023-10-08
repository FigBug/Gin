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

  ID:                   gin_gui
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin GUI Components
  description:          Gin GUI Components
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core juce_graphics juce_events juce_gui_basics juce_gui_extra gin
  OSXFrameworks:        Security

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_gui
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
#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <gin/gin.h>

#include <map>
#include <optional>

namespace gin
{
#include "utilities/gin_asyncutilities.h"
#include "utilities/gin_coalescedtimer.h"
#include "utilities/gin_util.h"
#include "utilities/gin_openstreetmaps.h"
#include "utilities/gin_elevatedfilecopy.h"
#include "utilities/gin_layout.h"

#include "images/gin_imageutilities.h"

#include "components/gin_componentgrid.h"
#include "components/gin_componentutils.h"
#include "components/gin_componentviewer.h"
#include "components/gin_ginlookandfeel.h"
#include "components/gin_mapviewer.h"
#include "components/gin_propertycomponents.h"
#include "components/gin_singlelinetexteditor.h"
#include "components/gin_svgbutton.h"

}

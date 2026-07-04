/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                   gin_controllers
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin Game Controllers
  description:          Cross-platform game controller support (XInput / GameController / joydev)
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core juce_events gin
  OSXFrameworks:        GameController
  windowsLibs:          xinput

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_controllers
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

//==============================================================================
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <gin/gin.h>

#include <array>
#include <cstring>

namespace gin
{
#include "controllers/gin_gamecontroller.h"
#include "controllers/gin_gamecontrollermanager.h"
}

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

  ID:                   gin_svg
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin SVG
  description:          SVG parsing and rendering using NanoSVG
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core juce_graphics

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_svg
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

//==============================================================================
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace gin
{
#include "svg/gin_svg.h"
}

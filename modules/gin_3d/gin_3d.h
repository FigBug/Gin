/*==============================================================================

 Copyright 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/


/*******************************************************************************
 The block below describes the properties of this module, and is read by
 the Projucer to automatically generate project code that uses it.
 For details about the syntax and how to create or use a module, see the
 JUCE Module Format.txt file.


 BEGIN_JUCE_MODULE_DECLARATION

  ID:                   gin_3d
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin 3D Wireframe Engine
  description:          Basic wireframe 3D rendering engine
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core juce_graphics

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_3d
 #ifdef _MSC_VER
  #pragma message ("Have you included your AppConfig.h file before including the JUCE headers?")
 #else
  #warning "Have you included your AppConfig.h file before including the JUCE headers?"
 #endif
#endif

//==============================================================================
#include <cmath>
#include <vector>
#include <array>

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

namespace gin
{
#include "3d/gin_vec3.h"
#include "3d/gin_mat4.h"
#include "3d/gin_camera3d.h"
#include "3d/gin_scene3d.h"
#include "3d/gin_renderer3d.h"
}

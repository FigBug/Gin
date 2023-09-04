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

  ID:                   gin
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin General Utilities
  description:          Gin General Utilities
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core juce_data_structures
  OSXFrameworks:        Security

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin
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
#include <juce_data_structures/juce_data_structures.h>

#if JUCE_MODULE_AVAILABLE_juce_graphics
 #include <juce_graphics/juce_graphics.h>
#endif

#include <algorithm>
#include <map>
#include <cmath>


namespace gin
{
#include "utilities/gin_point.h"
#include "utilities/gin_downloadmanager.h"
#include "utilities/gin_equationparser.h"
#include "utilities/gin_util.h"
#include "utilities/gin_easing.h"
#include "utilities/gin_filesystemwatcher.h"
#include "utilities/gin_fileutilities.h"
#include "utilities/gin_integrator.h"
#include "utilities/gin_lagrange.h"
#include "utilities/gin_leastsquaresregression.h"
#include "utilities/gin_linearregression.h"
#include "utilities/gin_messagepack.h"
#include "utilities/gin_plist.h"
#include "utilities/gin_realtimeasyncupdater.h"
#include "utilities/gin_riffparser.h"
#ifndef JUCE_ANDROID
 #include "utilities/gin_sharedmemory.h"
#endif
#include "utilities/gin_spline.h"
#include "utilities/gin_systemsemaphore.h"
#include "utilities/gin_threading.h"
#include "utilities/gin_valuetreeobject.h"
#include "utilities/gin_valuetreeutilities.h"

#include "geometry/gin_catenary.h"
#include "geometry/gin_geometry.h"

}

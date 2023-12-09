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

  ID:                   gin_simd
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin SIMD helpers
  description:          Gin SIMD helpers
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         juce_core

 END_JUCE_MODULE_DECLARATION

*******************************************************************************/


#pragma once

#ifndef JUCE_MODULE_AVAILABLE_gin_simd
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

JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4309 4244)

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE("-Wimplicit-float-conversion",
                                    "-Wunused-parameter",
                                    "-Wshadow",
                                    "-Wzero-length-array",
                                    "-Wcast-align",
                                    "-Wimplicit-int-conversion",
                                    "-Warray-parameter",
                                    "-Wsign-conversion")

#if JUCE_INTEL
 #ifndef __SSE__
  #define __SSE__ 1
 #endif 
 #ifndef __SSE2__
  #define __SSE2__ 1
 #endif 
#endif

#ifndef GIN_HAS_SIMD
 #ifdef JUCE_32BIT
  #define GIN_HAS_SIMD 0
 #else
  #define GIN_HAS_SIMD 1
 #endif
#endif

#if GIN_HAS_SIMD
 #include "mipp/mipp.h"
#endif

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

JUCE_END_IGNORE_WARNINGS_MSVC

namespace gin
{
#include "math/gin_math.h"
}

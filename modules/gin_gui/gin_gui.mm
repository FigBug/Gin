/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if __APPLE__
 #include "TargetConditionals.h"
 #if TARGET_OS_OSX
  #define JUCE_CORE_INCLUDE_OBJC_HELPERS 1
  #define JUCE_GRAPHICS_INCLUDE_COREGRAPHICS_HELPERS 1
  #import <objc/message.h>
  #import <AppKit/AppKit.h>
 #endif
#endif

#include "gin_gui.cpp"


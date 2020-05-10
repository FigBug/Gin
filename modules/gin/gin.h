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

  ID:               gin
  vendor:           Roland Rabien
  version:          1.0.0
  name:             Gin
  description:      Gin
  website:          www.rabiensoftware.com
  license:          BSD

  dependencies:     juce_gui_basics juce_gui_extra juce_events
  OSXFrameworks:    Security

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
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_events/juce_events.h>

#include <map>

namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::Component;

#include "utilities/asyncutilities.h"
#include "utilities/coalescedtimer.h"
#include "utilities/downloadmanager.h"
#include "utilities/equationparser.h"
#include "utilities/util.h"
#include "utilities/easing.h"
#include "utilities/elevatedfilecopy.h"
#include "utilities/filesystemwatcher.h"
#include "utilities/fileutilities.h"
#include "utilities/integrator.h"
#include "utilities/lagrange.h"
#include "utilities/leastsquaresregression.h"
#include "utilities/linearregression.h"
#include "utilities/openstreetmaps.h"
#include "utilities/messagepack.h"
#include "utilities/plist.h"
#include "utilities/realtimeasyncupdater.h"
#include "utilities/sharedmemory.h"
#include "utilities/spline.h"
#include "utilities/systemsemaphore.h"
#include "utilities/threading.h"
#include "utilities/valuetreeobject.h"
#include "utilities/valuetreeutilities.h"

#include "images/bmpimageformat.h"
#include "images/imageeffects.h"
#include "images/imageutilities.h"

#include "geometry/geometry.h"

#include "components/componentutils.h"
#include "components/componentviewer.h"
#include "components/ginlookandfeel.h"
#include "components/mapviewer.h"
#include "components/propertycomponents.h"
#include "components/singlelinetexteditor.h"

}

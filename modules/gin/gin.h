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
#include "utilities/gin_asyncutilities.h"
#include "utilities/gin_coalescedtimer.h"
#include "utilities/gin_downloadmanager.h"
#include "utilities/gin_equationparser.h"
#include "utilities/gin_util.h"
#include "utilities/gin_easing.h"
#include "utilities/gin_elevatedfilecopy.h"
#include "utilities/gin_filesystemwatcher.h"
#include "utilities/gin_fileutilities.h"
#include "utilities/gin_integrator.h"
#include "utilities/gin_lagrange.h"
#include "utilities/gin_leastsquaresregression.h"
#include "utilities/gin_linearregression.h"
#include "utilities/gin_openstreetmaps.h"
#include "utilities/gin_messagepack.h"
#include "utilities/gin_plist.h"
#include "utilities/gin_realtimeasyncupdater.h"
#include "utilities/gin_sharedmemory.h"
#include "utilities/gin_spline.h"
#include "utilities/gin_systemsemaphore.h"
#include "utilities/gin_threading.h"
#include "utilities/gin_valuetreeobject.h"
#include "utilities/gin_valuetreeutilities.h"

#include "images/gin_bmpimageformat.h"
#include "images/gin_imageeffects.h"
#include "images/gin_imageutilities.h"

#include "geometry/gin_geometry.h"

#include "components/gin_componentutils.h"
#include "components/gin_componentviewer.h"
#include "components/gin_ginlookandfeel.h"
#include "components/gin_mapviewer.h"
#include "components/gin_propertycomponents.h"
#include "components/gin_singlelinetexteditor.h"

}

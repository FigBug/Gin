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

  ID:                   gin_dsp
  vendor:               Roland Rabien
  version:              1.0.0
  name:                 Gin DSP Utilities
  description:          Gin DSP Utilities
  website:              www.rabiensoftware.com
  license:              BSD
  minimumCppStandard:   20

  dependencies:         gin juce_core juce_audio_utils juce_dsp

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
#if defined (__clang__)
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wcast-qual"
#endif

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <gin/gin.h>

#if defined (__clang__)
 #pragma clang diagnostic pop
#endif

#include <map>
#include <random>
#define _USE_MATH_DEFINES
#include <cmath>

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wfloat-equal")

#include "3rdparty/AudioFilter/src/AudioFilterTypes.h"
#include "3rdparty/AudioFilter/src/FilterInstance.h"
#include "3rdparty/AudioFilter/src/ParametricCreator.h"
#include "3rdparty/AudioFilter/src/Response.h"

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

namespace gin
{

#include "dsp/gin_audioutil.h"
#include "dsp/gin_leveltracker.h"
#include "dsp/gin_scratchbuffer.h"

#include "dsp/gin_adsr.h"
#include "dsp/gin_analogadsr.h"
#include "dsp/gin_audiofifo.h"
#include "dsp/gin_bandlimitedlookuptable.h"
#include "dsp/gin_distortion.h"
#include "dsp/gin_delayline.h"
#include "dsp/gin_dynamics.h"
#include "dsp/gin_easedvaluesmoother.h"
#include "dsp/gin_fastmath.h"
#include "dsp/gin_filter.h"
#include "dsp/gin_eq.h"
#include "dsp/gin_gateeffect.h"
#include "dsp/gin_lfo.h"
#include "dsp/gin_midififo.h"
#include "dsp/gin_modulation.h"
#include "dsp/gin_noise.h"
#include "dsp/gin_oscillators.h"
#include "dsp/gin_platereverb.h"
#include "dsp/gin_resamplingfifo.h"
#include "dsp/gin_sample.h"
#include "dsp/gin_sampleoscillator.h"
#include "dsp/gin_simpleverb.h"
#include "dsp/gin_steplfo.h"
#include "dsp/gin_stereodelay.h"
#include "dsp/gin_stereoenhancer.h"
#include "dsp/gin_stereoprocessor.h"
#include "dsp/gin_synthesiser.h"
#include "dsp/gin_valuesmoother.h"
#include "dsp/gin_wtoscillators.h"
#include "dsp/gin_audiofunctions.h"
#include "dsp/gin_textrenderer.h"
#include "dsp/gin_audioequationparser.h"

#include "components/gin_dynamicsmeter.h"
#include "components/gin_levelmeter.h"
#include "components/gin_triggeredscope.h"
#include "components/gin_waveformcomponent.h"
#include "components/gin_wavetablecomponent.h"
#include "components/gin_xyscope.h"

}

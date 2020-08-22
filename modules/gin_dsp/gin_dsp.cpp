/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#include "gin_dsp.h"

//==============================================================================
#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #pragma clang diagnostic ignored "-Wc99-extensions"
#endif

#if _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4200 4815 4127 4244 4305)
#endif

#define PACKAGE "src"
#define VERSION "0.1.9"
#define CPU_CLIPS_NEGATIVE 0
#define CPU_CLIPS_POSITIVE 0

extern "C"
{
#include "3rdparty/src/samplerate.h"
#include "3rdparty/src/src_linear.c"
#include "3rdparty/src/src_sinc.c"
#include "3rdparty/src/src_zoh.c"
#include "3rdparty/src/samplerate.c"
}

#if __clang__
 #pragma clang diagnostic pop
#endif

#include <cmath>

namespace gin
{

#include "dsp/gin_adsr.cpp"
#include "dsp/gin_analogadsr.cpp"
#include "dsp/gin_audiofunctions.cpp"
#include "dsp/gin_audioequationparser.cpp"
#include "dsp/gin_audioutil.cpp"
#include "dsp/gin_bandlimitedlookuptable.cpp"
#include "dsp/gin_dynamics.cpp"
#include "dsp/gin_filter.cpp"
#include "dsp/gin_lfo.cpp"
#include "dsp/gin_modulation.cpp"
#include "dsp/gin_leveltracker.cpp"
#include "dsp/gin_oscillators.cpp"
#include "dsp/gin_resamplingfifo.cpp"
#include "dsp/gin_scratchbuffer.cpp"
#include "dsp/gin_sampleoscillator.cpp"
#include "dsp/gin_sample.cpp"
#include "dsp/gin_wtoscillators.cpp"

#include "components/gin_dynamicsmeter.cpp"
#include "components/gin_levelmeter.cpp"
#include "components/gin_triggeredscope.cpp"
#include "components/gin_waveformcomponent.cpp"
#include "components/gin_xyscope.cpp"

}


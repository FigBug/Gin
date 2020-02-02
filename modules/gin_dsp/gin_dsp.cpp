/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

// Your project must contain an AppConfig.h file with your project-specific settings in it,
// and your header search path must make it accessible to the module's files.
#include "AppConfig.h"

//==============================================================================

#include "gin_dsp.h"

//==============================================================================
#if __clang__
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wunused-parameter"
 #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
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

namespace gin
{
using namespace juce;
using juce::Rectangle;
using juce::MemoryBlock;

#include "dsp/adsr.cpp"
#include "dsp/analogadsr.cpp"
#include "dsp/audiofunctions.cpp"
#include "dsp/audioequationparser.cpp"
#include "dsp/audioutil.cpp"
#include "dsp/oscillators.cpp"
#include "dsp/resamplingfifo.cpp"
#include "dsp/scratchbuffer.cpp"
#include "dsp/sampleoscillator.cpp"
#include "dsp/sample.cpp"

}


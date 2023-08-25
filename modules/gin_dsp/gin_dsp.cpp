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
#elif _MSC_VER
 #pragma warning (push)
 #pragma warning (disable: 4200 4815 4127 4244 4305)
#endif

#undef VERSION
#define PACKAGE "src"
#define VERSION "0.2.2"
#define CPU_CLIPS_NEGATIVE 0
#define CPU_CLIPS_POSITIVE 0

#define ENABLE_SINC_BEST_CONVERTER      1
#define ENABLE_SINC_MEDIUM_CONVERTER    1
#define ENABLE_SINC_FAST_CONVERTER      1

namespace gin
{
    namespace SRC
    {
        #include "3rdparty/SecretRabbitCode/samplerate.h"
        #include "3rdparty/SecretRabbitCode/src_linear.c"
        #include "3rdparty/SecretRabbitCode/src_sinc.c"
        #include "3rdparty/SecretRabbitCode/src_zoh.c"
        #include "3rdparty/SecretRabbitCode/samplerate.c"
    }
}

JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wfloat-equal")

#include "3rdparty/AudioFilter/src/ButterworthCreator.cpp"
#include "3rdparty/AudioFilter/src/ParametricCreator.cpp"
#include "3rdparty/AudioFilter/src/Response.cpp"

JUCE_END_IGNORE_WARNINGS_GCC_LIKE

#if __clang__
 #pragma clang diagnostic pop
#elif _MSC_VER
    #pragma warning (pop)
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
#include "dsp/gin_simpleverb.cpp"
#include "dsp/gin_stereoenhancer.cpp"
#include "dsp/gin_stereoprocessor.cpp"
#include "dsp/gin_textrenderer.cpp"
#include "dsp/gin_wtoscillators.cpp"

#include "components/gin_dynamicsmeter.cpp"
#include "components/gin_levelmeter.cpp"
#include "components/gin_triggeredscope.cpp"
#include "components/gin_waveformcomponent.cpp"
#include "components/gin_wavetablecomponent.cpp"
#include "components/gin_xyscope.cpp"

}

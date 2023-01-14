/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.c"om

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/enc/histogram_enc.c"
#include "3rdparty/webp/dsp/upsampling_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/filters.c"
#include "3rdparty/webp/dsp/rescaler_mips32.c"
#include "3rdparty/webp/dsp/cpu.c"
#include "3rdparty/webp/enc/quant_enc.c"
#include "3rdparty/webp/dsp/lossless_enc_sse2.c"
#include "3rdparty/webp/dsp/cost.c"
#include "3rdparty/webp/dsp/upsampling_neon.c"

#if defined (HAS_SSE41)
#include "3rdparty/webp/dsp/yuv_sse41.c"
#endif

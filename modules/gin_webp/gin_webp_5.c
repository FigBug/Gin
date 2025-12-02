/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/dec/io_dec.c"
#include "3rdparty/webp/dsp/cost_mips32.c"
#include "3rdparty/webp/dsp/filters_sse2.c"
#include "3rdparty/webp/utils/thread_utils.c"
#include "3rdparty/webp/dsp/yuv_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/enc.c"
#include "3rdparty/webp/dsp/enc_sse2.c"
#include "3rdparty/webp/dsp/lossless_enc.c"
#include "3rdparty/webp/dsp/lossless_enc_mips32.c"
#include "3rdparty/webp/dsp/yuv.c"
#include "3rdparty/webp/enc/vp8l_enc.c"
#include "3rdparty/webp/mux/muxread.c"
#include "3rdparty/webp/dsp/lossless_mips_dsp_r2.c"
#include "3rdparty/webp/mux/anim_encode.c"
#include "3rdparty/webp/enc/picture_tools_enc.c"
#include "3rdparty/webp/dsp/lossless_neon.c"

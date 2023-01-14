/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/dsp/dec_clip_tables.c"
#include "3rdparty/webp/dec/vp8_dec.c"
#include "3rdparty/webp/dec/quant_dec.c"
#include "3rdparty/webp/enc/frame_enc.c"
#include "3rdparty/webp/sharpyuv/sharpyuv_csp.c"
#include "3rdparty/webp/sharpyuv/sharpyuv_neon.c"
#include "3rdparty/webp/sharpyuv/sharpyuv_sse2.c"

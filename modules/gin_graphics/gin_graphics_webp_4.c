/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/dec/alpha_dec.c"
#include "3rdparty/webp/enc/near_lossless_enc.c"
#include "3rdparty/webp/dsp/yuv_sse2.c"
#include "3rdparty/webp/utils/rescaler_utils.c"
#include "3rdparty/webp/dsp/rescaler_neon.c"
#include "3rdparty/webp/dsp/lossless_enc_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/rescaler.c"
#include "3rdparty/webp/enc/picture_psnr_enc.c"
#include "3rdparty/webp/dec/buffer_dec.c"
#include "3rdparty/webp/dsp/dec_sse2.c"
#include "3rdparty/webp/dec/webp_dec.c"
#include "3rdparty/webp/dsp/enc_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/yuv_neon.c"
#if defined (HAS_SSE41)
#include "3rdparty/webp/dsp/lossless_sse41.c"
#endif
#include "3rdparty/webp/demux/anim_decode.c"
#include "3rdparty/webp/sharpyuv/sharpyuv.c"

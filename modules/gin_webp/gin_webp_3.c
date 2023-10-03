/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/dsp/dec_mips32.c"
#include "3rdparty/webp/dsp/lossless_enc_neon.c"
#include "3rdparty/webp/utils/bit_writer_utils.c"
#include "3rdparty/webp/utils/color_cache_utils.c"
#include "3rdparty/webp/enc/cost_enc.c"
#include "3rdparty/webp/dsp/rescaler_sse2.c"
#include "3rdparty/webp/dsp/dec.c"
#include "3rdparty/webp/dsp/filters_neon.c"
#include "3rdparty/webp/dec/vp8l_dec.c"
#include "3rdparty/webp/dec/idec_dec.c"
#include "3rdparty/webp/dsp/alpha_processing_sse2.c"
#include "3rdparty/webp/enc/config_enc.c"
#include "3rdparty/webp/dsp/alpha_processing_mips_dsp_r2.c"
#include "3rdparty/webp/dec/frame_dec.c"
#include "3rdparty/webp/utils/filters_utils.c"
#include "3rdparty/webp/dsp/dec_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/cost_mips_dsp_r2.c"
#include "3rdparty/webp/dec/tree_dec.c"
#include "3rdparty/webp/dsp/rescaler_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/enc_msa.c"
#include "3rdparty/webp/dsp/lossless_sse2.c"
#include "3rdparty/webp/dsp/lossless.c"

#if defined (HAS_SSE41)
#include "3rdparty/webp/dsp/enc_sse41.c"
#endif


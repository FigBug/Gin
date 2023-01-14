/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/enc/backward_references_enc.c"
#include "3rdparty/webp/dsp/enc_mips32.c"
#include "3rdparty/webp/enc/analysis_enc.c"
#include "3rdparty/webp/enc/predictor_enc.c"
#include "3rdparty/webp/dsp/enc_neon.c"
#include "3rdparty/webp/dsp/filters_mips_dsp_r2.c"
#include "3rdparty/webp/dsp/alpha_processing.c"
#include "3rdparty/webp/dsp/cost_sse2.c"
#include "3rdparty/webp/enc/webp_enc.c"
#include "3rdparty/webp/dsp/alpha_processing_neon.c"
#include "3rdparty/webp/utils/huffman_utils.c"
#include "3rdparty/webp/enc/filter_enc.c"
#include "3rdparty/webp/enc/picture_enc.c"
#include "3rdparty/webp/mux/muxedit.c"
#include "3rdparty/webp/dsp/ssim_sse2.c"
#include "3rdparty/webp/enc/picture_csp_enc.c"
#include "3rdparty/webp/dsp/upsampling_sse2.c"
#if defined (HAS_SSE41)
#include "3rdparty/webp/dsp/dec_sse41.c"
#endif

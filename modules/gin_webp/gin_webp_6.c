/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "webp_warnings.h"

#include "3rdparty/webp/utils/quant_levels_dec_utils.c"
#include "3rdparty/webp/dsp/filters_msa.c"
#include "3rdparty/webp/dsp/rescaler_msa.c"
#include "3rdparty/webp/dsp/ssim.c"
#include "3rdparty/webp/dsp/dec_msa.c"
#include "3rdparty/webp/demux/demux.c"
#include "3rdparty/webp/mux/muxinternal.c"
#include "3rdparty/webp/utils/utils.c"
#include "3rdparty/webp/dsp/yuv_mips32.c"
#include "3rdparty/webp/dsp/lossless_msa.c"
#include "3rdparty/webp/utils/huffman_encode_utils.c"
#include "3rdparty/webp/enc/token_enc.c"
#include "3rdparty/webp/enc/backward_references_cost_enc.c"
#include "3rdparty/webp/enc/syntax_enc.c"
#include "3rdparty/webp/dsp/lossless_enc_msa.c"
#if defined (HAS_SSE41)
#include "3rdparty/webp/dsp/lossless_enc_sse41.c"
#endif
#include "3rdparty/webp/enc/picture_rescale_enc.c"
#include "3rdparty/webp/dsp/upsampling.c"
#include "3rdparty/webp/utils/bit_reader_utils.c"
#include "3rdparty/webp/utils/quant_levels_utils.c"
#include "3rdparty/webp/enc/iterator_enc.c"
#include "3rdparty/webp/enc/alpha_enc.c"
#include "3rdparty/webp/sharpyuv/sharpyuv_dsp.c"

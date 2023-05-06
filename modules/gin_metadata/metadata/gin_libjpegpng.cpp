/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================

#if JUCE_MSVC
 #pragma warning (push)
 #pragma warning (disable: 4390 4611 4365 4267)
  #ifdef __INTEL_COMPILER
   #pragma warning (disable: 2544 2545)
  #endif
#endif

}

namespace juce
{
namespace zlibNamespace
{
#if JUCE_INCLUDE_ZLIB_CODE
 #undef OS_CODE
 #undef fdopen
 #include "juce_core/zip/zlib/zlib.h"
 #undef OS_CODE
 #else
 #include JUCE_ZLIB_INCLUDE_PATH
 #endif
}
}

namespace gin
{
#if ! defined (jmp_buf) || ! defined (longjmp)
 #include <setjmp.h>
#endif

namespace pnglibNamespace
{
    using namespace juce::zlibNamespace;

#if JUCE_INCLUDE_PNGLIB_CODE || ! defined (JUCE_INCLUDE_PNGLIB_CODE)

#if _MSC_VER != 1310
    using std::calloc; // (causes conflict in VS.NET 2003)
    using std::malloc;
    using std::free;
#endif

  #if JUCE_CLANG
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wsign-conversion"
   #pragma clang diagnostic ignored "-Wfloat-equal"
   #if __has_warning("-Wzero-as-null-pointer-constant")
    #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
   #endif
   #if __has_warning("-Wcomma")
    #pragma clang diagnostic ignored "-Wcomma"
   #endif
  #endif

  #if JUCE_GCC
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wsign-conversion"
   #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
  #endif

#undef check
using std::abs;
#define NO_DUMMY_DECL
#define PNGLCONF_H 1

#if JUCE_ANDROID
 #define PNG_ARM_NEON_SUPPORTED
#endif

#define PNG_16BIT_SUPPORTED
#define PNG_ALIGNED_MEMORY_SUPPORTED
#define PNG_BENIGN_ERRORS_SUPPORTED
#define PNG_BENIGN_READ_ERRORS_SUPPORTED
#define PNG_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_COLORSPACE_SUPPORTED
#define PNG_CONSOLE_IO_SUPPORTED
#define PNG_EASY_ACCESS_SUPPORTED
#define PNG_FIXED_POINT_SUPPORTED
#define PNG_FLOATING_ARITHMETIC_SUPPORTED
#define PNG_FLOATING_POINT_SUPPORTED
#define PNG_FORMAT_AFIRST_SUPPORTED
#define PNG_FORMAT_BGR_SUPPORTED
#define PNG_GAMMA_SUPPORTED
#define PNG_GET_PALETTE_MAX_SUPPORTED
#define PNG_HANDLE_AS_UNKNOWN_SUPPORTED
#define PNG_INCH_CONVERSIONS_SUPPORTED
#define PNG_INFO_IMAGE_SUPPORTED
#define PNG_IO_STATE_SUPPORTED
#define PNG_POINTER_INDEXING_SUPPORTED
#define PNG_PROGRESSIVE_READ_SUPPORTED
#define PNG_READ_16BIT_SUPPORTED
#define PNG_READ_ALPHA_MODE_SUPPORTED
#define PNG_READ_ANCILLARY_CHUNKS_SUPPORTED
#define PNG_READ_BACKGROUND_SUPPORTED
#define PNG_READ_BGR_SUPPORTED
#define PNG_READ_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_READ_COMPOSITE_NODIV_SUPPORTED
#define PNG_READ_COMPRESSED_TEXT_SUPPORTED
#define PNG_READ_EXPAND_16_SUPPORTED
#define PNG_READ_EXPAND_SUPPORTED
#define PNG_READ_FILLER_SUPPORTED
#define PNG_READ_GAMMA_SUPPORTED
#define PNG_READ_GET_PALETTE_MAX_SUPPORTED
#define PNG_READ_GRAY_TO_RGB_SUPPORTED
#define PNG_READ_INTERLACING_SUPPORTED
#define PNG_READ_INT_FUNCTIONS_SUPPORTED
#define PNG_READ_INVERT_ALPHA_SUPPORTED
#define PNG_READ_INVERT_SUPPORTED
#define PNG_READ_OPT_PLTE_SUPPORTED
#define PNG_READ_PACKSWAP_SUPPORTED
#define PNG_READ_PACK_SUPPORTED
#define PNG_READ_QUANTIZE_SUPPORTED
#define PNG_READ_RGB_TO_GRAY_SUPPORTED
#define PNG_READ_SCALE_16_TO_8_SUPPORTED
#define PNG_READ_SHIFT_SUPPORTED
#define PNG_READ_STRIP_16_TO_8_SUPPORTED
#define PNG_READ_STRIP_ALPHA_SUPPORTED
#define PNG_READ_SUPPORTED
#define PNG_READ_SWAP_ALPHA_SUPPORTED
#define PNG_READ_SWAP_SUPPORTED
#define PNG_READ_TEXT_SUPPORTED
#define PNG_READ_TRANSFORMS_SUPPORTED
#define PNG_READ_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_READ_USER_CHUNKS_SUPPORTED
#define PNG_READ_USER_TRANSFORM_SUPPORTED
#define PNG_READ_bKGD_SUPPORTED
#define PNG_READ_cHRM_SUPPORTED
#define PNG_READ_gAMA_SUPPORTED
#define PNG_READ_hIST_SUPPORTED
#define PNG_READ_iCCP_SUPPORTED
#define PNG_READ_iTXt_SUPPORTED
#define PNG_READ_oFFs_SUPPORTED
#define PNG_READ_pCAL_SUPPORTED
#define PNG_READ_pHYs_SUPPORTED
#define PNG_READ_sBIT_SUPPORTED
#define PNG_READ_sCAL_SUPPORTED
#define PNG_READ_sPLT_SUPPORTED
#define PNG_READ_sRGB_SUPPORTED
#define PNG_READ_tEXt_SUPPORTED
#define PNG_READ_tIME_SUPPORTED
#define PNG_READ_tRNS_SUPPORTED
#define PNG_READ_zTXt_SUPPORTED
#define PNG_SAVE_INT_32_SUPPORTED
#define PNG_SAVE_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_SEQUENTIAL_READ_SUPPORTED
#define PNG_SET_CHUNK_CACHE_LIMIT_SUPPORTED
#define PNG_SET_CHUNK_MALLOC_LIMIT_SUPPORTED
#define PNG_SET_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_SET_USER_LIMITS_SUPPORTED
#define PNG_SIMPLIFIED_READ_AFIRST_SUPPORTED
#define PNG_SIMPLIFIED_READ_BGR_SUPPORTED
#define PNG_SIMPLIFIED_WRITE_AFIRST_SUPPORTED
#define PNG_SIMPLIFIED_WRITE_BGR_SUPPORTED
#define PNG_STDIO_SUPPORTED
#define PNG_STORE_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_TEXT_SUPPORTED
#define PNG_TIME_RFC1123_SUPPORTED
#define PNG_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_USER_CHUNKS_SUPPORTED
#define PNG_USER_LIMITS_SUPPORTED
#define PNG_USER_TRANSFORM_INFO_SUPPORTED
#define PNG_USER_TRANSFORM_PTR_SUPPORTED
#define PNG_WARNINGS_SUPPORTED
#define PNG_WRITE_16BIT_SUPPORTED
#define PNG_WRITE_ANCILLARY_CHUNKS_SUPPORTED
#define PNG_WRITE_BGR_SUPPORTED
#define PNG_WRITE_CHECK_FOR_INVALID_INDEX_SUPPORTED
#define PNG_WRITE_COMPRESSED_TEXT_SUPPORTED
#define PNG_WRITE_CUSTOMIZE_ZTXT_COMPRESSION_SUPPORTED
#define PNG_WRITE_FILLER_SUPPORTED
#define PNG_WRITE_FILTER_SUPPORTED
#define PNG_WRITE_FLUSH_SUPPORTED
#define PNG_WRITE_GET_PALETTE_MAX_SUPPORTED
#define PNG_WRITE_INTERLACING_SUPPORTED
#define PNG_WRITE_INT_FUNCTIONS_SUPPORTED
#define PNG_WRITE_INVERT_ALPHA_SUPPORTED
#define PNG_WRITE_INVERT_SUPPORTED
#define PNG_WRITE_OPTIMIZE_CMF_SUPPORTED
#define PNG_WRITE_PACKSWAP_SUPPORTED
#define PNG_WRITE_PACK_SUPPORTED
#define PNG_WRITE_SHIFT_SUPPORTED
#define PNG_WRITE_SUPPORTED
#define PNG_WRITE_SWAP_ALPHA_SUPPORTED
#define PNG_WRITE_SWAP_SUPPORTED
#define PNG_WRITE_TEXT_SUPPORTED
#define PNG_WRITE_TRANSFORMS_SUPPORTED
#define PNG_WRITE_UNKNOWN_CHUNKS_SUPPORTED
#define PNG_WRITE_USER_TRANSFORM_SUPPORTED
#define PNG_WRITE_WEIGHTED_FILTER_SUPPORTED
#define PNG_WRITE_bKGD_SUPPORTED
#define PNG_WRITE_cHRM_SUPPORTED
#define PNG_WRITE_gAMA_SUPPORTED
#define PNG_WRITE_hIST_SUPPORTED
#define PNG_WRITE_iCCP_SUPPORTED
#define PNG_WRITE_iTXt_SUPPORTED
#define PNG_WRITE_oFFs_SUPPORTED
#define PNG_WRITE_pCAL_SUPPORTED
#define PNG_WRITE_pHYs_SUPPORTED
#define PNG_WRITE_sBIT_SUPPORTED
#define PNG_WRITE_sCAL_SUPPORTED
#define PNG_WRITE_sPLT_SUPPORTED
#define PNG_WRITE_sRGB_SUPPORTED
#define PNG_WRITE_tEXt_SUPPORTED
#define PNG_WRITE_tIME_SUPPORTED
#define PNG_WRITE_tRNS_SUPPORTED
#define PNG_WRITE_zTXt_SUPPORTED
#define PNG_bKGD_SUPPORTED
#define PNG_cHRM_SUPPORTED
#define PNG_gAMA_SUPPORTED
#define PNG_hIST_SUPPORTED
#define PNG_iCCP_SUPPORTED
#define PNG_iTXt_SUPPORTED
#define PNG_oFFs_SUPPORTED
#define PNG_pCAL_SUPPORTED
#define PNG_pHYs_SUPPORTED
#define PNG_sBIT_SUPPORTED
#define PNG_sCAL_SUPPORTED
#define PNG_sPLT_SUPPORTED
#define PNG_sRGB_SUPPORTED
#define PNG_tEXt_SUPPORTED
#define PNG_tIME_SUPPORTED
#define PNG_tRNS_SUPPORTED
#define PNG_zTXt_SUPPORTED

#define PNG_STRING_COPYRIGHT "";
#define PNG_STRING_NEWLINE "\n"
#define PNG_LITERAL_SHARP 0x23
#define PNG_LITERAL_LEFT_SQUARE_BRACKET 0x5b
#define PNG_LITERAL_RIGHT_SQUARE_BRACKET 0x5d

#define PNG_API_RULE 0
#define PNG_CALLOC_SUPPORTED
#define PNG_COST_SHIFT 3
#define PNG_DEFAULT_READ_MACROS 1
#define PNG_GAMMA_THRESHOLD_FIXED 5000
#define PNG_IDAT_READ_SIZE PNG_ZBUF_SIZE
#define PNG_INFLATE_BUF_SIZE 1024
#define PNG_MAX_GAMMA_8 11
#define PNG_QUANTIZE_BLUE_BITS 5
#define PNG_QUANTIZE_GREEN_BITS 5
#define PNG_QUANTIZE_RED_BITS 5
#define PNG_TEXT_Z_DEFAULT_COMPRESSION (-1)
#define PNG_TEXT_Z_DEFAULT_STRATEGY 0
#define PNG_WEIGHT_SHIFT 8
#define PNG_ZBUF_SIZE 8192
#define PNG_Z_DEFAULT_COMPRESSION (-1)
#define PNG_Z_DEFAULT_NOFILTER_STRATEGY 0
#define PNG_Z_DEFAULT_STRATEGY 1
#define PNG_sCAL_PRECISION 5
#define PNG_sRGB_PROFILE_CHECKS 2

#define PNG_LINKAGE_API 
#define PNG_LINKAGE_FUNCTION 

#define PNG_ARM_NEON_OPT 0

#if ! defined (PNG_USER_WIDTH_MAX)
 #define PNG_USER_WIDTH_MAX 1000000
#endif

#if ! defined (PNG_USER_HEIGHT_MAX)
 #define PNG_USER_HEIGHT_MAX 1000000
#endif

#define png_debug(a, b)
#define png_debug1(a, b, c)
#define png_debug2(a, b, c, d)

#include "juce_graphics/image_formats/pnglib/png.h"
#include "juce_graphics/image_formats/pnglib/pngconf.h"
#include "juce_graphics/image_formats/pnglib/pngstruct.h"
#include "juce_graphics/image_formats/pnglib/pnginfo.h"

#define PNG_NO_EXTERN
#include "juce_graphics/image_formats/pnglib/png.c"
#include "juce_graphics/image_formats/pnglib/pngerror.c"
#include "juce_graphics/image_formats/pnglib/pngget.c"
#include "juce_graphics/image_formats/pnglib/pngmem.c"
#include "juce_graphics/image_formats/pnglib/pngread.c"
#include "juce_graphics/image_formats/pnglib/pngpread.c"
#include "juce_graphics/image_formats/pnglib/pngrio.c"

void png_do_expand_palette (png_row_infop, png_bytep, png_const_colorp, png_const_bytep, int);
void png_do_expand (png_row_infop, png_bytep, png_const_color_16p);
void png_do_chop (png_row_infop, png_bytep);
void png_do_quantize (png_row_infop, png_bytep, png_const_bytep, png_const_bytep);
void png_do_gray_to_rgb (png_row_infop, png_bytep);
void png_do_unshift (png_row_infop, png_bytep, png_const_color_8p);
void png_do_unpack (png_row_infop, png_bytep);
int png_do_rgb_to_gray (png_structrp, png_row_infop, png_bytep);
void png_do_compose (png_row_infop, png_bytep, png_structrp);
void png_do_gamma (png_row_infop, png_bytep, png_structrp);
void png_do_encode_alpha (png_row_infop, png_bytep, png_structrp);
void png_do_scale_16_to_8 (png_row_infop, png_bytep);
void png_do_expand_16 (png_row_infop, png_bytep);
void png_do_read_filler (png_row_infop, png_bytep, png_uint_32, png_uint_32);
void png_do_read_invert_alpha (png_row_infop, png_bytep);
void png_do_read_swap_alpha (png_row_infop, png_bytep);

#include "juce_graphics/image_formats/pnglib/pngrtran.c"
#include "juce_graphics/image_formats/pnglib/pngrutil.c"
#include "juce_graphics/image_formats/pnglib/pngset.c"
#include "juce_graphics/image_formats/pnglib/pngtrans.c"
#include "juce_graphics/image_formats/pnglib/pngwio.c"
#include "juce_graphics/image_formats/pnglib/pngwrite.c"
#include "juce_graphics/image_formats/pnglib/pngwtran.c"
#include "juce_graphics/image_formats/pnglib/pngwutil.c"

#if JUCE_CLANG
 #pragma clang diagnostic pop
#endif
#else
extern "C"
{
#include <png.h>
#include <pngconf.h>
}
#endif
}

#undef max
#undef min
#undef fdopen

#if JUCE_MSVC
 #pragma warning (pop)
#endif

//==============================================================================

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4365)
#endif

namespace jpeglibNamespace
{
#if JUCE_INCLUDE_JPEGLIB_CODE || ! defined (JUCE_INCLUDE_JPEGLIB_CODE)
#if JUCE_MINGW
    typedef unsigned char boolean;
#endif

#if JUCE_CLANG
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wdeprecated-register"
 #pragma clang diagnostic ignored "-Wcast-align"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
 #if __has_warning("-Wcomma")
  #pragma clang diagnostic ignored "-Wcomma"
 #endif
#endif

#if JUCE_GCC
 #pragma GCC diagnostic push
 #pragma GCC diagnostic ignored "-Wconversion"
 #pragma GCC diagnostic ignored "-Wsign-conversion"
 #pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
 #if __GNUC__ > 5
  #pragma GCC diagnostic ignored "-Wshift-negative-value"
 #endif
#endif

#define JPEG_INTERNALS
#undef FAR
#include "juce_graphics/image_formats/jpglib/jpeglib.h"

#include "juce_graphics/image_formats/jpglib/jcapimin.c"
#include "juce_graphics/image_formats/jpglib/jcapistd.c"
#include "juce_graphics/image_formats/jpglib/jccoefct.c"
#include "juce_graphics/image_formats/jpglib/jccolor.c"
#undef FIX
#include "juce_graphics/image_formats/jpglib/jcdctmgr.c"
#undef CONST_BITS
#include "juce_graphics/image_formats/jpglib/jchuff.c"
#undef emit_byte
#include "juce_graphics/image_formats/jpglib/jcinit.c"
#include "juce_graphics/image_formats/jpglib/jcmainct.c"
#include "juce_graphics/image_formats/jpglib/jcmarker.c"
#include "juce_graphics/image_formats/jpglib/jcmaster.c"
#include "juce_graphics/image_formats/jpglib/jcomapi.c"
#include "juce_graphics/image_formats/jpglib/jcparam.c"
#include "juce_graphics/image_formats/jpglib/jcphuff.c"
#include "juce_graphics/image_formats/jpglib/jcprepct.c"
#include "juce_graphics/image_formats/jpglib/jcsample.c"
#include "juce_graphics/image_formats/jpglib/jctrans.c"
#include "juce_graphics/image_formats/jpglib/jdapistd.c"
#include "juce_graphics/image_formats/jpglib/jdapimin.c"
#include "juce_graphics/image_formats/jpglib/jdatasrc.c"
#include "juce_graphics/image_formats/jpglib/jdcoefct.c"
#undef FIX
#include "juce_graphics/image_formats/jpglib/jdcolor.c"
#undef FIX
#include "juce_graphics/image_formats/jpglib/jddctmgr.c"
#undef CONST_BITS
#undef ASSIGN_STATE
#include "juce_graphics/image_formats/jpglib/jdhuff.c"
#include "juce_graphics/image_formats/jpglib/jdinput.c"
#include "juce_graphics/image_formats/jpglib/jdmainct.c"
#include "juce_graphics/image_formats/jpglib/jdmarker.c"
#include "juce_graphics/image_formats/jpglib/jdmaster.c"
#undef FIX
#include "juce_graphics/image_formats/jpglib/jdmerge.c"
#undef ASSIGN_STATE
#include "juce_graphics/image_formats/jpglib/jdphuff.c"
#include "juce_graphics/image_formats/jpglib/jdpostct.c"
#undef FIX
#include "juce_graphics/image_formats/jpglib/jdsample.c"
#include "juce_graphics/image_formats/jpglib/jdtrans.c"
#include "juce_graphics/image_formats/jpglib/jfdctflt.c"
#include "juce_graphics/image_formats/jpglib/jfdctint.c"
#undef CONST_BITS
#undef MULTIPLY
#undef FIX_0_541196100
#include "juce_graphics/image_formats/jpglib/jfdctfst.c"
#undef FIX_0_541196100
#include "juce_graphics/image_formats/jpglib/jidctflt.c"
#undef CONST_BITS
#undef FIX_1_847759065
#undef MULTIPLY
#undef DEQUANTIZE
#undef DESCALE
#include "juce_graphics/image_formats/jpglib/jidctfst.c"
#undef CONST_BITS
#undef FIX_1_847759065
#undef MULTIPLY
#undef DEQUANTIZE
#include "juce_graphics/image_formats/jpglib/jidctint.c"
#include "juce_graphics/image_formats/jpglib/jidctred.c"
#include "juce_graphics/image_formats/jpglib/jmemmgr.c"
#include "juce_graphics/image_formats/jpglib/jmemnobs.c"
#include "juce_graphics/image_formats/jpglib/jquant1.c"
#include "juce_graphics/image_formats/jpglib/jquant2.c"
#include "juce_graphics/image_formats/jpglib/jutils.c"
#include "juce_graphics/image_formats/jpglib/transupp.c"

#if JUCE_CLANG
#pragma clang diagnostic pop
#endif
#else
#define JPEG_INTERNALS
#undef FAR
#include <jpeglib.h>
#endif
}

#undef max
#undef min

#if JUCE_MSVC
#pragma warning (pop)
#endif

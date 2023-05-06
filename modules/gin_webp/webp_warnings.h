/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if defined (__clang__)
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wdeprecated-register"
 #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #pragma clang diagnostic ignored "-Wimplicit"
 #pragma clang diagnostic ignored "-Wimplicit-int"
 #pragma clang diagnostic ignored "-Wunused-variable"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #pragma clang diagnostic ignored "-Wsign-conversion"
 #pragma clang diagnostic ignored "-Wcast-align"
 #pragma clang diagnostic ignored "-Wextra-semi"
 #pragma clang diagnostic ignored "-Wsign-compare"
 #pragma clang diagnostic ignored "-Wbad-function-cast"
 #pragma clang diagnostic ignored "-Wfloat-equal"
 #pragma clang diagnostic ignored "-Wconditional-uninitialized"
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
#endif

#ifdef _MSC_VER
 #pragma warning(push)
 #pragma warning(disable: 4459)
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4267)
 #pragma warning(disable: 4996)
 #pragma warning(disable: 4127)
 #pragma warning(disable: 4245)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4310)
 #pragma warning(disable: 4389)
#endif

#if defined (__clang__) || defined (__GNUC__)
 #if (__SSE4_1__)
  #define HAS_SSE41
 #endif
#else
 #if defined(_MSC_VER) && _MSC_VER >= 1500 && (defined(_M_X64) || defined(_M_IX86))
  #define HAS_SSE41
 #endif
#endif

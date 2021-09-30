/*==============================================================================

 Copyright 2019 by Roland Rabien
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
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
#endif

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4459)
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
#endif

#include "3rdparty/mbedtls/library/gcm.c"
#include "3rdparty/mbedtls/library/hmac_drbg.c"

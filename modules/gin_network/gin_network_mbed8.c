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
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4310)
#endif

#include "3rdparty/mbedtls/library/constant_time.c"
#include "3rdparty/mbedtls/library/net_sockets.c"
#include "3rdparty/mbedtls/library/ssl_cookie.c"
#include "3rdparty/mbedtls/library/ssl_msg.c"
#include "3rdparty/mbedtls/library/sha1.c"
#include "3rdparty/mbedtls/library/rsa_alt_helpers.c"


/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4310)
 #pragma warning(disable: 4242)
#endif

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

#include "3rdparty/mbedtls/library/ssl_ciphersuites.c"
#include "3rdparty/mbedtls/library/ssl_cli.c"
#include "3rdparty/mbedtls/library/ripemd160.c"
#include "3rdparty/mbedtls/library/x509.c"

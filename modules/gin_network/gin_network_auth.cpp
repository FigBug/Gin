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
 #pragma clang diagnostic ignored "-Wdeprecated-declarations"
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
#endif

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4267)
 #pragma warning(disable: 4477)
 #pragma warning(disable: 4702)
 #pragma warning(disable: 4189)
#endif

#include "3rdparty/liboauthcpp/include/liboauthcpp/liboauthcpp.h"
#include "3rdparty/liboauthcpp/src/base64.cpp"
#include "3rdparty/liboauthcpp/src/HMAC_SHA1.cpp"
#include "3rdparty/liboauthcpp/src/liboauthcpp.cpp"
#include "3rdparty/liboauthcpp/src/SHA1.cpp"
#include "3rdparty/liboauthcpp/src/urlencode.cpp"


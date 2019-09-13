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

#include "3rdparty/mbedtls/library/ssl_ciphersuites.c"
#include "3rdparty/mbedtls/library/ssl_cli.c"
#include "3rdparty/mbedtls/library/ripemd160.c"
#include "3rdparty/mbedtls/library/x509.c"

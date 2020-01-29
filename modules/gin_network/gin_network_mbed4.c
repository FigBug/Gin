/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4245)
 #pragma warning(disable: 4242)
#endif

#include "3rdparty/mbedtls/library/poly1305.c"
#include "3rdparty/mbedtls/library/ssl_srv.c"
#include "3rdparty/mbedtls/library/sha256.c"

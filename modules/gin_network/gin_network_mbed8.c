/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4310)
#endif

#include "3rdparty/mbedtls/library/net_sockets.c"
#include "3rdparty/mbedtls/library/ssl_cookie.c"
#include "3rdparty/mbedtls/library/ssl_msg.c"
#include "3rdparty/mbedtls/library/sha1.c"

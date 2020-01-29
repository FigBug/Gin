/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4459)
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
#endif

#include "3rdparty/mbedtls/library/camellia.c"
#include "3rdparty/mbedtls/library/chachapoly.c"
#include "3rdparty/mbedtls/library/sha512.c"

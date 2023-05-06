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
 #pragma clang diagnostic ignored "-Wcast-align"
 #pragma clang diagnostic ignored "-Wunused-but-set-variable"
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
#endif

#ifdef _WIN32
 #pragma warning(push)
 #pragma warning(disable: 4267)
 #pragma warning(disable: 4477)
 #pragma warning(disable: 4702)
 #pragma warning(disable: 4244)
 #pragma warning(disable: 4245)
 #pragma warning(disable: 4132)
 #pragma warning(disable: 4459)
 #pragma warning(disable: 4005)
 #pragma warning(disable: 4127)
 #pragma warning(disable: 4701)
 #pragma warning(disable: 4242)
#endif

#include "3rdparty/mbedtls/library/aes.c"
#include "3rdparty/mbedtls/library/aesni.c"
#include "3rdparty/mbedtls/library/aria.c"
#include "3rdparty/mbedtls/library/asn1parse.c"
#include "3rdparty/mbedtls/library/asn1write.c"
#include "3rdparty/mbedtls/library/base64.c"
#include "3rdparty/mbedtls/library/bignum.c"
#include "3rdparty/mbedtls/library/ccm.c"
#include "3rdparty/mbedtls/library/chacha20.c"
#include "3rdparty/mbedtls/library/cipher.c"
#include "3rdparty/mbedtls/library/cipher_wrap.c"
#include "3rdparty/mbedtls/library/cmac.c"
#include "3rdparty/mbedtls/library/ctr_drbg.c"
#include "3rdparty/mbedtls/library/debug.c"
#include "3rdparty/mbedtls/library/des.c"
#include "3rdparty/mbedtls/library/dhm.c"
#include "3rdparty/mbedtls/library/ecdh.c"
#include "3rdparty/mbedtls/library/ecdsa.c"
#include "3rdparty/mbedtls/library/ecjpake.c"
#include "3rdparty/mbedtls/library/ecp.c"
#include "3rdparty/mbedtls/library/ecp_curves.c"
#include "3rdparty/mbedtls/library/entropy.c"
#include "3rdparty/mbedtls/library/entropy_poll.c"
#include "3rdparty/mbedtls/library/hkdf.c"
#include "3rdparty/mbedtls/library/md.c"
#include "3rdparty/mbedtls/library/memory_buffer_alloc.c"
#include "3rdparty/mbedtls/library/nist_kw.c"
#include "3rdparty/mbedtls/library/oid.c"
#include "3rdparty/mbedtls/library/padlock.c"
#include "3rdparty/mbedtls/library/pem.c"
#include "3rdparty/mbedtls/library/pk.c"
#include "3rdparty/mbedtls/library/pk_wrap.c"
#include "3rdparty/mbedtls/library/pkcs12.c"
#include "3rdparty/mbedtls/library/pkcs5.c"
#include "3rdparty/mbedtls/library/pkparse.c"
#include "3rdparty/mbedtls/library/pkwrite.c"
#include "3rdparty/mbedtls/library/platform.c"
#include "3rdparty/mbedtls/library/platform_util.c"
#include "3rdparty/mbedtls/library/rsa.c"
#include "3rdparty/mbedtls/library/ssl_ticket.c"
#include "3rdparty/mbedtls/library/ssl_tls.c"
#include "3rdparty/mbedtls/library/threading.c"
#include "3rdparty/mbedtls/library/timing.c"
#include "3rdparty/mbedtls/library/version.c"
#include "3rdparty/mbedtls/library/x509_create.c"
#include "3rdparty/mbedtls/library/x509_crl.c"
#include "3rdparty/mbedtls/library/x509write_crt.c"
#include "3rdparty/mbedtls/library/x509write_csr.c"

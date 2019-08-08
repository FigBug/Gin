/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"

static void debug_func (void* ctx, int level,
                        const char* file, int line, const char* str)
{
    ((void) level);
    fprintf ((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush ((FILE *) ctx);
}

//==============================================================================
class SecureStreamingSocket::Impl
{
public:
    Impl()  { init();     }
    ~Impl() { shutdown(); }
    
    bool connect (const String& remoteHostname, int remotePortNumber, int timeOutMillisecs)
    {
        ignoreUnused (timeOutMillisecs);
        
        int ret = 0;
        String port = String (remotePortNumber);
        if ((ret = mbedtls_net_connect (&server_fd, remoteHostname.toRawUTF8(),
                                        port.toRawUTF8(), MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            DBG("failed: mbedtls_net_connect returned " + String (ret));
            return false;
        }
        
        if ((ret = mbedtls_ssl_config_defaults (&conf,
                                                MBEDTLS_SSL_IS_CLIENT,
                                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                                MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
        {
            DBG("failed: mbedtls_ssl_config_defaults returned " + String (ret));
            return false;
        }
        
        mbedtls_ssl_conf_authmode (&conf, MBEDTLS_SSL_VERIFY_NONE);
        mbedtls_ssl_conf_ca_chain (&conf, &cacert, nullptr);
        mbedtls_ssl_conf_rng (&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
        mbedtls_ssl_conf_dbg (&conf, debug_func, stdout);
        
        if ((ret = mbedtls_ssl_setup (&ssl, &conf )) != 0)
        {
            DBG("failed: mbedtls_ssl_setup returned %d" + String (ret));
            return false;
        }
        
        if ((ret = mbedtls_ssl_set_hostname (&ssl, remoteHostname.toRawUTF8())) != 0)
        {
            DBG("failed: mbedtls_ssl_set_hostname returned " + String (ret));
            return false;
        }
        
        mbedtls_ssl_set_bio (&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, nullptr);
        
        while ((ret = mbedtls_ssl_handshake (&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                DBG("failed: mbedtls_ssl_handshake returned -0x%x\n\n" + String (-ret));
                return false;
            }
        }
        
        return true;
    }
    
    int read (void* destBuffer, int maxBytesToRead, bool blockUntilSpecifiedAmountHasArrived)
    {
        ignoreUnused (blockUntilSpecifiedAmountHasArrived);
        return mbedtls_ssl_read (&ssl, (unsigned char*)destBuffer, (size_t)maxBytesToRead);
    }
    
    int write (const void* sourceBuffer, int numBytesToWrite)
    {
        return mbedtls_ssl_write (&ssl, (const unsigned char*)sourceBuffer, (size_t)numBytesToWrite);
    }
    
private:
    void init()
    {
        mbedtls_net_init (&server_fd);
        mbedtls_ssl_init (&ssl);
        mbedtls_ssl_config_init (&conf);
        mbedtls_x509_crt_init (&cacert);
        mbedtls_ctr_drbg_init (&ctr_drbg);
        mbedtls_entropy_init (&entropy);
        
        int ret = 0;
        
        const char* pers = "gin_ssl_client";
        if ((ret = mbedtls_ctr_drbg_seed (&ctr_drbg, mbedtls_entropy_func, &entropy,
                                          (const unsigned char *)pers,
                                          strlen (pers))) != 0)
        {
            DBG("failed: mbedtls_ctr_drbg_seed returned " + String (ret));
            jassertfalse;
        }
    }
    
    void shutdown()
    {
        mbedtls_net_free (&server_fd);
        mbedtls_ssl_free (&ssl);
        mbedtls_ssl_config_free (&conf);
        mbedtls_ctr_drbg_free (&ctr_drbg);
        mbedtls_entropy_free (&entropy);
    }
    
    URL url;
    
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
};

//==============================================================================
SecureStreamingSocket::SecureStreamingSocket (bool secure)
{
    if (secure)
        impl = std::make_unique<Impl>();
    else
        normalSocket = std::make_unique<StreamingSocket>();
}

bool SecureStreamingSocket::connect (const String& remoteHostname, int remotePortNumber, int timeOutMillisecs)
{
    if (impl != nullptr)
        return impl->connect (remoteHostname, remotePortNumber, timeOutMillisecs);
    return normalSocket->connect (remoteHostname, remotePortNumber, timeOutMillisecs);
}

int SecureStreamingSocket::read (void* destBuffer, int maxBytesToRead, bool blockUntilSpecifiedAmountHasArrived)
{
    if (impl != nullptr)
        return impl->read (destBuffer, maxBytesToRead, blockUntilSpecifiedAmountHasArrived);
    return normalSocket->read (destBuffer, maxBytesToRead, blockUntilSpecifiedAmountHasArrived);
}

int SecureStreamingSocket::write (const void* sourceBuffer, int numBytesToWrite)
{
    if (impl != nullptr)
        return impl->write (sourceBuffer, numBytesToWrite);
    return normalSocket->write (sourceBuffer, numBytesToWrite);
}

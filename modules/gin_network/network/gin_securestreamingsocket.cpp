/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if JUCE_CLANG
 #pragma clang diagnostic push
 #pragma clang diagnostic ignored "-Wconversion"
 #pragma clang diagnostic ignored "-Wshadow"
 #pragma clang diagnostic ignored "-Wdeprecated-register"
 #pragma clang diagnostic ignored "-Wshorten-64-to-32"
 #pragma clang diagnostic ignored "-Wswitch-enum"
 #if __has_warning("-Wzero-as-null-pointer-constant")
  #pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
 #endif
#endif

#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

#if JUCE_CLANG
 #pragma clang diagnostic pop
#endif

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
    Impl()  { init();  }
    ~Impl() { close(); }

    bool connect (const juce::String& remoteHostname, int remotePortNumber, int timeOutMillisecs)
    {
        juce::ignoreUnused (timeOutMillisecs);

        int ret = 0;
        juce::String port = juce::String (remotePortNumber);
        if ((ret = mbedtls_net_connect (&server_fd, remoteHostname.toRawUTF8(),
                                        port.toRawUTF8(), MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            DBG("failed: mbedtls_net_connect returned " + juce::String (ret));
            return false;
        }

        if ((ret = mbedtls_ssl_config_defaults (&conf,
                                                MBEDTLS_SSL_IS_CLIENT,
                                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                                MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
        {
            DBG("failed: mbedtls_ssl_config_defaults returned " + juce::String (ret));
            return false;
        }

        mbedtls_ssl_conf_authmode (&conf, MBEDTLS_SSL_VERIFY_NONE);
        mbedtls_ssl_conf_ca_chain (&conf, &cacert, nullptr);
        mbedtls_ssl_conf_rng (&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
        mbedtls_ssl_conf_dbg (&conf, debug_func, stdout);

        if ((ret = mbedtls_ssl_setup (&ssl, &conf )) != 0)
        {
            DBG("failed: mbedtls_ssl_setup returned %d" + juce::String (ret));
            return false;
        }

        if ((ret = mbedtls_ssl_set_hostname (&ssl, remoteHostname.toRawUTF8())) != 0)
        {
            DBG("failed: mbedtls_ssl_set_hostname returned " + juce::String (ret));
            return false;
        }

        mbedtls_ssl_set_bio (&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, nullptr);

        while ((ret = mbedtls_ssl_handshake (&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                DBG("failed: mbedtls_ssl_handshake returned -0x%x\n\n" + juce::String (-ret));
                return false;
            }
        }

        connected = true;

        return true;
    }

    bool isConnected() const noexcept
    {
        return connected;
    }

    void close()
    {
        if (isConnected())
            shutdown();
    }

    int read (void* destBuffer, int maxBytesToRead, bool blockUntilSpecifiedAmountHasArrived)
    {
        if (blockUntilSpecifiedAmountHasArrived)
        {
            int todo = maxBytesToRead;
            while (todo > 0)
            {
                int res = sslRead (destBuffer, todo);
                if (res > 0)
                {
                    todo -= res;
                    destBuffer = (char*)destBuffer + res;
                }
                else if (res < 0)
                {
                    return maxBytesToRead - todo;
                }
                else
                {
                    juce::Thread::sleep (1);
                }
            }
            return maxBytesToRead - todo;
        }
        else
        {
            mbedtls_net_set_nonblock (&server_fd);
            int res = sslRead (destBuffer, maxBytesToRead);
            mbedtls_net_set_block (&server_fd);
            return res;
        }
    }

    int sslRead (void* destBuffer, int maxBytesToRead)
    {
        int res = mbedtls_ssl_read (&ssl, (unsigned char*)destBuffer, (size_t)maxBytesToRead);

        if (res >= 0)
            return res;

         if (res < 0 && (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE))
             return 0;

        connected = false;
        return -1;
    }

    int write (const void* sourceBuffer, int numBytesToWrite)
    {
        int res = mbedtls_ssl_write (&ssl, (const unsigned char*)sourceBuffer, (size_t)numBytesToWrite);

        if (res >= 0)
            return res;

        if (res < 0 && (res == MBEDTLS_ERR_SSL_WANT_READ || res == MBEDTLS_ERR_SSL_WANT_WRITE))
            return 0;

        connected = false;
        return -1;
    }

    int getRawSocketHandle()
    {
        return server_fd.fd;
    }

    int waitUntilReady (bool readyForReading, int timeoutMsecs)
    {
        if (connected)
        {
            if (readyForReading)
            {
                if (mbedtls_ssl_check_pending (&ssl) == 1)
                    return 1;

                if (mbedtls_ssl_get_bytes_avail (&ssl) >= 1)
                    return 1;

                int res = mbedtls_net_poll (&server_fd, MBEDTLS_NET_POLL_READ, uint32_t (timeoutMsecs));
                if (res > 0)
                    return (res | MBEDTLS_NET_POLL_READ) ? 1 : 0;
                else if (res == 0)
                    return 0;
            }
            else
            {
                return 1;
            }
        }
        return -1;
    }

private:
    void init()
    {
        if (server_fd.fd >= 0)
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
            DBG("failed: mbedtls_ctr_drbg_seed returned " + juce::String (ret));
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

    juce::URL url;

    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    bool connected = false;
};

//==============================================================================
SecureStreamingSocket::SecureStreamingSocket (bool secure)
{
    if (secure)
        impl = std::make_unique<Impl>();
    else
        normalSocket = std::make_unique<juce::StreamingSocket>();
}

SecureStreamingSocket::~SecureStreamingSocket()
{
}

bool SecureStreamingSocket::connect (const juce::String& remoteHostname, int remotePortNumber, int timeOutMillisecs)
{
    if (impl != nullptr)
        return impl->connect (remoteHostname, remotePortNumber, timeOutMillisecs);
    return normalSocket->connect (remoteHostname, remotePortNumber, timeOutMillisecs);
}

bool SecureStreamingSocket::isConnected () const noexcept
{
    if (impl != nullptr)
        return impl->isConnected();
    return normalSocket->isConnected();
}

void SecureStreamingSocket::close()
{
    if (impl != nullptr)
        impl->close();
    normalSocket->close();
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

int SecureStreamingSocket::getRawSocketHandle() const noexcept
{
    if (impl != nullptr)
        return impl->getRawSocketHandle();
    return normalSocket->getRawSocketHandle();
}

int SecureStreamingSocket::waitUntilReady (bool readyForReading, int timeoutMsecs)
{
    if (impl != nullptr)
        return impl->waitUntilReady (readyForReading, timeoutMsecs);
    return normalSocket->waitUntilReady (readyForReading, timeoutMsecs);
}

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Streaming socket with optional TLS/SSL encryption support.

    SecureStreamingSocket provides a unified interface for both secure (HTTPS/TLS)
    and non-secure (HTTP) streaming socket connections. It wraps mbedTLS for secure
    connections and falls back to JUCE's StreamingSocket for non-secure connections.

    Key Features:
    - TLS/SSL support via mbedTLS
    - Fallback to plain sockets when security not required
    - Blocking and non-blocking read modes
    - Timeout support for connections and reads
    - Compatible with WebSocket and HTTP implementations

    Usage:
    @code
    SecureStreamingSocket socket(true); // true = secure (TLS/SSL)

    if (socket.connect("example.com", 443, 3000))
    {
        const char* request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
        socket.write(request, strlen(request));

        char buffer[1024];
        int bytesRead = socket.read(buffer, sizeof(buffer), false);
        if (bytesRead > 0)
        {
            // Process received data
        }

        socket.close();
    }
    @endcode

    Thread Safety:
    Not thread-safe. Use from a single thread or provide external synchronization.

    @see WebSocket, Http, AsyncWebsocket
*/
class SecureStreamingSocket
{
public:
    SecureStreamingSocket (bool secure = true);
    ~SecureStreamingSocket();

    bool connect (const juce::String& remoteHostname, int remotePortNumber, int timeOutMillisecs = 3000);
    void close();

    bool isConnected () const noexcept;

    int read (void* destBuffer, int maxBytesToRead, bool blockUntilSpecifiedAmountHasArrived);
    int write (const void* sourceBuffer, int numBytesToWrite);

    int getRawSocketHandle () const noexcept;
    int waitUntilReady (bool readyForReading, int timeoutMsecs);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
    std::unique_ptr<juce::StreamingSocket> normalSocket;
};

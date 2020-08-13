/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

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

#pragma once

// This code comes from:
// https://github.com/dhbaird/easywsclient
//
// To get the latest version:
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.hpp
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.cpp
//
// Heavily refactored to become gin::Websocket

#include <string>
#include <vector>

struct WSHeaderType
{
    enum Opcode
    {
        CONTINUATION = 0x0,
        TEXT_FRAME = 0x1,
        BINARY_FRAME = 0x2,
        CLOSE = 8,
        PING = 9,
        PONG = 0xa,
    };

    unsigned header_size;
    bool fin;
    bool mask;
    Opcode opcode;
    int N0;
    uint64_t N;
    uint8_t masking_key[4];
};

class WebSocket
{
  public:
    enum ReadyStateValues { CLOSING, CLOSED, OPEN };
    ReadyStateValues readyStateValues;

    // Factories:
    static WebSocket* fromURL (const juce::String& url, const juce::String& origin = {});
    static WebSocket* fromURLNoMask (const juce::String& url, const juce::String& origin = {});
    static WebSocket* fromURL (const juce::String& url, bool useMask, const juce::String& origin);

    static WebSocket* fromURL (const juce::String& url, const juce::String& origin, const juce::StringPairArray& customHeaders);

    ~WebSocket();

    bool readIncoming();
    void poll (int timeout = 0); // timeout in milliseconds

    void send (const juce::String& message);
    void sendBinary (const juce::String& message);
    void sendBinary (const juce::MemoryBlock& message);
    void sendPing();

    void close();
    ReadyStateValues getReadyState() const;

    void dispatch (std::function<void (const juce::MemoryBlock& message, bool isBinary)> callback);

private:
    WebSocket (std::unique_ptr<gin::SecureStreamingSocket>&& socket_, bool useMask);

    void sendData (WSHeaderType::Opcode type, const juce::String& message);
    void sendData (WSHeaderType::Opcode type, const juce::MemoryBlock& message);

    juce::MemoryBlock rxbuf;
    juce::MemoryBlock txbuf;
    juce::MemoryBlock receivedData;

    std::unique_ptr<SecureStreamingSocket> socket;
    int sockfd = 0;
    ReadyStateValues readyState = OPEN;
    bool useMask = false;
    bool isRxBad = false;
    bool isBinary = false;
};

/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/**
    Asynchronous WebSocket client with non-blocking operations.

    AsyncWebsocket provides a thread-safe, asynchronous interface to WebSocket
    connections. All network operations run on a background thread, ensuring that
    none of the public methods block the calling thread. All callbacks are delivered
    on the JUCE message thread for safe UI updates.

    Key Features:
    - Non-blocking connect, disconnect, and send operations
    - Automatic ping/pong keep-alive with configurable interval
    - Thread-safe message queuing for outgoing data
    - Lambda-based callbacks on the message thread
    - Support for both text and binary WebSocket messages
    - Custom header support for authentication and protocols

    Usage:
    Create an AsyncWebsocket with a URL, register callback handlers, then call
    connect(). The connection runs asynchronously on a background thread.

    @code
    AsyncWebsocket ws(juce::URL("wss://example.com/socket"));

    ws.onConnect = []() {
        DBG("Connected!");
    };

    ws.onText = [](const juce::String& message) {
        DBG("Received: " + message);
    };

    ws.onDisconnect = []() {
        DBG("Disconnected");
    };

    ws.connect(); // Non-blocking
    ws.send("Hello, server!");
    @endcode

    Thread Safety:
    - All public methods are thread-safe
    - Callbacks execute on the JUCE message thread
    - Internal message queue protected by critical section

    @see WebSocket, SecureStreamingSocket
*/
class AsyncWebsocket : public juce::Thread
{
public:
    //==============================================================================
    AsyncWebsocket(const juce::URL url, const juce::StringPairArray& customHeaders = {});
    ~AsyncWebsocket() override;

    //==============================================================================
    void connect();
    void disconnect();

    bool isConnected();

    void setPingInterval (int seconds)              { pingInterval = seconds; }

    void send (const juce::String& text);
    void send (const juce::MemoryBlock& binary);
    void sendPing();

    std::function<void ()> onConnect;
    std::function<void ()> onDisconnect;
    std::function<void (const juce::String&)> onText;
    std::function<void (const juce::MemoryBlock&)> onBinary;

private:
    //==============================================================================
    void run() override;
    void process();

    juce::URL url;
    juce::StringPairArray customHeaders;

    //==============================================================================
    enum MessageType
    {
        binaryMsg,
        textMsg,
        pingMsg
    };

    struct QueueData
    {
        QueueData() = default;
        QueueData (juce::String text_) : type (textMsg), text (text_) {}
        QueueData (juce::MemoryBlock data_) : type (binaryMsg), data (data_) {}
        QueueData (MessageType type_) : type (type_) {}

        MessageType type = textMsg;
        juce::String text;
        juce::MemoryBlock data;
    };

    juce::CriticalSection lock;
    juce::Array<QueueData> outgoingQueue;
    double lastPing { juce::Time::getMillisecondCounterHiRes() / 1000 };
    int pingInterval;

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (AsyncWebsocket)
};

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/* Asynchronous access to a Websocket. None of the methods block and
   all callbacks happen via lambdas on the message thread
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

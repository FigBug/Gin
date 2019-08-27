/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/* Asynchronous access to a Websocket. None of the methods block and
   all callbacks happen via lambdas on the message thread
 */
class AsyncWebsocket : public Thread
{
public:
    //==============================================================================
    AsyncWebsocket (const juce::URL url);
    ~AsyncWebsocket() override;

    //==============================================================================
    void connect();
    void disconnect();
    
    bool isConnected();

    void send (const juce::String& text);
    void send (const juce::MemoryBlock& binary);

    std::function<void ()> onConnect;
    std::function<void ()> onDisconnect;
    std::function<void (const juce::String&)> onText;
    std::function<void (const juce::MemoryBlock&)> onBinary;

private:
    //==============================================================================
    void run() override;

    void processIncomingData();
    void processOutgoingData();

    struct Impl;
    std::unique_ptr<Impl> impl;

    URL url;

    //==============================================================================
    struct QueueData
    {
        QueueData() = default;
        QueueData (juce::String text_) : binary (false), text (text_) {}
        QueueData (juce::MemoryBlock data_) : binary (true), data (data_) {}

        bool binary = false;
        juce::String text;
        juce::MemoryBlock data;
    };

    juce::CriticalSection outgoingQueueLock;
    juce::Array<QueueData> outgoingQueue;
    double lastPing { juce::Time::getMillisecondCounterHiRes() / 1000 };

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (AsyncWebsocket)
};

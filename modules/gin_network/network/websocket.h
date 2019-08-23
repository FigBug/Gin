/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
class Websocket : public Thread
{
public:
    //==============================================================================
    Websocket (const juce::URL url);
    ~Websocket() override;

    //==============================================================================
    void connect();
    void disconnect();
    
    bool isConnected();

    void send (const juce::String& text);
    void send (const juce::MemoryBlock& binary);

    std::function<void ()> onConnect, onDisconnect;
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

    //==============================================================================
    JUCE_DECLARE_WEAK_REFERENCEABLE (Websocket)
};

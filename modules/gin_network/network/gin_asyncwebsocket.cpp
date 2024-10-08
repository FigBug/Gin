/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
AsyncWebsocket::AsyncWebsocket(const juce::URL url_, const juce::StringPairArray& headers)
    : Thread("websocket"), url(url_), customHeaders(headers)
{
}

AsyncWebsocket::~AsyncWebsocket()
{
    disconnect();
}

void AsyncWebsocket::disconnect()
{
    signalThreadShouldExit();
    stopThread (1500);
}

void AsyncWebsocket::connect()
{
    startThread (juce::Thread::Priority::normal);
}

void AsyncWebsocket::run()
{
    try
    {
        process();
    }
    catch (...)
    {
    }
}

void AsyncWebsocket::process()
{
    //==============================================================================
    auto processIncomingData = [this](std::unique_ptr<WebSocket>& ws)
    {
        using MM = juce::MessageManager;
        juce::WeakReference<AsyncWebsocket> weakThis = this;

        ws->dispatch([this, weakThis](const juce::MemoryBlock message, bool isBinary)
        {
            auto messageCopy = message;
            MM::callAsync([this, weakThis, messageCopy, isBinary]
            {
                if (weakThis != nullptr)
                {
                    // If we are receiving data, we don't need to ping
                    lastPing = juce::Time::getMillisecondCounterHiRes() / 1000;

                    if (isBinary && onBinary)
                        onBinary(messageCopy);
                    else if (!isBinary && onText)
                        onText(juce::String::fromUTF8((char*)messageCopy.getData(), int(messageCopy.getSize())));
                }
            });
        });
    };

    //==============================================================================
    auto processOutgoingData = [this](std::unique_ptr<WebSocket>& ws)
    {
        juce::ScopedLock sl(lock);
        for (auto& data : outgoingQueue)
        {
            if (data.type == pingMsg)
                ws->sendPing();
            else if (data.type == binaryMsg)
                ws->sendBinary(data.data);
            else if (data.type == textMsg)
                ws->send(data.text);
            else
                jassertfalse;
        }
        outgoingQueue.clear();
    };

    //==============================================================================
    using MM = juce::MessageManager;
    juce::WeakReference<AsyncWebsocket> weakThis = this;

    // Use custom headers when creating WebSocket connection
    auto ws = std::unique_ptr<WebSocket>(WebSocket::fromURL(url.toString(true), {}, customHeaders));

    if (ws != nullptr)
    {
        MM::callAsync([this, weakThis]
        {
            if (weakThis != nullptr && onConnect)
                onConnect();
        });

        while (!threadShouldExit())
        {
            ws->poll(50);
            if (ws->getReadyState() == WebSocket::CLOSED)
                break;

            double now = juce::Time::getMillisecondCounterHiRes() / 1000;
            if (now - lastPing > pingInterval)
            {
                ws->sendPing();
                lastPing = now;
            }

            processIncomingData(ws);
            processOutgoingData(ws);
        }

        if (ws->getReadyState() != WebSocket::CLOSED)
        {
            ws->close();
            ws->poll(0);
        }
    }

    MM::callAsync([this, weakThis]
    {
        if (weakThis != nullptr && onDisconnect)
        {
            auto safeCallback = onDisconnect;
            safeCallback();
        }
    });
}

bool AsyncWebsocket::isConnected()
{
    return isThreadRunning();
}

void AsyncWebsocket::send (const juce::String& text)
{
    juce::ScopedLock sl (lock);
    outgoingQueue.add ({ text });
}

void AsyncWebsocket::send (const juce::MemoryBlock& binary)
{
    juce::ScopedLock sl (lock);
    outgoingQueue.add ({ binary });
}

void AsyncWebsocket::sendPing()
{
    juce::ScopedLock sl (lock);
    // If we are manually sending a ping, delay the automatic pings
    lastPing = juce::Time::getMillisecondCounterHiRes() / 1000;

    outgoingQueue.add ({ pingMsg });
}

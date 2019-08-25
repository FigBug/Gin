/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

struct Websocket::Impl
{
    std::unique_ptr<easywsclient::WebSocket> socket;
};

//==============================================================================
Websocket::Websocket (const juce::URL url_)
    : Thread ("websocket"), url (url_)
{
    impl = std::make_unique<Impl>();
}

Websocket::~Websocket()
{
    disconnect();
}

void Websocket::disconnect()
{
    signalThreadShouldExit();

    if (impl->socket)
        impl->socket->interrupt();

    stopThread (1000);
    impl->socket = nullptr;
}

void Websocket::connect()
{
    startThread();
}

void Websocket::run()
{
    using MM = MessageManager;
    WeakReference<Websocket> weakThis = this;

    if (auto ws = easywsclient::WebSocket::from_url (url.toString (true).toStdString()))
    {
        impl->socket.reset (ws);

        MM::callAsync ([this, weakThis]
                       {
                           if (weakThis != nullptr && onConnect)
                               onConnect();
                       });
        
        while (! threadShouldExit())
        {
            impl->socket->poll (10000);
            if (impl->socket->getReadyState() == easywsclient::WebSocket::CLOSED)
                break;

            double now = Time::getMillisecondCounterHiRes() / 1000;
            if (now - lastPing > 100)
            {
                impl->socket->sendPing();
                lastPing = now;
            }

            processIncomingData();
            processOutgoingData();
        }

        if (impl->socket->getReadyState() != easywsclient::WebSocket::CLOSED)
            impl->socket->close();
    }

    MM::callAsync ([this, weakThis]
                   {
                       if (weakThis != nullptr && onDisconnect)
                           onDisconnect();
                   });
}

void Websocket::processIncomingData()
{
    using MM = MessageManager;
    WeakReference<Websocket> weakThis = this;

    impl->socket->dispatch ([&] (const std::vector<uint8_t>& message, bool isBinary)
                            {
                                std::vector<uint8_t> messageCopy = message;
                                MM::callAsync ([this, weakThis, messageCopy, isBinary]
                                               {
                                                   if (weakThis != nullptr)
                                                   {
                                                       if (isBinary && onBinary)
                                                           onBinary (MemoryBlock (messageCopy.data(), messageCopy.size()));
                                                       else if (! isBinary && onText)
                                                           onText (String::fromUTF8 ((char*)messageCopy.data(), int (messageCopy.size())));
                                                   }
                                               });
                            });
}

void Websocket::processOutgoingData()
{
    ScopedLock sl (outgoingQueueLock);
    for (auto& data : outgoingQueue)
    {
        if (data.binary)
        {
            std::vector<uint8_t> buf;
            buf.insert (buf.end(), (uint8_t*)data.data.getData(), (uint8_t*)data.data.getData() + data.data.getSize());

            impl->socket->sendBinary (buf);
        }
        else
        {
            impl->socket->send (data.text.toStdString());
        }
    }
    outgoingQueue.clear();
}

bool Websocket::isConnected()
{
    return isThreadRunning();
}

void Websocket::send (const juce::String& text)
{
    if (impl->socket != nullptr)
    {
        ScopedLock sl (outgoingQueueLock);

        outgoingQueue.add ({ text });
        impl->socket->interrupt();
    }
}

void Websocket::send (const juce::MemoryBlock& binary)
{
    if (impl->socket != nullptr)
    {
        ScopedLock sl (outgoingQueueLock);

        outgoingQueue.add ({ binary });
        impl->socket->interrupt();
    }
}

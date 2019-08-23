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
    auto& mm = *MessageManager::getInstance();
    WeakReference<Websocket> weakThis = this;

    if (auto ws = easywsclient::WebSocket::from_url (url.toString (true).toStdString()))
    {
        impl->socket.reset (ws);

        mm.callAsync ([this, weakThis]
                      {
                          if (weakThis != nullptr && onConnect)
                              onConnect();
                      });

        while (! threadShouldExit())
        {
            impl->socket->poll (10000);
            if (impl->socket->getReadyState() == easywsclient::WebSocket::CLOSED)
                break;

            impl->socket->sendPing();

            processIncomingData();
            processOutgoingData();
        }

        if (impl->socket->getReadyState() != easywsclient::WebSocket::CLOSED)
            impl->socket->close();


    }

    mm.callAsync ([this, weakThis]
                  {
                      if (weakThis != nullptr && onDisconnect)
                          onDisconnect();
                  });
}

void Websocket::processIncomingData()
{
    auto& mm = *MessageManager::getInstance();
    WeakReference<Websocket> weakThis = this;

    impl->socket->dispatch ([&] (const std::string& message)
                            {
                                std::string messageCopy = message;
                                mm.callAsync ([this, weakThis, messageCopy]
                                              {
                                                  if (weakThis != nullptr && onText)
                                                      onText (String (messageCopy));
                                              });
                            });

    impl->socket->dispatchBinary ([&] (const std::vector<uint8_t>& message)
                                  {
                                      std::vector<uint8_t> messageCopy = message;
                                      mm.callAsync ([this, weakThis, messageCopy]
                                                    {
                                                        if (weakThis != nullptr && onBinary)
                                                            onBinary (MemoryBlock (messageCopy.data(), messageCopy.size()));
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

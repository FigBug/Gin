/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

struct AsyncWebsocket::Impl
{
    std::unique_ptr<easywsclient::WebSocket> socket;
};

//==============================================================================
AsyncWebsocket::AsyncWebsocket (const juce::URL url_)
    : Thread ("websocket"), url (url_)
{
    impl = std::make_unique<Impl>();
}

AsyncWebsocket::~AsyncWebsocket()
{
    disconnect();
}

void AsyncWebsocket::disconnect()
{
    signalThreadShouldExit();

    {
        juce::ScopedLock sl (lock);
        if (impl->socket != nullptr)
            impl->socket->interrupt();
    }

    stopThread (5000);
}

void AsyncWebsocket::connect()
{
    startThread (3);
}

void AsyncWebsocket::run()
{
    using MM = juce::MessageManager;
    juce::WeakReference<AsyncWebsocket> weakThis = this;

    if (auto ws = easywsclient::WebSocket::from_url (url.toString (true).toStdString()))
    {
        {
            juce::ScopedLock sl (lock);
            impl->socket.reset (ws);
        }

        MM::callAsync ([this, weakThis]
                       {
                           if (weakThis != nullptr && onConnect)
                               onConnect();
                       });
        
        while (! threadShouldExit())
        {
            impl->socket->poll (4000);
            if (impl->socket->getReadyState() == easywsclient::WebSocket::CLOSED)
                break;

            double now = juce::Time::getMillisecondCounterHiRes() / 1000;
            if (now - lastPing > pingInterval)
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

    {
		juce::ScopedLock sl (lock);
        impl->socket = nullptr;
    }

    MM::callAsync ([this, weakThis]
                   {
                       if (weakThis != nullptr && onDisconnect)
                           onDisconnect();
                   });
}

void AsyncWebsocket::processIncomingData()
{
	using MM = juce::MessageManager;
	juce::WeakReference<AsyncWebsocket> weakThis = this;

    impl->socket->dispatch ([&] (const std::vector<uint8_t>& message, bool isBinary)
                            {
                                std::vector<uint8_t> messageCopy = message;
                                MM::callAsync ([this, weakThis, messageCopy, isBinary]
                                               {
                                                   if (weakThis != nullptr)
                                                   {
                                                       // if we are receiving data we don't need to ping
													   lastPing = juce::Time::getMillisecondCounterHiRes() / 1000;

                                                       if (isBinary && onBinary)
                                                           onBinary (juce::MemoryBlock (messageCopy.data(), messageCopy.size()));
                                                       else if (! isBinary && onText)
														   onText (juce::String::fromUTF8 ((char*)messageCopy.data(), int (messageCopy.size())));
                                                   }
                                               });
                            });
}

void AsyncWebsocket::processOutgoingData()
{
	juce::ScopedLock sl (lock);
    for (auto& data : outgoingQueue)
    {
        if (data.type == pingMsg)
        {
            impl->socket->sendPing();
        }
        else if (data.type == binaryMsg)
        {
            std::vector<uint8_t> buf;
            buf.insert (buf.end(), (uint8_t*)data.data.getData(), (uint8_t*)data.data.getData() + data.data.getSize());

            impl->socket->sendBinary (buf);
        }
        else if (data.type == textMsg)
        {
            auto buf = data.text.toStdString();
            impl->socket->send (buf);
        }
        else
        {
            jassertfalse;
        }
    }
    outgoingQueue.clear();
}

bool AsyncWebsocket::isConnected()
{
    return isThreadRunning();
}

void AsyncWebsocket::send (const juce::String& text)
{
	juce::ScopedLock sl (lock);
    if (impl->socket != nullptr)
    {
        outgoingQueue.add ({ text });
        impl->socket->interrupt();
    }
}

void AsyncWebsocket::send (const juce::MemoryBlock& binary)
{
	juce::ScopedLock sl (lock);
    if (impl->socket != nullptr)
    {
        outgoingQueue.add ({ binary });
        impl->socket->interrupt();
    }
}

void AsyncWebsocket::sendPing()
{
	juce::ScopedLock sl (lock);
    if (impl->socket != nullptr)
    {
        // If we are manually sending a ping, delay the automatic pings
		lastPing = juce::Time::getMillisecondCounterHiRes() / 1000;

        outgoingQueue.add ({ pingMsg });
        impl->socket->interrupt();
    }
}

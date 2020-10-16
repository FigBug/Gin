/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
AsyncWebsocket::AsyncWebsocket (const juce::URL url_)
    : Thread ("websocket"), url (url_)
{
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
        if (socket != nullptr)
            socket->interrupt();
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

    if (auto ws = WebSocket::fromURL (url.toString (true).toStdString()))
    {
        {
            juce::ScopedLock sl (lock);
            socket.reset (ws);
        }

        MM::callAsync ([this, weakThis]
		{
		   if (weakThis != nullptr && onConnect)
			   onConnect();
		});

        while (! threadShouldExit())
        {
            socket->poll (4000);
            if (socket->getReadyState() == WebSocket::CLOSED)
                break;

            double now = juce::Time::getMillisecondCounterHiRes() / 1000;
            if (now - lastPing > pingInterval)
            {
                socket->sendPing();
                lastPing = now;
            }

            processIncomingData();
            processOutgoingData();
        }

        if (socket->getReadyState() != WebSocket::CLOSED)
            socket->close();
    }

    {
        juce::ScopedLock sl (lock);
        socket = nullptr;
    }

    MM::callAsync ([this, weakThis]
	{
		if (weakThis != nullptr && onDisconnect)
		{
		   // The disconnect callback may cause the websocket to get deleted
		   // Dangerous to use the lambda that is a member function, so make a copy
		   auto safeCallback = onDisconnect;
		   safeCallback ();
		}
	});
}

void AsyncWebsocket::processIncomingData()
{
    using MM = juce::MessageManager;
    juce::WeakReference<AsyncWebsocket> weakThis = this;

    socket->dispatch ([this, weakThis] (const std::vector<uint8_t>& message, bool isBinary)
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
            socket->sendPing();
        }
        else if (data.type == binaryMsg)
        {
            std::vector<uint8_t> buf;
            buf.insert (buf.end(), (uint8_t*)data.data.getData(), (uint8_t*)data.data.getData() + data.data.getSize());

            socket->sendBinary (buf);
        }
        else if (data.type == textMsg)
        {
            auto buf = data.text.toStdString();
            socket->send (buf);
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
    if (socket != nullptr)
    {
        outgoingQueue.add ({ text });
        socket->interrupt();
    }
}

void AsyncWebsocket::send (const juce::MemoryBlock& binary)
{
    juce::ScopedLock sl (lock);
    if (socket != nullptr)
    {
        outgoingQueue.add ({ binary });
        socket->interrupt();
    }
}

void AsyncWebsocket::sendPing()
{
    juce::ScopedLock sl (lock);
    if (socket != nullptr)
    {
        // If we are manually sending a ping, delay the automatic pings
        lastPing = juce::Time::getMillisecondCounterHiRes() / 1000;

        outgoingQueue.add ({ pingMsg });
        socket->interrupt();
    }
}

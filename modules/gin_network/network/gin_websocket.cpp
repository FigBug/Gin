// http://tools.ietf.org/html/rfc6455#section-5.2  Base Framing Protocol
//
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-------+-+-------------+-------------------------------+
// |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
// |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
// |N|V|V|V|       |S|             |   (if payload len==126/127)   |
// | |1|2|3|       |K|             |                               |
// +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
// |     Extended payload length continued, if payload len == 127  |
// + - - - - - - - - - - - - - - - +-------------------------------+
// |                               |Masking-key, if MASK set to 1  |
// +-------------------------------+-------------------------------+
// | Masking-key (continued)       |          Payload Data         |
// +-------------------------------- - - - - - - - - - - - - - - - +
// :                     Payload Data continued ...                :
// + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// |                     Payload Data continued ...                |
// +---------------------------------------------------------------+

WebSocket::WebSocket (std::unique_ptr<gin::SecureStreamingSocket>&& socket_, bool useMask_)
    : socket (std::move (socket_))
    , sockfd (socket->getRawSocketHandle())
    , useMask (useMask_)
{
    jassert (socket != nullptr);
}

WebSocket::~WebSocket()
{
}

WebSocket::ReadyStateValues WebSocket::getReadyState() const
{
    return readyState;
}

bool WebSocket::readIncoming()
{
    bool foundData = false;

    while (true)
    {
        // FD_ISSET(0, &rfds) will be true
        uint8_t buffer[1500];

        auto ret = socket->read (buffer, 1500, false);

        if (ret <= 0)
        {
            break;
        }
        else
        {
            rxbuf.append (buffer, size_t (ret));
            foundData = true;
        }
    }

    return foundData;
}

void WebSocket::poll (int timeout) // timeout in milliseconds
{
    if (readyState == CLOSED)
    {
        jassertfalse;
        return;
    }

    if (! socket->isConnected())
    {
        readyState = CLOSED;
        return;
    }

    if (txbuf.getSize() == 0 && ! readIncoming())
    {
        socket->waitUntilReady (true, timeout);
    }

    // Read incoming data
    readIncoming();

    // Write outgoing data
    while (txbuf.getSize())
    {
        int ret = socket->write (txbuf.getData(), int (txbuf.getSize()));
        if (ret < 0)
        {
            readyState = CLOSED;
            fputs (ret < 0 ? "Connection error!\n" : "Connection closed!\n", stderr);
            break;
        }
        else if (ret > 0)
        {
            txbuf.removeSection (0, size_t (ret));
        }
    }

    if (txbuf.getSize() == 0 && readyState == CLOSING)
        readyState = CLOSED;
}

void WebSocket::dispatch (std::function<void (const juce::MemoryBlock& message, bool isBinary)> callback)
{
    // TODO: consider acquiring a lock on rxbuf...
    if (isRxBad)
        return;

    while (true)
    {
        WSHeaderType ws;
        if (rxbuf.getSize() < 2)
            return; /* Need at least 2 */

        const uint8_t * data = (uint8_t *) rxbuf.getData(); // peek, but don't consume
        ws.fin = (data[0] & 0x80) == 0x80;
        ws.opcode = (WSHeaderType::Opcode) (data[0] & 0x0f);
        ws.mask = (data[1] & 0x80) == 0x80;
        ws.N0 = (data[1] & 0x7f);
        ws.header_size = 2 + (ws.N0 == 126 ? 2 : 0) + (ws.N0 == 127? 8 : 0) + (ws.mask ? 4 : 0);

        if (rxbuf.getSize() < ws.header_size)
            return; /* Need: ws.header_size - rxbuf.size() */

        int i = 0;
        if (ws.N0 < 126)
        {
            ws.N = uint64_t (ws.N0);
            i = 2;
        }
        else if (ws.N0 == 126)
        {
            ws.N = 0;
            ws.N |= ((uint64_t) data[2]) << 8;
            ws.N |= ((uint64_t) data[3]) << 0;
            i = 4;
        }
        else if (ws.N0 == 127)
        {
            ws.N = 0;
            ws.N |= ((uint64_t) data[2]) << 56;
            ws.N |= ((uint64_t) data[3]) << 48;
            ws.N |= ((uint64_t) data[4]) << 40;
            ws.N |= ((uint64_t) data[5]) << 32;
            ws.N |= ((uint64_t) data[6]) << 24;
            ws.N |= ((uint64_t) data[7]) << 16;
            ws.N |= ((uint64_t) data[8]) << 8;
            ws.N |= ((uint64_t) data[9]) << 0;
            i = 10;
            if (ws.N & 0x8000000000000000ull) {
                // https://tools.ietf.org/html/rfc6455 writes the "the most
                // significant bit MUST be 0."
                //
                // We can't drop the frame, because (1) we don't we don't
                // know how much data to skip over to find the next header,
                // and (2) this would be an impractically long length, even
                // if it were valid. So just close() and return immediately
                // for now.
                isRxBad = true;
                fprintf(stderr, "ERROR: Frame has invalid frame length. Closing.\n");
                close();
                return;
            }
        }
        if (ws.mask)
        {
            ws.masking_key[0] = (uint8_t) (((uint8_t) data[i+0]) << 0);
            ws.masking_key[1] = (uint8_t) (((uint8_t) data[i+1]) << 0);
            ws.masking_key[2] = (uint8_t) (((uint8_t) data[i+2]) << 0);
            ws.masking_key[3] = (uint8_t) (((uint8_t) data[i+3]) << 0);
        }
        else
        {
            ws.masking_key[0] = 0;
            ws.masking_key[1] = 0;
            ws.masking_key[2] = 0;
            ws.masking_key[3] = 0;
        }

        // Note: The checks above should hopefully ensure this addition
        //       cannot overflow:
        if (rxbuf.getSize() < ws.header_size + ws.N)
            return; /* Need: ws.header_size+ws.N - rxbuf.size() */

        // We got a whole message, now do something with it:
        if (
               ws.opcode == WSHeaderType::TEXT_FRAME
            || ws.opcode == WSHeaderType::BINARY_FRAME
            || ws.opcode == WSHeaderType::CONTINUATION
        )
        {
            isBinary = (ws.opcode == WSHeaderType::BINARY_FRAME);
            if (ws.mask)
                for (size_t j = 0; j != ws.N; ++j)
                    rxbuf[j + ws.header_size] ^= ws.masking_key[j & 0x3];

            receivedData.append ((uint8_t*)rxbuf.getData() + ws.header_size, ws.N);// just feed

            if (ws.fin)
            {
                callback (receivedData, isBinary);
                receivedData.reset ();
            }
        }
        else if (ws.opcode == WSHeaderType::PING)
        {
            if (ws.mask)
            {
                for (size_t j = 0; j != ws.N; ++j)
                {
                    rxbuf[j + ws.header_size] ^= ws.masking_key[j & 0x3];
                }
            }

            juce::MemoryBlock pongData ((uint8_t*)rxbuf.getData() + ws.header_size, ws.N);
            sendData (WSHeaderType::PONG, pongData);
        }
        else if (ws.opcode == WSHeaderType::PONG)
        {
            // noop
        }
        else if (ws.opcode == WSHeaderType::CLOSE)
        {
            close();
        }
        else
        {
            fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n");
            close();
        }

        rxbuf.removeSection (0, size_t (long (ws.header_size + ws.N)));
    }
}

void WebSocket::sendPing()
{
    juce::String empty;
    sendData(WSHeaderType::PING, empty);
}

void WebSocket::send (const juce::String& message)
{
    sendData (WSHeaderType::TEXT_FRAME, message);
}

void WebSocket::sendBinary (const juce::String& message)
{
    sendData (WSHeaderType::BINARY_FRAME, message);
}

void WebSocket::sendBinary (const juce::MemoryBlock& message)
{
    sendData (WSHeaderType::BINARY_FRAME, message);
}

void WebSocket::sendData (WSHeaderType::Opcode type, const juce::String& message)
{
    const juce::MemoryBlock msg (message.toRawUTF8(), message.getNumBytesAsUTF8());
    sendData (type, msg);
}

void WebSocket::sendData (WSHeaderType::Opcode type, const juce::MemoryBlock& message)
{
    uint64_t message_size = message.getSize();
    auto message_begin = message.begin();
    auto message_end = message.end();

    // TODO:
    // Masking key should (must) be derived from a high quality random
    // number generator, to mitigate attacks on non-WebSocket friendly
    // middleware:
    const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };
    // TODO: consider acquiring a lock on txbuf...
    if (readyState == CLOSING || readyState == CLOSED)
        return;

    juce::MemoryBlock header;
    header.setSize (2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0));
    header.fillWith (0);
    auto headerPtr = (uint8_t*)header.getData();

    headerPtr[0] = uint8_t (0x80 | type);
    if (message_size < 126)
    {
        headerPtr[1] = uint8_t ((message_size & 0xff) | (useMask ? 0x80 : 0));
        if (useMask)
        {
            headerPtr[2] = masking_key[0];
            headerPtr[3] = masking_key[1];
            headerPtr[4] = masking_key[2];
            headerPtr[5] = masking_key[3];
        }
    }
    else if (message_size < 65536)
    {
        headerPtr[1] = 126 | (useMask ? 0x80 : 0);
        headerPtr[2] = (message_size >> 8) & 0xff;
        headerPtr[3] = (message_size >> 0) & 0xff;
        if (useMask)
        {
            headerPtr[4] = masking_key[0];
            headerPtr[5] = masking_key[1];
            headerPtr[6] = masking_key[2];
            headerPtr[7] = masking_key[3];
        }
    }
    else
    {  // TODO: run coverage testing here
        headerPtr[1] = 127 | (useMask ? 0x80 : 0);
        headerPtr[2] = (message_size >> 56) & 0xff;
        headerPtr[3] = (message_size >> 48) & 0xff;
        headerPtr[4] = (message_size >> 40) & 0xff;
        headerPtr[5] = (message_size >> 32) & 0xff;
        headerPtr[6] = (message_size >> 24) & 0xff;
        headerPtr[7] = (message_size >> 16) & 0xff;
        headerPtr[8] = (message_size >>  8) & 0xff;
        headerPtr[9] = (message_size >>  0) & 0xff;
        if (useMask)
        {
            headerPtr[10] = masking_key[0];
            headerPtr[11] = masking_key[1];
            headerPtr[12] = masking_key[2];
            headerPtr[13] = masking_key[3];
        }
    }
    // N.B. - txbuf will keep growing until it can be transmitted over the socket:
    txbuf.append (header.getData(), header.getSize());
    txbuf.append (message_begin, size_t (message_end - message_begin));

    if (useMask)
    {
        size_t message_offset = txbuf.getSize() - message_size;
        for (size_t i = 0; i != message_size; ++i)
            txbuf[message_offset + i] ^= masking_key[i & 0x3];
    }
}

void WebSocket::close()
{
    if (readyState == CLOSING || readyState == CLOSED)
        return;

    readyState = CLOSING;
    uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00}; // last 4 bytes are a masking key
    txbuf.append (closeFrame, sizeof (closeFrame));
}

WebSocket* WebSocket::fromURL (const juce::String& url, bool useMask, const juce::String& origin)
{
    char path[512] = {0};
    char host[512] = {0};
    int port = 0;

    if (url.length() >= 512)
    {
        fprintf(stderr, "ERROR: url size limit exceeded: %s\n", url.toRawUTF8());
        return nullptr;
    }
    if (origin.length() >= 200)
    {
        fprintf(stderr, "ERROR: origin size limit exceeded: %s\n", origin.toRawUTF8());
        return nullptr;
    }

    bool secure = false;
    if (sscanf (url.toRawUTF8(), "wss://%[^:/]:%d/%s", host, &port, path) == 3)
    {
        secure = true;
    }
    else if (sscanf (url.toRawUTF8(), "wss://%[^:/]/%s", host, path) == 2)
    {
        secure = true;
        port = 443;
    }
    else if (sscanf (url.toRawUTF8(), "wss://%[^:/]:%d", host, &port) == 2)
    {
        secure = true;
        path[0] = '\0';
    }
    else if (sscanf (url.toRawUTF8(), "wss://%[^:/]", host) == 1)
    {
        secure = true;
        port = 443;
        path[0] = '\0';
    }
    else if (sscanf (url.toRawUTF8(), "ws://%[^:/]:%d/%s", host, &port, path) == 3)
    {
    }
    else if (sscanf (url.toRawUTF8(), "ws://%[^:/]/%s", host, path) == 2)
    {
        port = 80;
    }
    else if (sscanf (url.toRawUTF8(), "ws://%[^:/]:%d", host, &port) == 2)
    {
        path[0] = '\0';
    }
    else if (sscanf (url.toRawUTF8(), "ws://%[^:/]", host) == 1)
    {
        port = 80;
        path[0] = '\0';
    }
    else
    {
        fprintf(stderr, "ERROR: Could not parse WebSocket url: %s\n", url.toRawUTF8());
        return nullptr;
    }

    auto socket = std::make_unique<gin::SecureStreamingSocket> (secure);
    if (! socket->connect (host, port))
    {
        fprintf(stderr, "Unable to connect to %s:%d\n", host, port);
        return nullptr;
    }

    int sockfd = socket->getRawSocketHandle();

    {
        // XXX: this should be done non-blocking,
        char line[1024] = {0};
        int status;
        int i;

        snprintf (line, 1024, "GET /%s HTTP/1.1\r\n", path);
        socket->write (line, int (strlen (line)));

        if (port == 80)
        {
            snprintf (line, 1024, "Host: %s\r\n", host);
            socket->write (line, int (strlen (line)));
        }
        else
        {
            snprintf (line, 1024, "Host: %s:%d\r\n", host, port);
            socket->write (line, int (strlen (line)));
        }

        snprintf (line, 1024, "Upgrade: websocket\r\n");
        socket->write (line, int (strlen (line)));

        snprintf (line, 1024, "Connection: Upgrade\r\n");
        socket->write (line, int (strlen (line)));

        if (origin.isNotEmpty())
        {
            snprintf (line, 1024, "Origin: %s\r\n", origin.toRawUTF8());
            socket->write (line, int (strlen (line)));
        }

        snprintf (line, 1024, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
        socket->write (line, int (strlen (line)));

        snprintf (line, 1024, "Sec-WebSocket-Version: 13\r\n");
        socket->write (line, int (strlen (line)));

        snprintf (line, 1024, "\r\n");
        socket->write (line, int (strlen (line)));

        for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i)
        {
            if (socket->read (line+i, 1, true) == 0)
                return nullptr;
        }

        line[i] = 0;
        if (i == 1023)
        {
            fprintf(stderr, "ERROR: Got invalid status line connecting to: %s\n", url.toRawUTF8());
            return nullptr;
        }

        if (sscanf (line, "HTTP/1.1 %d", &status) != 1 || status != 101)
        {
            fprintf(stderr, "ERROR: Got bad status connecting to %s: %s", url.toRawUTF8(), line);
            return nullptr;
        }

        // TODO: verify response headers,
        while (true)
        {
            for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i)
                if (socket->read (line+i, 1, true) == 0)
                    return nullptr;

            if (line[0] == '\r' && line[1] == '\n')
                break;
        }
    }
    int flag = 1;
    setsockopt (sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof (flag)); // Disable Nagle's algorithm

    return new WebSocket (std::move (socket), useMask);
}

WebSocket* WebSocket::fromURL(const juce::String& url, const juce::String& origin, const juce::StringPairArray& customHeaders)
{
    char path[512] = {0};
    char host[512] = {0};
    int port = 0;

    // Ensure URL and origin length limits are respected
    if (url.length() >= 512)
    {
        fprintf(stderr, "ERROR: URL size limit exceeded: %s\n", url.toRawUTF8());
        return nullptr;
    }
    if (origin.length() >= 200)
    {
        fprintf(stderr, "ERROR: Origin size limit exceeded: %s\n", origin.toRawUTF8());
        return nullptr;
    }

    bool secure = false;

    // Parse URL for wss:// or ws://
    if (sscanf(url.toRawUTF8(), "wss://%[^:/]:%d/%s", host, &port, path) == 3)
    {
        secure = true;
    }
    else if (sscanf(url.toRawUTF8(), "wss://%[^:/]/%s", host, path) == 2)
    {
        secure = true;
        port = 443;
    }
    else if (sscanf(url.toRawUTF8(), "wss://%[^:/]:%d", host, &port) == 2)
    {
        secure = true;
        path[0] = '\0';
    }
    else if (sscanf(url.toRawUTF8(), "wss://%[^:/]", host) == 1)
    {
        secure = true;
        port = 443;
        path[0] = '\0';
    }
    else if (sscanf(url.toRawUTF8(), "ws://%[^:/]:%d/%s", host, &port, path) == 3)
    {
        // non-secure ws://
    }
    else if (sscanf(url.toRawUTF8(), "ws://%[^:/]/%s", host, path) == 2)
    {
        port = 80;
    }
    else if (sscanf(url.toRawUTF8(), "ws://%[^:/]:%d", host, &port) == 2)
    {
        path[0] = '\0';
    }
    else if (sscanf(url.toRawUTF8(), "ws://%[^:/]", host) == 1)
    {
        port = 80;
        path[0] = '\0';
    }
    else
    {
        fprintf(stderr, "ERROR: Could not parse WebSocket URL: %s\n", url.toRawUTF8());
        return nullptr;
    }

    // Create secure or non-secure socket
    auto socket = std::make_unique<gin::SecureStreamingSocket>(secure);
    if (!socket->connect(host, port))
    {
        fprintf(stderr, "Unable to connect to %s:%d\n", host, port);
        return nullptr;
    }

    int sockfd = socket->getRawSocketHandle();

    // Begin WebSocket handshake
    {
        char line[1024] = {0};
        int status;
        int i;

        snprintf(line, 1024, "GET /%s HTTP/1.1\r\n", path);
        socket->write(line, int(strlen(line)));

        if (port == 80)
        {
            snprintf(line, 1024, "Host: %s\r\n", host);
            socket->write(line, int(strlen(line)));
        }
        else
        {
            snprintf(line, 1024, "Host: %s:%d\r\n", host, port);
            socket->write(line, int(strlen(line)));
        }

        // Standard WebSocket headers
        snprintf(line, 1024, "Upgrade: websocket\r\n");
        socket->write(line, int(strlen(line)));

        snprintf(line, 1024, "Connection: Upgrade\r\n");
        socket->write(line, int(strlen(line)));

        if (origin.isNotEmpty())
        {
            snprintf(line, 1024, "Origin: %s\r\n", origin.toRawUTF8());
            socket->write(line, int(strlen(line)));
        }

        // Send Sec-WebSocket headers
        snprintf(line, 1024, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
        socket->write(line, int(strlen(line)));

        snprintf(line, 1024, "Sec-WebSocket-Version: 13\r\n");
        socket->write(line, int(strlen(line)));

        // Custom Headers
        const juce::StringArray& headerKeys = customHeaders.getAllKeys();
        const juce::StringArray& headerValues = customHeaders.getAllValues();

        for (int j = 0; j < headerKeys.size(); ++j)
        {
            snprintf(line, 1024, "%s: %s\r\n", headerKeys[j].toRawUTF8(), headerValues[j].toRawUTF8());
            socket->write(line, int(strlen(line)));
        }
        
        // End the header section
        snprintf(line, 1024, "\r\n");
        socket->write(line, int(strlen(line)));

        // Read server's handshake response
        for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i)
        {
            if (socket->read(line + i, 1, true) == 0)
                return nullptr;
        }

        line[i] = 0;
        if (i == 1023)
        {
            fprintf(stderr, "ERROR: Got invalid status line connecting to: %s\n", url.toRawUTF8());
            return nullptr;
        }

        if (sscanf(line, "HTTP/1.1 %d", &status) != 1 || status != 101)
        {
            fprintf(stderr, "ERROR: Got bad status connecting to %s: %s", url.toRawUTF8(), line);
            return nullptr;
        }

        // Validate response headers
        while (true)
        {
            for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i)
                if (socket->read(line + i, 1, true) == 0)
                    return nullptr;

            if (line[0] == '\r' && line[1] == '\n')
                break;
        }
    }

    // Disable Nagle's algorithm
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));

    // Return WebSocket object
    return new WebSocket(std::move(socket), true);
}
WebSocket* WebSocket::fromURL (const juce::String& url, const juce::String& origin)
{
    return WebSocket::fromURL (url, true, origin);
}

WebSocket* WebSocket::fromURLNoMask (const juce::String& url, const juce::String& origin)
{
    return WebSocket::fromURL (url, false, origin);
}

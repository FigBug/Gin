#ifdef _WIN32
    #define NOMINMAX
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
        #define _CRT_SECURE_NO_WARNINGS // _CRT_SECURE_NO_WARNINGS for sscanf errors in MSVC2013 Express
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <fcntl.h>
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #pragma comment( lib, "ws2_32" )
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/types.h>
    #include <io.h>
    #ifndef _SSIZE_T_DEFINED
        typedef int ssize_t;
        #define _SSIZE_T_DEFINED
    #endif
    #ifndef snprintf
        #define snprintf _snprintf_s
    #endif
    #include <stdint.h>
    #define socketerrno WSAGetLastError()
    #define SOCKET_EAGAIN_EINPROGRESS WSAEINPROGRESS
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#else
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <stdint.h>
    #ifndef INVALID_SOCKET
        #define INVALID_SOCKET (-1)
    #endif
    #ifndef SOCKET_ERROR
        #define SOCKET_ERROR   (-1)
    #endif
    #define closesocket(s) ::close(s)
    #include <errno.h>
    #define socketerrno errno
    #define SOCKET_EAGAIN_EINPROGRESS EAGAIN
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK
#endif

#include <vector>
#include <string>
#include <algorithm>

#include "easywsclient.hpp"

namespace easywsclient {

#ifdef _WIN32
/* dumb_socketpair
 * Copyright 2007 by Nathan C. Myers <ncm@cantrip.org>; some rights reserved.
 * This code is Free Software.  It may be copied freely, in original or
 * modified form, subject only to the restrictions that (1) the author is
 * relieved from all responsibilities for any use for any purpose, and (2)
 * this copyright notice must be retained, unchanged, in its entirety.  If
 * for any reason the author might be held responsible for any consequences
 * of copying or use, license is withheld.
 */
static int dumb_socketpair(SOCKET socks[2], int make_overlapped)
{
	union {
		struct sockaddr_in inaddr;
		struct sockaddr addr;
	} a;
	SOCKET listener;
	int e;
	socklen_t addrlen = sizeof(a.inaddr);
	DWORD flags = (make_overlapped ? WSA_FLAG_OVERLAPPED : 0);
	int reuse = 1;

	if (socks == 0) {
		WSASetLastError(WSAEINVAL);
		return SOCKET_ERROR;
	}

	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == INVALID_SOCKET)
		return SOCKET_ERROR;

	memset(&a, 0, sizeof(a));
	a.inaddr.sin_family = AF_INET;
	a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	a.inaddr.sin_port = 0;

	socks[0] = socks[1] = INVALID_SOCKET;
	do {
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
			(char*)& reuse, (socklen_t) sizeof(reuse)) == -1)
			break;
		if (bind(listener, &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
			break;
		if (getsockname(listener, &a.addr, &addrlen) == SOCKET_ERROR)
			break;
		if (listen(listener, 1) == SOCKET_ERROR)
			break;
		socks[0] = WSASocketW(AF_INET, SOCK_STREAM, 0, nullptr, 0, flags);
		if (socks[0] == INVALID_SOCKET)
			break;
		if (connect(socks[0], &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR)
			break;
		socks[1] = accept(listener, nullptr, nullptr);
		if (socks[1] == INVALID_SOCKET)
			break;

		closesocket(listener);
		return 0;

	} while (0);

	e = WSAGetLastError();
	closesocket(listener);
	closesocket(socks[0]);
	closesocket(socks[1]);
	WSASetLastError(e);
	return SOCKET_ERROR;
}
#endif

    
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

WebSocket::WebSocket(std::unique_ptr<gin::SecureStreamingSocket>&& socket_, bool useMask)
    : socket(std::move(socket_))
    , sockfd(socket->getRawSocketHandle())
    , readyState(OPEN)
    , interruptIn(0)
    , interruptOut(0)
    , useMask(useMask)
    , isRxBad(false)
    , isBinary(false) {
   #ifdef _WIN32
    SOCKET pipes[2] = {0, 0};
    if (!dumb_socketpair(pipes, 0))
    {
        interruptIn  = pipes[0];
        interruptOut = pipes[1];

        u_long on = 1;
        ioctlsocket(interruptIn, FIONBIO, &on);
    }
   #else
    int fd[2] = {0, 0};
    if (socketpair(PF_LOCAL, SOCK_STREAM, 0, fd) == 0)
    {
        interruptIn  = fd[0];
        interruptOut = fd[1];

        int flags = fcntl(interruptIn, F_GETFL, 0);
        if (fcntl(interruptIn, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            closesocket(interruptIn);
            closesocket(interruptOut);
            interruptIn  = 0;
            interruptOut = 0;
        }
    }
   #endif
}

WebSocket::~WebSocket()
{
    if (interruptIn)  closesocket(interruptIn);
    if (interruptOut) closesocket(interruptOut);
}

WebSocket::readyStateValues WebSocket::getReadyState() const {
  return readyState;
}

bool WebSocket::readIncoming()
{
    bool foundData = false;

    while (true) {
        // FD_ISSET(0, &rfds) will be true
        size_t N = rxbuf.size();
        ssize_t ret = 0;
        rxbuf.resize(N + 1500);

        ret = socket->read((char*)&rxbuf[0] + N, 1500, false);

        if (ret <= 0) {
            rxbuf.resize(N);
            break;
        }
        else {
            rxbuf.resize(size_t(N + ret));
            foundData = true;
        }
    }

    return foundData;
}

void WebSocket::poll(int timeout) { // timeout in milliseconds
    if (readyState == CLOSED) {
        jassertfalse;
        return;
    }
    
    if (!socket->isConnected()) {
        readyState = CLOSED;
        return;
    }

    if (txbuf.size() == 0 && ! readIncoming())
    {
        // Only block if we have no data to send and no data to recv
        if (timeout != 0) {
            fd_set rfds;
            fd_set wfds;
            timeval tv = { timeout/1000, (timeout%1000) * 1000 };
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_SET(sockfd, &rfds);

            int maxSocket = sockfd;
            if (interruptIn) {
                FD_SET(interruptIn, &rfds);
                maxSocket = std::max(maxSocket, interruptIn);
            }

            if (txbuf.size())
                FD_SET(sockfd, &wfds);

            select(maxSocket + 1, &rfds, &wfds, nullptr, &tv);
        }
    }

    while (true) {
        char dummy[128] = {0};
        ssize_t ret = recv(interruptIn, dummy, sizeof(dummy), 0);
        if (ret <= 0)
            break;
    }

    // Read incoming data
    readIncoming();
    
    // Write outgoing data
    while (txbuf.size()) {
        int ret = socket->write((char*)&txbuf[0], txbuf.size());
        if (ret < 0) {
            readyState = CLOSED;
            fputs(ret < 0 ? "Connection error!\n" : "Connection closed!\n", stderr);
            break;
        }
        else {
            txbuf.erase(txbuf.begin(), txbuf.begin() + ret);
        }
    }
    if (!txbuf.size() && readyState == CLOSING) {
        readyState = CLOSED;
    }
}

void WebSocket::interrupt()
{
    if (interruptOut)
        ::send(interruptOut, "\0", 1, 0);
}

void WebSocket::dispatch(std::function<void (const std::vector<uint8_t>& message, bool isBinary)> callback) {
    // TODO: consider acquiring a lock on rxbuf...
    if (isRxBad) {
        return;
    }
    while (true) {
        wsheader_type ws;
        if (rxbuf.size() < 2) { return; /* Need at least 2 */ }
        const uint8_t * data = (uint8_t *) &rxbuf[0]; // peek, but don't consume
        ws.fin = (data[0] & 0x80) == 0x80;
        ws.opcode = (wsheader_type::opcode_type) (data[0] & 0x0f);
        ws.mask = (data[1] & 0x80) == 0x80;
        ws.N0 = (data[1] & 0x7f);
        ws.header_size = 2 + (ws.N0 == 126? 2 : 0) + (ws.N0 == 127? 8 : 0) + (ws.mask? 4 : 0);
        if (rxbuf.size() < ws.header_size) { return; /* Need: ws.header_size - rxbuf.size() */ }
        int i = 0;
        if (ws.N0 < 126) {
            ws.N = uint64_t (ws.N0);
            i = 2;
        }
        else if (ws.N0 == 126) {
            ws.N = 0;
            ws.N |= ((uint64_t) data[2]) << 8;
            ws.N |= ((uint64_t) data[3]) << 0;
            i = 4;
        }
        else if (ws.N0 == 127) {
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
        if (ws.mask) {
            ws.masking_key[0] = ((uint8_t) data[i+0]) << 0;
            ws.masking_key[1] = ((uint8_t) data[i+1]) << 0;
            ws.masking_key[2] = ((uint8_t) data[i+2]) << 0;
            ws.masking_key[3] = ((uint8_t) data[i+3]) << 0;
        }
        else {
            ws.masking_key[0] = 0;
            ws.masking_key[1] = 0;
            ws.masking_key[2] = 0;
            ws.masking_key[3] = 0;
        }

        // Note: The checks above should hopefully ensure this addition
        //       cannot overflow:
        if (rxbuf.size() < ws.header_size+ws.N) { return; /* Need: ws.header_size+ws.N - rxbuf.size() */ }

        // We got a whole message, now do something with it:
        if (
               ws.opcode == wsheader_type::TEXT_FRAME
            || ws.opcode == wsheader_type::BINARY_FRAME
            || ws.opcode == wsheader_type::CONTINUATION
        ) {
            isBinary = (ws.opcode == wsheader_type::BINARY_FRAME);
            if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
            receivedData.insert(receivedData.end(), rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+(size_t)ws.N);// just feed
            if (ws.fin) {
                callback((const std::vector<uint8_t>) receivedData, isBinary);
                receivedData.erase(receivedData.begin(), receivedData.end());
                std::vector<uint8_t> ().swap(receivedData);// free memory
            }
        }
        else if (ws.opcode == wsheader_type::PING) {
            if (ws.mask) { for (size_t i = 0; i != ws.N; ++i) { rxbuf[i+ws.header_size] ^= ws.masking_key[i&0x3]; } }
            std::string data(rxbuf.begin()+ws.header_size, rxbuf.begin()+ws.header_size+(size_t)ws.N);
            sendData(wsheader_type::PONG, data);
        }
        else if (ws.opcode == wsheader_type::PONG) { }
        else if (ws.opcode == wsheader_type::CLOSE) { close(); }
        else { fprintf(stderr, "ERROR: Got unexpected WebSocket message.\n"); close(); }

        rxbuf.erase(rxbuf.begin(), rxbuf.begin() + ws.header_size+(size_t)ws.N);
    }
}

void WebSocket::sendPing() {
    std::string empty;
    sendData(wsheader_type::PING, empty);
}

void WebSocket::send(const std::string& message) {
    sendData(wsheader_type::TEXT_FRAME, message);
}

void WebSocket::sendBinary(const std::string& message) {
    sendData(wsheader_type::BINARY_FRAME, message);
}

void WebSocket::sendBinary(const std::vector<uint8_t>& message) {
    sendData(wsheader_type::BINARY_FRAME, message);
}

void WebSocket::sendData(wsheader_type::opcode_type type, const std::string& message)
{
    const std::vector<uint8_t> msg(message.begin(), message.end());
    sendData(type, msg);
}
    
void WebSocket::sendData(wsheader_type::opcode_type type, const std::vector<uint8_t>& message) {
    
    uint64_t message_size = message.size();
    auto message_begin = message.begin();
    auto message_end = message.end();
    
    // TODO:
    // Masking key should (must) be derived from a high quality random
    // number generator, to mitigate attacks on non-WebSocket friendly
    // middleware:
    const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };
    // TODO: consider acquiring a lock on txbuf...
    if (readyState == CLOSING || readyState == CLOSED) { return; }
    std::vector<uint8_t> header;
    header.assign(2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0), 0);
    header[0] = 0x80 | type;
    if (message_size < 126) {
        header[1] = (message_size & 0xff) | (useMask ? 0x80 : 0);
        if (useMask) {
            header[2] = masking_key[0];
            header[3] = masking_key[1];
            header[4] = masking_key[2];
            header[5] = masking_key[3];
        }
    }
    else if (message_size < 65536) {
        header[1] = 126 | (useMask ? 0x80 : 0);
        header[2] = (message_size >> 8) & 0xff;
        header[3] = (message_size >> 0) & 0xff;
        if (useMask) {
            header[4] = masking_key[0];
            header[5] = masking_key[1];
            header[6] = masking_key[2];
            header[7] = masking_key[3];
        }
    }
    else { // TODO: run coverage testing here
        header[1] = 127 | (useMask ? 0x80 : 0);
        header[2] = (message_size >> 56) & 0xff;
        header[3] = (message_size >> 48) & 0xff;
        header[4] = (message_size >> 40) & 0xff;
        header[5] = (message_size >> 32) & 0xff;
        header[6] = (message_size >> 24) & 0xff;
        header[7] = (message_size >> 16) & 0xff;
        header[8] = (message_size >>  8) & 0xff;
        header[9] = (message_size >>  0) & 0xff;
        if (useMask) {
            header[10] = masking_key[0];
            header[11] = masking_key[1];
            header[12] = masking_key[2];
            header[13] = masking_key[3];
        }
    }
    // N.B. - txbuf will keep growing until it can be transmitted over the socket:
    txbuf.insert(txbuf.end(), header.begin(), header.end());
    txbuf.insert(txbuf.end(), message_begin, message_end);
    if (useMask) {
        size_t message_offset = txbuf.size() - message_size;
        for (size_t i = 0; i != message_size; ++i) {
            txbuf[message_offset + i] ^= masking_key[i&0x3];
        }
    }
}

void WebSocket::close() {
    if(readyState == CLOSING || readyState == CLOSED) { return; }
    readyState = CLOSING;
    uint8_t closeFrame[6] = {0x88, 0x80, 0x00, 0x00, 0x00, 0x00}; // last 4 bytes are a masking key
    std::vector<uint8_t> header(closeFrame, closeFrame+6);
    txbuf.insert(txbuf.end(), header.begin(), header.end());
}

WebSocket* WebSocket::from_url(const std::string& url, bool useMask, const std::string& origin) {
    char host[512];
    int port;
    char path[512];
    if (url.size() >= 512) {
      fprintf(stderr, "ERROR: url size limit exceeded: %s\n", url.c_str());
      return nullptr;
    }
    if (origin.size() >= 200) {
      fprintf(stderr, "ERROR: origin size limit exceeded: %s\n", origin.c_str());
      return nullptr;
    }
    
    bool secure = false;
    if (sscanf(url.c_str(), "wss://%[^:/]:%d/%s", host, &port, path) == 3) {
        secure = true;
    }
    else if (sscanf(url.c_str(), "wss://%[^:/]/%s", host, path) == 2) {
        secure = true;
        port = 443;
    }
    else if (sscanf(url.c_str(), "wss://%[^:/]:%d", host, &port) == 2) {
        secure = true;
        path[0] = '\0';
    }
    else if (sscanf(url.c_str(), "wss://%[^:/]", host) == 1) {
        secure = true;
        port = 443;
        path[0] = '\0';
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d/%s", host, &port, path) == 3) {
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]/%s", host, path) == 2) {
        port = 80;
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]:%d", host, &port) == 2) {
        path[0] = '\0';
    }
    else if (sscanf(url.c_str(), "ws://%[^:/]", host) == 1) {
        port = 80;
        path[0] = '\0';
    }

    else {
        fprintf(stderr, "ERROR: Could not parse WebSocket url: %s\n", url.c_str());
        return nullptr;
    }
    //fprintf(stderr, "easywsclient: connecting: host=%s port=%d path=/%s\n", host, port, path);
    
    auto socket = std::make_unique<gin::SecureStreamingSocket>(secure);
    if (!socket->connect(host, port)) {
        fprintf(stderr, "Unable to connect to %s:%d\n", host, port);
        return nullptr;
    }
    
    int sockfd = socket->getRawSocketHandle();
    
    {
        // XXX: this should be done non-blocking,
        char line[1024] = {0};
        int status;
        int i;
        
        snprintf(line, 1024, "GET /%s HTTP/1.1\r\n", path);
        socket->write(line, strlen(line));
        
        if (port == 80) {
            snprintf(line, 1024, "Host: %s\r\n", host); socket->write(line, strlen(line));;
        }
        else {
            snprintf(line, 1024, "Host: %s:%d\r\n", host, port); socket->write(line, strlen(line));;
        }
        
        snprintf(line, 1024, "Upgrade: websocket\r\n");
        socket->write(line, strlen(line));;
        
        snprintf(line, 1024, "Connection: Upgrade\r\n");
        socket->write(line, strlen(line));;
        
        if (!origin.empty()) {
            snprintf(line, 1024, "Origin: %s\r\n", origin.c_str()); socket->write(line, strlen(line));;
        }
        
        snprintf(line, 1024, "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==\r\n");
        socket->write(line, strlen(line));
        
        snprintf(line, 1024, "Sec-WebSocket-Version: 13\r\n");
        socket->write(line, strlen(line));
        
        snprintf(line, 1024, "\r\n");
        socket->write(line, strlen(line));
        
        for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i) {
            if (socket->read(line+i, 1, true) == 0) {
                return nullptr;
            }
        }
        
        line[i] = 0;
        if (i == 1023) {
            fprintf(stderr, "ERROR: Got invalid status line connecting to: %s\n", url.c_str());
            return nullptr;
        }
        
        if (sscanf(line, "HTTP/1.1 %d", &status) != 1 || status != 101) {
            fprintf(stderr, "ERROR: Got bad status connecting to %s: %s", url.c_str(), line);
            return nullptr;
        }
        
        // TODO: verify response headers,
        while (true) {
            for (i = 0; i < 2 || (i < 1023 && line[i-2] != '\r' && line[i-1] != '\n'); ++i) {
                if (socket->read(line+i, 1, true) == 0) {
                    return nullptr;
                }
            }
            if (line[0] == '\r' && line[1] == '\n') {
                break;
            }
        }
    }
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag)); // Disable Nagle's algorithm
    
    return new WebSocket(std::move (socket), useMask);
}

WebSocket* WebSocket::from_url(const std::string& url, const std::string& origin) {
    return WebSocket::from_url(url, true, origin);
}

WebSocket* WebSocket::from_url_no_mask(const std::string& url, const std::string& origin) {
    return WebSocket::from_url(url, false, origin);
}


} // namespace easywsclient

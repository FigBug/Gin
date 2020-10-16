#pragma once

// This code comes from:
// https://github.com/dhbaird/easywsclient
//
// To get the latest version:
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.hpp
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.cpp
//
// Heavily refactored to become gin::Websocket

#include <string>
#include <vector>

struct WSHeaderType
{
	enum Opcode
	{
		CONTINUATION = 0x0,
		TEXT_FRAME = 0x1,
		BINARY_FRAME = 0x2,
		CLOSE = 8,
		PING = 9,
		PONG = 0xa,
	};

    unsigned header_size;
    bool fin;
    bool mask;
	Opcode opcode;
    int N0;
    uint64_t N;
    uint8_t masking_key[4];
};

class WebSocket
{
  public:
	enum ReadyStateValues { CLOSING, CLOSED, OPEN };
	ReadyStateValues readyStateValues;

    // Factories:
    static WebSocket* fromURL (const std::string& url, const std::string& origin = std::string());
    static WebSocket* fromURLNoMask (const std::string& url, const std::string& origin = std::string());
    static WebSocket* fromURL (const std::string& url, bool useMask, const std::string& origin);

    ~WebSocket();

    bool readIncoming();
    void poll (int timeout = 0); // timeout in milliseconds
    void interrupt(); // interrupt polling
    
    void send (const std::string& message);
    void sendBinary (const std::string& message);
    void sendBinary (const std::vector<uint8_t>& message);
    void sendPing();
    
    void close();
    ReadyStateValues getReadyState() const;

    void dispatch (std::function<void (const std::vector<uint8_t>& message, bool isBinary)> callback);
    
private:
    WebSocket (std::unique_ptr<gin::SecureStreamingSocket>&& socket_, bool useMask);

    void sendData (WSHeaderType::Opcode type, const std::string& message);
    void sendData (WSHeaderType::Opcode type, const std::vector<uint8_t>& message);

    std::vector<uint8_t> rxbuf;
    std::vector<uint8_t> txbuf;
    std::vector<uint8_t> receivedData;
    
    std::unique_ptr<gin::SecureStreamingSocket> socket;
    int sockfd = 0;
    ReadyStateValues readyState = OPEN;
    int interruptIn = 0;
    int interruptOut = 0;
    bool useMask = false;
    bool isRxBad = false;
    bool isBinary = false;
};

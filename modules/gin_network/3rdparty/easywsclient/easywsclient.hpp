#pragma once

// This code comes from:
// https://github.com/dhbaird/easywsclient
//
// To get the latest version:
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.hpp
// wget https://raw.github.com/dhbaird/easywsclient/master/easywsclient.cpp

#include <string>
#include <vector>

namespace easywsclient {
    
struct wsheader_type {
    unsigned header_size;
    bool fin;
    bool mask;
    enum opcode_type {
        CONTINUATION = 0x0,
        TEXT_FRAME = 0x1,
        BINARY_FRAME = 0x2,
        CLOSE = 8,
        PING = 9,
        PONG = 0xa,
    } opcode;
    int N0;
    uint64_t N;
    uint8_t masking_key[4];
};

class WebSocket {
  public:
    typedef enum readyStateValues { CLOSING, CLOSED, OPEN } readyStateValues;

    // Factories:
    static WebSocket* from_url(const std::string& url, const std::string& origin = std::string());
    static WebSocket* from_url_no_mask(const std::string& url, const std::string& origin = std::string());
    static WebSocket* from_url(const std::string& url, bool useMask, const std::string& origin);

    ~WebSocket();
    
    void poll(int timeout = 0); // timeout in milliseconds
    void interrupt(); // interrupt polling
    
    void send(const std::string& message);
    void sendBinary(const std::string& message);
    void sendBinary(const std::vector<uint8_t>& message);
    void sendPing();
    
    void close();
    readyStateValues getReadyState() const;

    void dispatch(std::function<void (const std::vector<uint8_t>& message, bool isBinary)> callback);
    
private:
    WebSocket(std::unique_ptr<gin::SecureStreamingSocket>&& socket_, bool useMask);
    void sendData(wsheader_type::opcode_type type, const std::string& message);
    void sendData(wsheader_type::opcode_type type, const std::vector<uint8_t>& message);

    std::vector<uint8_t> rxbuf;
    std::vector<uint8_t> txbuf;
    std::vector<uint8_t> receivedData;
    
    std::unique_ptr<gin::SecureStreamingSocket> socket;
    int sockfd;
    readyStateValues readyState;
    int interruptIn;
    int interruptOut;
    bool useMask;
    bool isRxBad;
    bool isBinary;
};

} // namespace easywsclient

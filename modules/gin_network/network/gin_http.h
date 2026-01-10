/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
    Simple HTTP client for making GET requests.

    Http provides a lightweight, synchronous HTTP client built on top of
    SecureStreamingSocket. It supports both HTTP and HTTPS protocols, handles
    chunked transfer encoding, and returns complete response data including
    status code, headers, and body.

    Key Features:
    - Synchronous GET requests (blocking)
    - HTTPS/TLS support via SecureStreamingSocket
    - Chunked transfer encoding handling
    - Complete header parsing
    - Returns status code, headers, and response body

    Usage:
    Create an Http instance with a URL, then call get() to perform the request.
    The method blocks until the response is received or an error occurs.

    @code
    Http http(juce::URL("https://api.example.com/data"));
    auto result = http.get();

    if (result.statusCode == 200)
    {
        juce::String response = result.data.toString();
        DBG("Response: " + response);
    }
    @endcode

    Note: This is a synchronous (blocking) operation. For non-blocking HTTP
    requests, consider using juce::URL::createInputStream() with a background
    thread or JUCE's built-in WebInputStream.

    @see SecureStreamingSocket, AsyncWebsocket
*/
class Http
{
public:
    /**
        Result of an HTTP request.

        Contains the complete response from an HTTP GET request including
        status code, all response headers, and the response body data.
    */
    struct HttpResult
    {
        int statusCode = 0;                     ///< HTTP status code (200, 404, etc.)
        juce::StringPairArray headers;          ///< Response headers as key-value pairs
        juce::MemoryBlock data;                 ///< Response body data
    };

    /**
        Creates an HTTP client for the specified URL.

        @param url The URL to request. Supports both http:// and https:// schemes.
    */
    Http (juce::URL url);

    /**
        Performs a synchronous HTTP GET request.

        Connects to the server, sends the GET request, and reads the complete
        response including headers and body. This method blocks until the request
        completes or fails.

        @return HttpResult containing status code, headers, and response data
    */
    HttpResult get();

private:
    bool getHeader (SecureStreamingSocket& s, HttpResult& result);
    bool isChunked (const juce::StringPairArray& headers);
    int getContentLength (const juce::StringPairArray& headers);

    bool readChunk (SecureStreamingSocket& s, juce::MemoryBlock& data);

    juce::URL url;
};

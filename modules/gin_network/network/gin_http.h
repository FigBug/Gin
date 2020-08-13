/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

class Http
{
public:
    struct HttpResult
    {
        int statusCode = 0;
        juce::StringPairArray headers;
        juce::MemoryBlock data;
    };

    Http (juce::URL url);

    HttpResult get();

private:
    bool getHeader (SecureStreamingSocket& s, HttpResult& result);
    bool isChunked (const juce::StringPairArray& headers);
    int getContentLength (const juce::StringPairArray& headers);

    bool readChunk (SecureStreamingSocket& s, juce::MemoryBlock& data);

    juce::URL url;
};

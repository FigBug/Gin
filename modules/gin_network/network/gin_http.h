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
        StringPairArray headers;
        MemoryBlock data;
    };
    
    Http (URL url);
    
    HttpResult get();
    
private:
    bool getHeader (SecureStreamingSocket& s, HttpResult& result);
    bool isChunked (const StringPairArray& headers);
    int getContentLength (const StringPairArray& headers);
    
    bool readChunk (SecureStreamingSocket& s, MemoryBlock& data);

    URL url;
};

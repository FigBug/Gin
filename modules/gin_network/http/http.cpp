/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
Http::Http (URL url_)
    : url (url_)
{
}

bool Http::isChunked (const StringPairArray& headers)
{
    if (headers["Transfer-Encoding"] == "chunked")
        return true;
    
    return false;
}

bool Http::getHeader (SecureStreamingSocket& s, HttpResult& result)
{
    String incoming;
    
    char buffer[2];
    int res = 0;
    
    while ((res = s.read (buffer, sizeof (buffer) - 1, false)) > 0)
    {
        buffer[res] = 0;
        incoming += buffer;
        
        if (incoming.endsWith ("\r\n\r\n"))
        {
            auto lines = StringArray::fromTokens (incoming, "\r\n", "");
            
            for (auto line : lines)
            {
                auto k = line.upToFirstOccurrenceOf (": ", false, false);
                auto v = line.fromFirstOccurrenceOf (": ", false, false);
                
                if (k.startsWith ("HTTP"))
                {
                    result.statusCode = StringArray::fromTokens (k, " ", "")[1].getIntValue();
                }
                else if (k.isNotEmpty() && v.isNotEmpty())
                {
                    result.headers.set (k, v);
                }
            }
            return true;
        }
    }
    return false;
}

bool Http::readChunk (SecureStreamingSocket& s, MemoryBlock& data)
{
    String line;
    char ch;

    while (! line.endsWith ("\r\n"))
    {
        s.read (&ch, 1, true);
        line += ch;
    }
    
    int sz = line.getHexValue32();
    if (sz > 0)
    {
        HeapBlock<char> buf (sz + 1);
        s.read (buf.get(), sz, true);
        buf[sz] = 0;
        
        data.append (buf.get(), sz);
    }
    
    s.read (&ch, 1, true);
    s.read (&ch, 1, true);
    
    return sz > 0;
}

Http::HttpResult Http::get()
{
    HttpResult result;
    
    bool secure = url.getScheme() == "https";
    int port = url.getPort();
    if (port == 0)
        port = secure ? 443 : 80;
    
    SecureStreamingSocket s (secure);
    
    if (s.connect (url.getDomain(), port))
    {
        String get;
        get += "GET /" + url.getSubPath() + " HTTP/1.1\r\n";
        get += "Host: " + url.getDomain() + "\r\n";
        get += "\r\n";
        
        s.write (get.toRawUTF8(), (int) get.getNumBytesAsUTF8());
        
        getHeader (s, result);
        
        if (isChunked (result.headers))
        {
            while (readChunk (s, result.data));
        }
        else
        {
            
        }
    }
    return {};
}

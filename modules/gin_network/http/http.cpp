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
            
        }
        else
        {
            
        }
    }
    return {};
}

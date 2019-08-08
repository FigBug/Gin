/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
Http::Http (URL url_)
    : url (url_)
{
}

Http::HttpResult Http::get()
{
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
        
        char buffer[1024];
        
        int res = 0;
        while ((res = s.read (buffer, sizeof (buffer) - 1, false)) > 0)
        {
            buffer[res] = 0;
            printf ("%s", buffer);
        }
    }
    return {};
}

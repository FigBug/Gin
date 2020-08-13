/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
Http::Http (juce::URL url_)
    : url (url_)
{
}

bool Http::isChunked (const juce::StringPairArray& headers)
{
    if (headers["Transfer-Encoding"] == "chunked")
        return true;

    return false;
}

int Http::getContentLength (const juce::StringPairArray& headers)
{
    return headers["Content-Length"].getIntValue();
}

bool Http::getHeader (SecureStreamingSocket& s, HttpResult& result)
{
    juce::String incoming;

    char ch;
    int res = 0;

    while ((res = s.read (&ch, 1, false)) > 0)
    {
        incoming += ch;

        if (incoming.endsWith ("\r\n\r\n"))
        {
            auto lines = juce::StringArray::fromTokens (incoming, "\r\n", "");

            for (auto line : lines)
            {
                auto k = line.upToFirstOccurrenceOf (": ", false, false);
                auto v = line.fromFirstOccurrenceOf (": ", false, false);

                if (k.startsWith ("HTTP"))
                {
                    result.statusCode = juce::StringArray::fromTokens (k, " ", "")[1].getIntValue();
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

bool Http::readChunk (SecureStreamingSocket& s, juce::MemoryBlock& data)
{
    juce::String line;
    char ch;

    while (! line.endsWith ("\r\n"))
    {
        s.read (&ch, 1, false);
        line += ch;
    }

    int sz = line.getHexValue32();
    if (sz > 0)
    {
        juce::HeapBlock<char> buf (sz);

        int todo = sz;
        while (todo > 0)
        {
            int done = s.read (buf.get(), todo, false);
            if (done > 0)
            {
                data.append (buf.get(), (size_t) done);
                todo -= done;
            }
        }
    }

    s.read (&ch, 1, false);
    s.read (&ch, 1, false);

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
        juce::String params = url.toString (true).fromFirstOccurrenceOf ("?", true, true);
        juce::String subpath = url.getSubPath();

        juce::String get;
        get += "GET /" + params + subpath + " HTTP/1.1\r\n";
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
            juce::HeapBlock<char> buf (64 * 1024);

            int todo = getContentLength (result.headers);
            while (todo > 0)
            {
                int done = s.read (buf.get(), std::min (64 * 1024, todo), false);
                if (done > 0)
                {
                    result.data.append (buf.get(), (size_t) done);
                    todo -= done;
                }
            }
        }
    }
    return result;
}

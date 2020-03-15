/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

DownloadManager::DownloadManager (int connect, int shutdown)
    : connectTimeout (connect), shutdownTimeout (shutdown)
{
}

DownloadManager::~DownloadManager()
{
    cancelAllDownloads();
}

void DownloadManager::setConcurrentDownloadLimit (int limit)
{
    maxDownloads = limit;
    triggerNextDownload();
}

void DownloadManager::triggerNextDownload()
{
    for (int i = 0; i < downloads.size() && runningDownloads < maxDownloads; i++)
    {
        auto d = downloads[i];
        if (! d->started)
        {
            runningDownloads++;
            d->started = true;
            d->startThread (priority);
        }
    }
}

DownloadManager::DownloadResult DownloadManager::blockingDownload (String url, String postData, String extraHeaders)
{
    return blockingDownload (URL (url).withPOSTData (postData), extraHeaders);
}

DownloadManager::DownloadResult DownloadManager::blockingDownload (URL url, String extraHeaders)
{
   #if JUCE_WINDOWS
    auto headerList = StringArray::fromTokens (extraHeaders, "\n", "");
    headerList.add ("Accept-Encoding: gzip");
    extraHeaders = headerList.joinIntoString ("\n");
   #endif

    Download download (*this);
    download.async = false;
    download.result.url = url;
    download.headers = extraHeaders;
    download.result.downloadId = 0;
    download.completionCallback = nullptr;
    download.progressCallback = nullptr;

    download.run();

    return download.result;
}

int DownloadManager::startAsyncDownload (String url, String postData,
                                         std::function<void (DownloadResult)> completionCallback,
                                         std::function<void (int64, int64, int64)> progressCallback,
                                         String extraHeaders)
{
    return startAsyncDownload (URL (url).withPOSTData (postData), completionCallback, progressCallback, extraHeaders);
}

int DownloadManager::startAsyncDownload (URL url,
                                         std::function<void (DownloadResult)> completionCallback,
                                         std::function<void (int64, int64, int64)> progressCallback,
                                         String extraHeaders)
{
   #if JUCE_WINDOWS
    // macOS does this automatically
    if (gzipDeflate)
    {
        auto headerList = StringArray::fromTokens (extraHeaders, "\n", "");
        headerList.add ("Accept-Encoding: gzip");
        extraHeaders = headerList.joinIntoString ("\n");
    }
   #endif

    auto download = new Download (*this);
    download->result.url = url;
    download->headers = extraHeaders;
    download->result.downloadId = ++nextId;
    download->completionCallback = completionCallback;
    download->progressCallback = progressCallback;

    downloads.add (download);

    triggerNextDownload();

    return download->result.downloadId;
}

void DownloadManager::cancelAllDownloads()
{
    runningDownloads = 0;
    downloads.clear();
}

void DownloadManager::cancelDownload (int downloadId)
{
    for (int i = downloads.size(); --i >= 0;)
    {
        if (downloads[i]->result.downloadId == downloadId)
        {
            if (downloads[i]->isThreadRunning())
                runningDownloads--;

            downloads.remove (i);
            triggerNextDownload();

            if (downloads.size() == 0 && queueFinishedCallback)
                queueFinishedCallback();

            break;
        }
    }
}

void DownloadManager::downloadFinished (Download* download)
{
    runningDownloads--;
    downloads.removeObject (download);

    triggerNextDownload();

    if (downloads.size() == 0 && queueFinishedCallback)
        queueFinishedCallback();
}

void DownloadManager::pauseDownloads (bool p)
{
    pause = p;
}

//==============================================================================
DownloadManager::Download::~Download()
{
    // Cancel any blocking reads
    if (is != nullptr)
        is->cancel();

    // Wait a long time before cancelling, WebInputStream could be stuck in
    // connect. Unlikely but possible.
    if (async)
        stopThread (owner.shutdownTimeout);
}

void DownloadManager::Download::run()
{
    int attemps = owner.retryLimit + 1;
    while (attemps-- && ! threadShouldExit())
    {
        result.attempts++;
        if (tryDownload())
            break;

        if (owner.retryDelay > 0)
            wait (roundToInt (owner.retryDelay * 1000));

        while (owner.pause.get())
            wait (500);
    }

    if (async && ! threadShouldExit())
    {
        // Get a weak reference to self, to check if we get deleted before
        // async call happens.
        WeakReference<Download> self = this;
        MessageManager::callAsync ([self]
                                   {
                                       if (self != nullptr)
                                           self->completionCallback (self->result);
                                       if (self != nullptr)
                                           self->owner.downloadFinished (self);
                                       // DownloadManager has now delete us, don't do anything else
                                   });
    }
}

bool DownloadManager::Download::tryDownload()
{
    // Use post if we have post data
    const bool post = result.url.getPostData().isNotEmpty();

    if ((is = std::make_unique<WebInputStream> (result.url, post)) != nullptr)
    {
        if (headers.isNotEmpty())
            is->withExtraHeaders (headers);
        is->withConnectionTimeout (owner.connectTimeout);

        if (is->connect (nullptr))
        {
            // Save headers and http response code
            result.httpCode         = is->getStatusCode();
            result.responseHeaders  = is->getResponseHeaders();

            auto keys = result.responseHeaders.getAllKeys();
            auto vals = result.responseHeaders.getAllValues();

            MemoryOutputStream os (result.data, false);

            lastBytesSent = 0;
            lastProgress = Time::getMillisecondCounter();
            int64 downloaded  = 0;
            int64 totalLength = is->getTotalLength();

            // For chunked http encoding, overall length may not be given
            if (totalLength < 0)
                totalLength = std::numeric_limits<int64>::max();

            // Download all data
            char buffer[128 * 1000];
            while (! is->isExhausted() && ! threadShouldExit() && downloaded < totalLength)
            {
                int64 toRead = jmin (int64 (sizeof (buffer)), int64 (owner.downloadBlockSize), totalLength - downloaded);

                int read = is->read (buffer, int (toRead));

                if (owner.pause.get())
                {
                    result.ok = false;
                    break;
                }
                else if (read > 0)
                {
                    os.write (buffer, size_t (read));
                    downloaded += read;
                    result.ok = (is->isExhausted() || downloaded == totalLength) && result.httpCode == 200;

                    updateProgress (downloaded, totalLength, false);
                }
                else if (read == 0 && is->isExhausted())
                {
                    // For chunked encoding, assume we have it all, otherwise check the length
                    if (totalLength < std::numeric_limits<int64>::max())
                        result.ok = (totalLength == downloaded) && result.httpCode == 200;
                    else
                        result.ok = result.httpCode == 200;

                    break;
                }
                else
                {
                    result.ok = false;
                    break;
                }
            }

            updateProgress (downloaded, totalLength, true);
        }
    }

   #if JUCE_WINDOWS
    // Decompress the gzip encoded data. This happens automatically on macOS
    if (result.ok && result.responseHeaders["Content-Encoding"] == "gzip")
    {
        MemoryInputStream mis (result.data, true);
        GZIPDecompressorInputStream gis (&mis, false, GZIPDecompressorInputStream::gzipFormat);

        result.data.reset();

        while (! gis.isExhausted())
        {
            char buffer[10 * 1024];
            int read = gis.read (buffer, sizeof (buffer));
            if (read > 0)
                result.data.append (buffer, size_t (read));
        }
    }
   #endif

    return result.ok;
}

void DownloadManager::Download::updateProgress (int64 current, int64 total, bool forceNotification)
{
    if (progressCallback)
    {
        // Update progress no more than once per second
        uint32 now = Time::getMillisecondCounter();
        if ((now >= lastProgress + uint32 (owner.downloadIntervalMS)) || forceNotification)
        {
            int64 delta = current - lastBytesSent;
            lastBytesSent = current;
            lastProgress = now;

            if (delta > 0)
            {
                // Get a weak reference to self, to check if we get deleted before
                // async call happens.
                WeakReference<Download> self = this;
                MessageManager::callAsync ([self, current, total, delta]
                                           {
                                               if (self != nullptr)
                                                   self->progressCallback (current, total, delta);
                                           });
            }
        }
    }
}

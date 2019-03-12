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

int DownloadManager::startAsyncDownload (String url, String postData,
                                         std::function<void (DownloadResult)> completionCallback,
                                         std::function<void (int64, int64)> progressCallback)
{
    return startAsyncDownload (URL (url).withPOSTData (postData), completionCallback, progressCallback);
}

int DownloadManager::startAsyncDownload (URL url,
                                         std::function<void (DownloadResult)> completionCallback,
                                         std::function<void (int64, int64)> progressCallback)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    
    auto* download = new Download (*this);
    download->result.url = url;
    download->result.downloadId = ++nextId;
    download->completionCallback = completionCallback;
    download->progressCallback = progressCallback;
    
    if (runningDownloads < maxDownloads)
    {
        download->started = true;
        download->startThread();
        runningDownloads++;
    }
    
    downloads.add (download);
    
    return download->result.downloadId;
}

void DownloadManager::cancelAllDownloads()
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    
    runningDownloads = 0;
    downloads.clear();
}

void DownloadManager::cancelDownload (int downloadId)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    
    for (int i = downloads.size(); --i >= 0;)
    {
        if (downloads[i]->result.downloadId == downloadId)
        {
            if (downloads[i]->isThreadRunning())
                runningDownloads--;
            
            downloads.remove (i);
            break;
        }
    }
}

void DownloadManager::downloadFinished (Download* download)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    
    runningDownloads--;
    downloads.removeObject (download);
    
    for (int i = 0; i < downloads.size() && runningDownloads < maxDownloads; i++)
    {
        auto d = downloads[i];
        if (! d->started)
        {
            runningDownloads++;
            d->started = true;
            d->startThread();
        }
    }
}

//==============================================================================
DownloadManager::Download::~Download()
{
    // Cancel any blocking reads
    if (is != nullptr)
        is->cancel();
    
    // Wait a long time before cancelling, WebInputStream could be stuck in
    // connect. Unlikely but possible.
    stopThread (owner.shutdownTimeout);
    
    masterReference.clear();
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
    }
    
    if (! threadShouldExit())
    {
        // Get a weak reference to self, to check if we get deleted before
        // async call happens.
        WeakReference<Download> self = this;
        MessageManager::callAsync ([self]
                                   {
                                       if (self != nullptr)
                                       {
                                           self->completionCallback (self->result);
                                           self->owner.downloadFinished (self);
                                           // DownloadManager has now delete us, don't do anything else
                                       }
                                   });
    }
}

bool DownloadManager::Download::tryDownload()
{
    // Use post if we have post data
    const bool post = result.url.getPostData().isNotEmpty();
    
    if ((is = new WebInputStream (result.url, post)) != nullptr)
    {
        is->withConnectionTimeout (owner.connectTimeout);
        
        if (is->connect (nullptr))
        {
            // Save headers and http response code
            result.httpCode         = is->getStatusCode();
            result.responseHeaders  = is->getResponseHeaders();
            
            MemoryOutputStream os (result.data, false);
            
            int64 downloaded  = 0;
            int64 totalLength = is->getTotalLength();
            
            // For chunked http encoding, overall length may not be given
            if (totalLength < 0)
                totalLength = std::numeric_limits<int64>::max();
            
            // Download all data
            while (! is->isExhausted() && ! threadShouldExit() && downloaded < totalLength)
            {
                char buffer[128 * 1000];
                int64 toRead = jmin (int64 (sizeof (buffer)), totalLength - downloaded);
                
                int read = is->read (buffer, int (toRead));
                if (read > 0)
                {
                    os.write (buffer, size_t (read));
                    downloaded += read;
                    result.ok = is->isExhausted() || downloaded == totalLength;
                    
                    updateProgress (downloaded, totalLength);
                }
                else
                {
                    result.ok = false;
                    break;
                }
            }
        }
    }
    
    return result.ok;
}

void DownloadManager::Download::updateProgress (int64 current, int64 total)
{
    if (progressCallback)
    {
        // Update progress no more than once per second
        uint32 now = Time::getApproximateMillisecondCounter();
        if (now >= lastProgress + 1000)
        {
            lastProgress = now;
            
            // Get a weak reference to self, to check if we get deleted before
            // async call happens.
            WeakReference<Download> self = this;
            MessageManager::callAsync ([self, current, total]
                                       {
                                           if (self != nullptr)
                                               self->progressCallback (current, total);
                                       });
        }
    }
}

/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

DownloadManager::DownloadManager()
{
}

DownloadManager::~DownloadManager()
{
    cancelAllDownloads();
}

int DownloadManager::startAsyncDownload (String url, String postData, std::function<void (DownloadResult)> callback)
{
    return startAsyncDownload (URL (url).withPOSTData (postData), callback);
}

int DownloadManager::startAsyncDownload (URL url, std::function<void (DownloadResult)> callback)
{
    auto* download = new Download (*this);
    download->result.url = url;
    download->result.downloadId = ++nextId;
    download->callback = callback;
    download->startThread();
    
    downloads.add (download);
    
    return download->result.downloadId;
}

void DownloadManager::cancelAllDownloads()
{
    downloads.clear();
}

void DownloadManager::cancelDownload (int downloadId)
{
    for (int i = downloads.size(); --i >= 0;)
        if (downloads[i]->result.downloadId == downloadId)
            downloads.remove (i);
}

//==============================================================================
DownloadManager::Download::~Download()
{
    // Cancel any blocking reads
    if (is != nullptr)
        is->cancel();
    
    // Wait a long time before cancelling, WebInputStream could be stuck in
    // connect. Unlikely but possible.
    stopThread (30 * 1000);
    
    masterReference.clear();
}

void DownloadManager::Download::run()
{
    // Use post if we have post data
    const bool post = result.url.getPostData().isNotEmpty();
    
    if ((is = new WebInputStream (result.url, post)) != nullptr)
    {
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
                }
                else
                {
                    result.ok = false;
                    break;
                }
            }
        }
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
                                           self->callback (self->result);                                        
                                           self->owner.downloads.removeObject (self);
                                           // We have now deleted ourself, don't do anything else
                                       }
                                   });
    }
}

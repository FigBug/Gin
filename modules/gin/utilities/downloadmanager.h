/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

/**
 
  Downloads files to a memory block and then calls a lambda
  on the message thread with the results. Does not block the
  message thread while establishing the HTTP connect like
  URL::downloadToFile
 
 */
class DownloadManager
{
public:
    //==============================================================================
    DownloadManager (int connectTimeout = 30 * 1000, int shutdownTimeout = 30 * 1000);
    ~DownloadManager();
    
    /** How long connecting is given before it times out */
    void setConnectTimeout (int timeout)    { connectTimeout = timeout; }
    
    //==============================================================================
    struct DownloadResult
    {
        URL url;
        int downloadId = 0;

        MemoryBlock data;

        bool ok = false;
        int httpCode = 0;
        StringPairArray responseHeaders;
    };
    
    //==============================================================================
    /** Starts a download and returns the download id which can be used to cancel the download */
    int startAsyncDownload (String url, String postData, std::function<void (DownloadResult)> callback);
    int startAsyncDownload (URL url, std::function<void (DownloadResult)> callback);
    
    /** Cancels all downloads */
    void cancelAllDownloads();
    /** Cancels a download with a given id */
    void cancelDownload (int downloadId);
    
private:
    //==============================================================================
    /** Manages a download on a background thread */
    struct Download : public Thread
    {
        Download (DownloadManager& o) : Thread ("DownloadManager::Download"), owner (o) {}
        ~Download();
        
        void run() override;
        
        DownloadResult result;
        std::function<void (DownloadResult)> callback;
        ScopedPointer<WebInputStream> is;
        
        DownloadManager& owner;
        
        WeakReference<Download>::Master masterReference;
        friend class WeakReference<Download>;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Download)
    };
    
    //==============================================================================
    int nextId = 0;
    int connectTimeout = 30 * 1000;
    int shutdownTimeout = 30 * 1000;
    OwnedArray<Download> downloads;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DownloadManager)
};

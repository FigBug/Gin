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
    
    /** If a download fails, how long many times it should retry */
    void setRetryLimit (int limit)          { retryLimit = limit;       }
    
    /** If a download fails, how long to wait until trying again */
    void setRetryDelay (double seconds)     { retryDelay = seconds;     }
    
    /** Maximum number of downloads to allow at once */
    void setConcurrentDownloadLimit (int l) { maxDownloads = l;         }
    
    /** Number of items in download queue */
    int getNumberOfDownloads()              { return downloads.size();  }
    
    //==============================================================================
    struct DownloadResult
    {
        URL url;
        int downloadId = 0;
        int attempts = 0;

        MemoryBlock data;

        bool ok = false;
        int httpCode = 0;
        StringPairArray responseHeaders;
    };
    
    //==============================================================================
    /** Starts a download and returns the download id which can be used to cancel the download
     
        progressCallback returns current amount downloaded and total amount downloaded. Note that
        for http chunk encoding total size is unknown and will be maximum int64 value.
      */
    int startAsyncDownload (String url, String postData,
                            std::function<void (DownloadResult)> completionCallback,
                            std::function<void (int64, int64)> progressCallback = nullptr);
    
    int startAsyncDownload (URL url,
                            std::function<void (DownloadResult)> completionCallback,
                            std::function<void (int64, int64)> progressCallback = nullptr);
    
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
        bool tryDownload();
        void updateProgress (int64 current, int64 total);
        
        //==============================================================================
        DownloadResult result;
        std::function<void (DownloadResult)> completionCallback;
        std::function<void (int64, int64)> progressCallback;
        
        ScopedPointer<WebInputStream> is;
        
        DownloadManager& owner;
        
        bool started = false;
        uint32 lastProgress = 0;
    
        //==============================================================================
        WeakReference<Download>::Master masterReference;
        friend class WeakReference<Download>;
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Download)
    };
    
    void downloadFinished (Download*);
    
    //==============================================================================
    int nextId = 0;
    int connectTimeout = 30 * 1000;
    int shutdownTimeout = 30 * 1000;
    int retryLimit = 0;
    double retryDelay = 0.0;
    int runningDownloads = 0, maxDownloads = 100;
    OwnedArray<Download> downloads;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DownloadManager)
};

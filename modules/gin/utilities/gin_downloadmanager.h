/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

#include "gin_util.h"

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

    /** This callback is called every time all downloads are finished */
    void setQueueFinishedCallback (std::function<void ()> callback)
    {
        queueFinishedCallback = callback;
    }

    /** How long connecting is given before it times out */
    void setConnectTimeout (int timeout)        { connectTimeout = timeout; }

    /** If a download fails, how long many times it should retry */
    void setRetryLimit (int limit)              { retryLimit = limit;       }

    /** If a download fails, how long to wait until trying again */
    void setRetryDelay (double seconds)         { retryDelay = seconds;     }

    /** Maximum number of downloads to allow at once */
    void setConcurrentDownloadLimit (int l);

    /** Number of items in download queue */
    int getNumberOfDownloads()                  { return downloads.size();  }

    /** Set download thread priority. Does not affect priority of
        already running threads */
    void setThreadPriority (juce::Thread::Priority p) { priority = p; }

    /** Sets minimum time between download progress callbacks in milliseconds */
    void setProgressInterval (int ms)           { downloadIntervalMS = std::max (1, ms); }

    /** Sets the block size of chunks to download. Progress callbacks and
        cancelling downloads can only happen between these blocks. Max size is 128 KB */
    void setDownloadBlockSize (int bs)          { downloadBlockSize = juce::jlimit (1, 128 * 1000, bs); }

    int getNumDownloadsInQueue()                { return downloads.size();  }

    /** If enabled, will request the server sends the data compressed
        This only has effect on windows. On macOS it is handled by the system libraries
        and is always on.
      */
    void enableGzipDeflate (bool e)             { gzipDeflate = e;          }

    /** Pause / resume all downloads. This actually stops any running downloads
        and then restarts them when unpaused. You will loose some downloaded data
        that will need to be redownloaded. */
    void pauseDownloads (bool);

    //==============================================================================
    struct DownloadResult
    {
        juce::URL url;
        int downloadId = 0;
        int attempts = 0;

        juce::MemoryBlock data;

        bool ok = false;
        int httpCode = 0;
        juce::StringPairArray responseHeaders;
        
        bool saveToFile (const juce::File& file, bool overwrite = true);
    };

    //==============================================================================
    /** Starts a download and returns the download id which can be used to cancel the download

        progressCallback returns current amount downloaded, total amount to download, and amount
        downloaded since last callback. Note that for http chunk encoding total size is unknown
        and will be maximum int64 value.
      */
    int startAsyncDownload (juce::String url, juce::String postData,
                            std::function<void (DownloadResult)> completionCallback,
                            std::function<void (juce::int64, juce::int64, juce::int64)> progressCallback = nullptr,
                            juce::String extraHeaders = {});

    int startAsyncDownload (juce::URL url,
                            std::function<void (DownloadResult)> completionCallback,
                            std::function<void (juce::int64, juce::int64, juce::int64)> progressCallback = nullptr,
                            juce::String extraHeaders = {});

    /** Cancels all downloads */
    void cancelAllDownloads();

    /** Cancels a download with a given id */
    void cancelDownload (int downloadId);

    //==============================================================================
    DownloadResult blockingDownload (juce::String url, juce::String postData, juce::String extraHeaders = {});

    DownloadResult blockingDownload (juce::URL url, juce::String extraHeaders = {});

private:
    //==============================================================================
    /** Manages a download on a background thread */
    struct Download : public juce::Thread
    {
        Download (DownloadManager& o) : Thread ("DownloadManager::Download"), owner (o) {}
        ~Download() override;

        void run() override;
        bool tryDownload();
        void updateProgress (juce::int64 current, juce::int64 total, bool );

        //==============================================================================
        DownloadResult result;
        std::function<void (DownloadResult)> completionCallback;
        std::function<void (juce::int64, juce::int64, juce::int64)> progressCallback;

        std::unique_ptr<juce::WebInputStream> is;

        DownloadManager& owner;

        juce::String headers;
        bool started = false, async = true;
        juce::uint32 lastProgress = 0;
        juce::int64 lastBytesSent = 0;

        //==============================================================================
        JUCE_DECLARE_WEAK_REFERENCEABLE (Download)
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Download)
    };

    void downloadFinished (Download*);
    void triggerNextDownload();

    //==============================================================================
    int nextId = 0;
    int connectTimeout = 30 * 1000;
    int shutdownTimeout = 30 * 1000;
    int retryLimit = 0, downloadIntervalMS = 1000, downloadBlockSize = 128 * 1000;
    juce::Thread::Priority priority = juce::Thread::Priority::normal;

    double retryDelay = 0.0;
    int runningDownloads = 0, maxDownloads = 100;
    juce::OwnedArray<Download, juce::CriticalSection> downloads;
    std::function<void ()> queueFinishedCallback;
    bool gzipDeflate = true;
    std::atomic<bool> pause { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DownloadManager)
};

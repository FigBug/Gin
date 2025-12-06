/*==============================================================================

Copyright 2018 - 2025 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

#include "gin_util.h"

/**
    Asynchronous HTTP download manager with queue and progress tracking.

    DownloadManager provides a robust system for downloading files without blocking
    the message thread. Unlike juce::URL::downloadToFile which blocks during HTTP
    connection establishment, DownloadManager performs all network operations on
    background threads and delivers results via callbacks on the message thread.

    Key Features:
    - Non-blocking HTTP downloads
    - Concurrent download queue with limit control
    - Automatic retry on failure with configurable delays
    - Progress tracking with adjustable callback intervals
    - Download pause/resume support
    - GZIP compression support
    - Thread priority control
    - Blocking and async download modes
    - Safe cancellation of in-progress downloads

    The manager maintains a queue of downloads, executes them on background threads,
    and calls completion callbacks on the message thread. This ensures UI remains
    responsive during large downloads or slow connections.

    Usage:
    @code
    DownloadManager manager;
    manager.setConnectTimeout(10000);  // 10 second timeout
    manager.setRetryLimit(3);          // Retry up to 3 times
    manager.setConcurrentDownloadLimit(2);  // Max 2 at once

    // Start async download with progress
    int downloadId = manager.startAsyncDownload(
        "https://example.com/file.zip",
        "",  // No POST data
        [](DownloadManager::DownloadResult result) {
            if (result.ok)
                result.saveToFile(File("downloaded.zip"));
            else
                DBG("Download failed: " + String(result.httpCode));
        },
        [](int64 current, int64 total, int64 delta) {
            DBG("Progress: " + String(current) + " / " + String(total));
        }
    );

    // Cancel if needed
    manager.cancelDownload(downloadId);

    // Or block until complete
    auto result = manager.blockingDownload("https://example.com/data.json");
    if (result.ok)
        processJSON(result.data.toString());
    @endcode

    Thread Safety:
    - All public methods are thread-safe
    - Callbacks are always executed on the message thread
    - Safe to cancel downloads from any thread

    @see DownloadResult, juce::URL
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
    /**
        Result of a completed or failed download.

        DownloadResult contains all information about a download attempt including
        the downloaded data, HTTP status, response headers, and success/failure state.

        Members:
        - url: The URL that was downloaded
        - downloadId: Unique ID assigned by DownloadManager
        - attempts: Number of attempts made (including retries)
        - data: Downloaded content as MemoryBlock
        - ok: true if download succeeded, false otherwise
        - httpCode: HTTP response code (200 = success, 404 = not found, etc.)
        - responseHeaders: HTTP response headers from server

        The result is passed to completion callbacks on the message thread.

        @see DownloadManager
    */
    struct DownloadResult
    {
        juce::URL url;                          ///< URL that was downloaded
        int downloadId = 0;                     ///< Unique download ID
        int attempts = 0;                       ///< Number of attempts made

        juce::MemoryBlock data;                 ///< Downloaded data

        bool ok = false;                        ///< true if download succeeded
        int httpCode = 0;                       ///< HTTP response code
        juce::StringPairArray responseHeaders;  ///< HTTP response headers

        /** Saves downloaded data to a file. Returns true if successful. */
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
    /**
        Internal class managing a single download on a background thread.

        Download handles the actual HTTP request, progress tracking, retry logic,
        and callback execution for a single download task. Each download runs on
        its own thread managed by the DownloadManager.

        @see DownloadManager, DownloadResult
    */
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

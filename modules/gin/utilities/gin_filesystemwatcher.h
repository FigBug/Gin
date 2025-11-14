/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX

/**
    Cross-platform file system watcher for monitoring folder changes.

    FileSystemWatcher provides real-time notifications when files are created,
    modified, deleted, or renamed in watched folders. It uses platform-specific
    APIs (FSEvents on macOS, ReadDirectoryChangesW on Windows, inotify on Linux)
    for efficient monitoring without polling.

    Key Features:
    - Real-time file system change notifications
    - Recursive subfolder watching (macOS and Windows)
    - Event coalescing to reduce callback frequency
    - Multiple folder watching
    - Create, modify, delete, and rename event tracking
    - Listener pattern for callbacks
    - Cross-platform (macOS, Windows, Linux)

    Platform Differences:
    - macOS/Windows: Recursively watches all subfolders automatically
    - Linux: Only watches the specified folder (not subfolders)

    Event Coalescing:
    The watcher can coalesce multiple events for the same file within a time
    window into a single event, reducing callback overhead when files change
    rapidly (e.g., during a build process or file copy).

    Usage:
    @code
    class MyFileMonitor : public FileSystemWatcher::Listener
    {
    public:
        void folderChanged(const File& folder) override
        {
            // Refresh file browser for this folder
            refreshBrowser(folder);
        }

        void fileChanged(const File& file, FileSystemEvent event) override
        {
            if (event == fileUpdated && file.hasFileExtension(".txt"))
                reloadTextFile(file);
        }
    };

    FileSystemWatcher watcher;
    watcher.addFolder(File("/path/to/watch"));
    watcher.coalesceEvents(100);  // Coalesce events within 100ms

    MyFileMonitor monitor;
    watcher.addListener(&monitor);

    // Watcher runs in background, calling listener on message thread
    @endcode

    Thread Safety:
    - addFolder/removeFolder are thread-safe
    - Listener callbacks are called on the message thread
    - Safe to add/remove listeners from any thread

    @see Listener, FileSystemEvent
*/
class FileSystemWatcher
{
public:
    //==============================================================================
    FileSystemWatcher();
    ~FileSystemWatcher();

    //==============================================================================
    /** All events that arrive withing the time window for a particular file will be
        coalesced into one event with the type of the most recent event */
    void coalesceEvents (int windowMS);

    /** Adds a folder to be watched */
    void addFolder (const juce::File& folder);

    /** Removes a folder from being watched */
    void removeFolder (const juce::File& folder);

    /** Removes all folders from being watched */
    void removeAllFolders();

    /** Gets a list of folders being watched */
    juce::Array<juce::File> getWatchedFolders ();

    /**
        File system event types for change notifications.

        FileSystemEvent describes what happened to a file in a watched folder.
        Rename events generate two separate callbacks: one with fileRenamedOldName
        for the original filename, and one with fileRenamedNewName for the new filename.

        Events:
        - undefined: Unknown or unrecognized event
        - fileCreated: New file was created
        - fileDeleted: Existing file was deleted
        - fileUpdated: Existing file was modified
        - fileRenamedOldName: File was renamed (this is the old name)
        - fileRenamedNewName: File was renamed (this is the new name)

        Note: Rename operations typically generate two events:
        1. fileRenamedOldName with the original filename
        2. fileRenamedNewName with the new filename

        @see FileSystemWatcher, Listener
    */
    enum FileSystemEvent
    {
        undefined,           ///< Unknown event
        fileCreated,         ///< File was created
        fileDeleted,         ///< File was deleted
        fileUpdated,         ///< File was modified
        fileRenamedOldName,  ///< File renamed (old name)
        fileRenamedNewName   ///< File renamed (new name)
    };

    //==============================================================================
    /**
        Listener interface for receiving file system change notifications.

        Listener provides two callback methods for responding to file system events:
        - folderChanged(): Called once when any file in a folder changes
        - fileChanged(): Called for each specific file change with event type

        Override the methods you need and add the listener to FileSystemWatcher
        to receive notifications on the message thread.

        Usage Example:
        @code
        class MyListener : public FileSystemWatcher::Listener
        {
            void folderChanged(const File& folder) override
            {
                // Refresh UI for any changes in this folder
                DBG("Folder changed: " + folder.getFullPathName());
            }

            void fileChanged(const File& file, FileSystemEvent event) override
            {
                // Handle specific file changes
                if (event == fileUpdated)
                    reloadFile(file);
                else if (event == fileDeleted)
                    removeFromCache(file);
            }
        };
        @endcode

        @see FileSystemWatcher, FileSystemEvent
    */
    class Listener
    {
    public:
        virtual ~Listener() = default;

        /**
            Called when any file in a watched folder changes.

            This is called once per folder when any change occurs, useful for
            refreshing file browsers or folder displays without needing to know
            which specific files changed.

            @param folder The watched folder that contains changed files
        */
        virtual void folderChanged (const juce::File&) {}

        /**
            Called for each file that changed and how it changed.

            This provides detailed information about each file change, including
            the specific event type. Use this when you need to react to specific
            changes like file updates for auto-reloading.

            @param file The specific file that changed
            @param event The type of change that occurred
        */
        virtual void fileChanged (const juce::File&, FileSystemEvent) {}
    };

    /** Registers a listener to be told when things happen to the text.
     @see removeListener
     */
    void addListener (Listener* newListener);

    /** Deregisters a listener.
     @see addListener
     */
    void removeListener (Listener* listener);

private:
    class Impl;

    void folderChanged (const juce::File& folder);
    void fileChanged (const juce::File& file, FileSystemEvent fsEvent);

    int coalesceWindowMS = 0;

    /**
        Internal timer for event coalescing.

        CoalesceTimer delays event callbacks to allow multiple rapid events for
        the same file to be combined into a single callback, reducing overhead
        when files change frequently.

        @see coalesceEvents()
    */
    struct CoalesceTimer : public juce::Timer
    {
        CoalesceTimer (FileSystemWatcher& o, juce::File f_)
            : owner (o), f (f_), folder (true)
        {
        }

        CoalesceTimer (FileSystemWatcher& o, juce::File f_, FileSystemEvent e_)
            : owner (o), f (f_), folder (false), fsEvent (e_)
        {
        }

        void timerCallback() override
        {
            stopTimer();

            if (folder)
                owner.listeners.call (&FileSystemWatcher::Listener::folderChanged, f);
            else
                owner.listeners.call (&FileSystemWatcher::Listener::fileChanged, f, fsEvent);

            owner.timers.erase (f);
        }

        FileSystemWatcher& owner;

        juce::File f;
        bool folder;
        FileSystemEvent fsEvent;
    };

    std::map<juce::File, std::unique_ptr<CoalesceTimer>> timers;

    juce::ListenerList<Listener> listeners;

    juce::OwnedArray<Impl> watched;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileSystemWatcher)
    JUCE_DECLARE_WEAK_REFERENCEABLE (FileSystemWatcher)
};

#endif

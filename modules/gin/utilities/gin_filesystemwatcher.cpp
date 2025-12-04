/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

//==============================================================================
#if JUCE_MAC
class FileSystemWatcher::Impl
{
public:
    Impl (FileSystemWatcher& o, juce::File f) : owner (o), folder (f)
    {
        NSString* newPath = [NSString stringWithUTF8String:folder.getFullPathName().toRawUTF8()];

        paths = [[NSArray arrayWithObject:newPath] retain];
        context.version         = 0L;
        context.info            = this;
        context.retain          = nil;
        context.release         = nil;
        context.copyDescription = nil;

        dispatch_queue_t queue = dispatch_queue_create("com.gin.filesystemwatcher", DISPATCH_QUEUE_SERIAL);
        stream = FSEventStreamCreate (kCFAllocatorDefault, callback, &context, (CFArrayRef)paths, kFSEventStreamEventIdSinceNow, 0.05,
                                      kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents);
        if (stream)
        {
            FSEventStreamSetDispatchQueue(stream, queue);
            FSEventStreamStart (stream);
        }

    }

    ~Impl()
    {
        if (stream)
        {
            FSEventStreamStop (stream);
            FSEventStreamSetDispatchQueue(stream, nullptr);
            FSEventStreamInvalidate (stream);
            FSEventStreamRelease (stream);
        }
    }

    static void callback (ConstFSEventStreamRef streamRef, void* clientCallBackInfo, size_t numEvents, void* eventPaths,
                          const FSEventStreamEventFlags* eventFlags, const FSEventStreamEventId* eventIds)
    {
        juce::ignoreUnused (streamRef, numEvents, eventIds, eventPaths, eventFlags);

        Impl* impl = (Impl*)clientCallBackInfo;


        auto safeOwner = juce::WeakReference<FileSystemWatcher> (&impl->owner);

        juce::MessageManager::callAsync ([safeOwner, f = impl->folder]
        {
            if (safeOwner)
                safeOwner->folderChanged (f);
        });

        char** files = (char**)eventPaths;

        for (int i = 0; i < int (numEvents); i++)
        {
            char* file = files[i];

            FSEventStreamEventFlags evt = eventFlags[i];

            juce::File path = juce::String::fromUTF8 (file);
            auto event = FileSystemEvent::undefined;

            if (evt & kFSEventStreamEventFlagItemModified)
                event = FileSystemEvent::fileUpdated;
            else if (evt & kFSEventStreamEventFlagItemRemoved)
                event = FileSystemEvent::fileDeleted;
            else if (evt & kFSEventStreamEventFlagItemRenamed)
                event = path.exists() ? FileSystemEvent::fileRenamedNewName : FileSystemEvent::fileRenamedOldName;
            else if (evt & kFSEventStreamEventFlagItemCreated)
                event = FileSystemEvent::fileCreated;

            if (event != FileSystemEvent::undefined)
            {
                juce::MessageManager::callAsync ([safeOwner, path, event]
                {
                    if (safeOwner)
                        safeOwner->fileChanged (path, event);
                });
            }
        }
    }

    FileSystemWatcher& owner;
    const juce::File folder;

    NSArray* paths;
    FSEventStreamRef stream;
    struct FSEventStreamContext context;
};
#endif

//==============================================================================
#ifdef JUCE_LINUX
#include <poll.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

class FileSystemWatcher::Impl final : public juce::Thread,
                                      private juce::AsyncUpdater
{
public:
    struct Event
    {
        Event () = default;
        Event (const juce::File& f, const FileSystemEvent e) : file(f), fsEvent(e) {}
        Event (const Event& other) = default;
        Event (Event&& other) = default;

        juce::File file;
        FileSystemEvent fsEvent = undefined;

        Event& operator= (const Event& other)
        {
            file = other.file;
            fsEvent = other.fsEvent;
            return *this;
        }
        
        bool operator== (const Event& other) const
        {
            return file == other.file && fsEvent == other.fsEvent;
        }
    };

    Impl (FileSystemWatcher& o, juce::File f)
      : juce::Thread ("FileSystemWatcher::Impl"), owner (o), folder (std::move(f))
    {
        fd = inotify_init();

        wd = inotify_add_watch (fd,
                                folder.getFullPathName().toRawUTF8(),
                                IN_ATTRIB | IN_CREATE | IN_DELETE | IN_DELETE_SELF |
                                IN_MODIFY | IN_MOVE_SELF | IN_MOVED_TO | IN_MOVED_FROM);

        startThread (juce::Thread::Priority::background);
    }

    ~Impl() override
    {
        signalThreadShouldExit();

        // Cancel any pending async updates before shutting down
        cancelPendingUpdate();

        // Wait for thread to exit (poll() timeout will allow it to check threadShouldExit)
        stopThread (1000);

        // Clean up inotify resources
        inotify_rm_watch (fd, wd);
        close (fd);
    }

    void run() override
    {
        try
        {
            const inotify_event* iNotifyEvent;

            struct pollfd pfd;
            pfd.fd = fd;
            pfd.events = POLLIN;

            while (! threadShouldExit())
            {
                // Poll with timeout to allow checking threadShouldExit periodically
                const int pollResult = poll (&pfd, 1, 100); // 100ms timeout

                if (pollResult < 0)
                {
                    // Error occurred
                    if (errno != EINTR) // Ignore interrupts
                        break;
                    continue;
                }

                if (pollResult == 0)
                {
                    // Timeout - no events available, loop back to check threadShouldExit
                    continue;
                }

                // Data is available to read
                if (pfd.revents & POLLIN)
                {
                    char buf[BUF_LEN];
                    const ssize_t numRead = read (fd, buf, BUF_LEN);

                    if (numRead <= 0)
                        break;

                    for (const char* ptr = buf; ptr < buf + numRead; ptr += sizeof(struct inotify_event) + iNotifyEvent->len)
                    {
                        iNotifyEvent = reinterpret_cast<const inotify_event*>(ptr);

                        FileSystemEvent eventType = undefined;
                        if (iNotifyEvent->mask & IN_CREATE)      eventType = FileSystemEvent::fileCreated;
                        else if (iNotifyEvent->mask & IN_CLOSE_WRITE) eventType = FileSystemEvent::fileUpdated;
                        else if (iNotifyEvent->mask & IN_MODIFY)      eventType = FileSystemEvent::fileUpdated;
                        else if (iNotifyEvent->mask & IN_MOVED_FROM)  eventType = FileSystemEvent::fileRenamedOldName;
                        else if (iNotifyEvent->mask & IN_MOVED_TO)    eventType = FileSystemEvent::fileRenamedNewName;
                        else if (iNotifyEvent->mask & IN_DELETE)      eventType = FileSystemEvent::fileDeleted;

                        if (eventType == FileSystemEvent::undefined)
                            continue;

                        Event e (folder.getFullPathName() + '/' + iNotifyEvent->name, eventType);
                        events.write (e);
                    }

                    if (events.getNumReady() > 0)
                        triggerAsyncUpdate();
                }

                // Check for errors or hangup
                if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
                    break;
            }
        }
        catch (...)
        {

        }
    }

    void handleAsyncUpdate() override
    {
        owner.folderChanged (folder);

        while (auto e = events.read())
            owner.fileChanged (e->file, e->fsEvent);
    }

    FileSystemWatcher& owner;
    juce::File folder;

    LockFreeQueue<Event> events { 100 };

    int fd;
    int wd;
};
#endif

//==============================================================================
#ifdef JUCE_WINDOWS
class FileSystemWatcher::Impl : private juce::Thread
{
public:
    Impl (FileSystemWatcher& o, juce::File f)
      : Thread ("FileSystemWatcher::Impl"), owner (o), folder (std::move(f))
    {
        WCHAR path[_MAX_PATH] = {0};
        wcsncpy_s (path, folder.getFullPathName().toWideCharPointer(), _MAX_PATH - 1);

        folderHandle = CreateFileW (path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);


        if (folderHandle != INVALID_HANDLE_VALUE)
            startThread (juce::Thread::Priority::background);
    }

    ~Impl() override
    {
        if (isThreadRunning())
        {
            signalThreadShouldExit();

            CancelIoEx (folderHandle, nullptr);

            stopThread (1000);
        }

        CloseHandle (folderHandle);
    }

    void run() override
    {
        constexpr int heapSize = 16 * 1024;

        DWORD bytesOut = 0;

        while (! threadShouldExit())
        {
            uint8_t buffer[heapSize] = {};
            const BOOL success = ReadDirectoryChangesW (folderHandle, buffer, heapSize, true,
                                                        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                                                        &bytesOut, nullptr, nullptr);

            if (success && bytesOut > 0)
            {
                uint8_t* rawData = buffer;
                while (true)
                {
                    const FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(rawData);

                    auto eventType = FileSystemEvent::undefined;
                    switch (fni->Action)
                    {
                        case FILE_ACTION_ADDED:
                            eventType = fileCreated;
                            break;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            eventType = fileRenamedNewName;
                            break;
                        case FILE_ACTION_MODIFIED:
                            eventType = fileUpdated;
                            break;
                        case FILE_ACTION_REMOVED:
                            eventType = fileDeleted;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            eventType = fileRenamedOldName;
                            break;
                        default:
                            break;
                    }

                    auto safeOwner = juce::WeakReference<FileSystemWatcher> (&owner);
                    auto path = folder.getChildFile (juce::String (fni->FileName, fni->FileNameLength / sizeof(wchar_t)));

                    if (eventType != FileSystemEvent::undefined)
                    {
                        juce::MessageManager::callAsync ([safeOwner, path, eventType, f = folder]
                        {
                            if (safeOwner)
                            {
                                safeOwner->folderChanged (f);
                                safeOwner->fileChanged (path, eventType);
                            }
                        });
                    }

                    if (fni->NextEntryOffset > 0)
                        rawData += fni->NextEntryOffset;
                    else
                        break;
                }
            }
        }
    }

    FileSystemWatcher& owner;
    const juce::File folder;

    HANDLE folderHandle;
};
#endif

#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
FileSystemWatcher::FileSystemWatcher()
{
}

FileSystemWatcher::~FileSystemWatcher()
{
    watched.clear();
}

void FileSystemWatcher::coalesceEvents (int windowMS)
{
    coalesceWindowMS = windowMS;
}

void FileSystemWatcher::addFolder (const juce::File& folder)
{
    // You can only listen to folders that exist
    jassert (folder.isDirectory());

#if JUCE_LINUX
    if ( ! getWatchedFolders().contains (folder))
        watched.add (new Impl (*this, folder));
#else
    for (auto parent : getWatchedFolders())
        if (folder == parent || folder.isAChildOf (parent))
            return;

    watched.add (new Impl (*this, folder));
#endif
}

void FileSystemWatcher::removeFolder (const juce::File& folder)
{
    for (int i = watched.size(); --i >= 0;)
    {
        if (watched[i]->folder == folder)
        {
            watched.remove (i);
            break;
        }
    }
}

void FileSystemWatcher::removeAllFolders()
{
    watched.clear();
}

void FileSystemWatcher::addListener (Listener* newListener)
{
    listeners.add (newListener);
}

void FileSystemWatcher::removeListener (Listener* listener)
{
    listeners.remove (listener);
}

void FileSystemWatcher::folderChanged (const juce::File& folder)
{
    if (coalesceWindowMS > 0)
    {
        if (! timers.contains (folder))
        {
            auto t = std::make_unique<CoalesceTimer> (*this, folder);
            t->startTimer (coalesceWindowMS);
            timers[folder] = std::move (t);
        }
    }
    else
    {
        listeners.call (&FileSystemWatcher::Listener::folderChanged, folder);
    }
}

void FileSystemWatcher::fileChanged (const juce::File& file, FileSystemEvent fsEvent)
{
    if (coalesceWindowMS > 0)
    {
        auto itr = timers.find (file);
        if (itr == timers.end())
        {
            auto t = std::make_unique<CoalesceTimer> (*this, file, fsEvent);
            t->startTimer (coalesceWindowMS);
            timers[file] = std::move (t);
        }
        else
        {
            itr->second->fsEvent = fsEvent;
        }
    }
    else
    {
        listeners.call (&FileSystemWatcher::Listener::fileChanged, file, fsEvent);
    }
}

juce::Array<juce::File> FileSystemWatcher::getWatchedFolders()
{
    juce::Array<juce::File> res;

    for (const auto* w : watched)
        res.add (w->folder);

    return res;
}

#endif

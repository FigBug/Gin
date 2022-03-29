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

        stream = FSEventStreamCreate (kCFAllocatorDefault, callback, &context, (CFArrayRef)paths, kFSEventStreamEventIdSinceNow, 0.05,
                                      kFSEventStreamCreateFlagNoDefer | kFSEventStreamCreateFlagFileEvents);
        if (stream)
        {
            FSEventStreamScheduleWithRunLoop (stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            FSEventStreamStart (stream);
        }

    }

    ~Impl()
    {
        if (stream)
        {
            FSEventStreamStop (stream);
            FSEventStreamUnscheduleFromRunLoop (stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
            FSEventStreamInvalidate (stream);
            FSEventStreamRelease (stream);
        }
    }

    static void callback (ConstFSEventStreamRef streamRef, void* clientCallBackInfo, size_t numEvents, void* eventPaths,
                          const FSEventStreamEventFlags* eventFlags, const FSEventStreamEventId* eventIds)
    {
        juce::ignoreUnused (streamRef, numEvents, eventIds, eventPaths, eventFlags);

        Impl* impl = (Impl*)clientCallBackInfo;
        impl->owner.folderChanged (impl->folder);

        char** files = (char**)eventPaths;

        for (int i = 0; i < int (numEvents); i++)
        {
            char* file = files[i];
            FSEventStreamEventFlags evt = eventFlags[i];

            juce::File path = juce::String::fromUTF8 (file);
            if (evt & kFSEventStreamEventFlagItemModified)
                impl->owner.fileChanged (path, FileSystemEvent::fileUpdated);
            else if (evt & kFSEventStreamEventFlagItemRemoved)
                impl->owner.fileChanged (path, FileSystemEvent::fileDeleted);
            else if (evt & kFSEventStreamEventFlagItemRenamed)
                impl->owner.fileChanged (path, path.exists() ? FileSystemEvent::fileRenamedNewName : FileSystemEvent::fileRenamedOldName);
            else if (evt & kFSEventStreamEventFlagItemCreated)
                impl->owner.fileChanged (path, FileSystemEvent::fileCreated);
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
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

class FileSystemWatcher::Impl : public juce::Thread,
                                private juce::AsyncUpdater
{
public:
    struct Event
    {
        Event () {}
        Event (Event& other) = default;
        Event (Event&& other) = default;

        juce::File file;
        FileSystemEvent fsEvent;

        bool operator== (const Event& other) const
        {
            return file == other.file && fsEvent == other.fsEvent;
        }
    };

    Impl (FileSystemWatcher& o, juce::File f)
      : juce::Thread ("FileSystemWatcher::Impl"), owner (o), folder (f)
    {
        fd = inotify_init();

        wd = inotify_add_watch (fd,
                                folder.getFullPathName().toRawUTF8(),
                                IN_ATTRIB | IN_CREATE | IN_DELETE | IN_DELETE_SELF |
                                IN_MODIFY | IN_MOVE_SELF | IN_MOVED_TO | IN_MOVED_FROM);

        startThread (3);
    }

    ~Impl()
    {
        signalThreadShouldExit();
        inotify_rm_watch (fd, wd);
        close (fd);

        waitForThreadToExit (1000);
    }

    void run() override
    {
        char buf[BUF_LEN];
        ssize_t numRead;

        const struct inotify_event* iNotifyEvent;
        char* ptr;

        while (true)
        {
            int numRead = read (fd, buf, BUF_LEN);

            if (numRead <= 0 || threadShouldExit())
                break;

            for (ptr = buf; ptr < buf + numRead; ptr += sizeof(struct inotify_event) + iNotifyEvent->len)
            {
                iNotifyEvent = (const struct inotify_event*)ptr;
                Event e;

                e.file = juce::File {folder.getFullPathName() + '/' + iNotifyEvent->name};

                     if (iNotifyEvent->mask & IN_CREATE)      e.fsEvent = FileSystemEvent::fileCreated;
                else if (iNotifyEvent->mask & IN_CLOSE_WRITE) e.fsEvent = FileSystemEvent::fileUpdated;
                else if (iNotifyEvent->mask & IN_MOVED_FROM)  e.fsEvent = FileSystemEvent::fileRenamedOldName;
                else if (iNotifyEvent->mask & IN_MOVED_TO)    e.fsEvent = FileSystemEvent::fileRenamedNewName;
                else if (iNotifyEvent->mask & IN_DELETE)      e.fsEvent = FileSystemEvent::fileDeleted;


                bool duplicateEvent = false;
                for (auto existing : events)
                {
                    if (e == existing)
                    {
                        duplicateEvent = true;
                        break;
                    }
                }

                if (! duplicateEvent)
                    events.add (std::move (e));
            }

            if (events.size() > 0)
                triggerAsyncUpdate();
        }
    }

    void handleAsyncUpdate() override
    {
        juce::ScopedLock sl (lock);

        owner.folderChanged (folder);

        for (auto& e : events)
            owner.fileChanged (e.file, e.fsEvent);

        events.clear();
    }

    FileSystemWatcher& owner;
    juce::File folder;

    juce::CriticalSection lock;
    juce::Array<Event> events;

    int fd;
    int wd;
};
#endif

//==============================================================================
#ifdef JUCE_WINDOWS
class FileSystemWatcher::Impl : private juce::AsyncUpdater,
                                private juce::Thread
{
public:
    struct Event
    {
        juce::File file;
        FileSystemEvent fsEvent;

        bool operator== (const Event& other) const
        {
            return file == other.file && fsEvent == other.fsEvent;
        }
    };

    Impl (FileSystemWatcher& o, juce::File f)
      : Thread ("FileSystemWatcher::Impl"), owner (o), folder (f)
    {
        WCHAR path[_MAX_PATH] = {0};
        wcsncpy (path, folder.getFullPathName().toWideCharPointer(), _MAX_PATH - 1);

        folderHandle = CreateFileW (path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);


        if (folderHandle != INVALID_HANDLE_VALUE)
            startThread (3);
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
        const int heapSize = 16 * 1024;
        uint8_t buffer[heapSize];

        DWORD bytesOut = 0;

        while (! threadShouldExit())
        {
            memset (buffer, 0, heapSize);
            BOOL success = ReadDirectoryChangesW (folderHandle, buffer, heapSize, true,
                FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &bytesOut, nullptr, nullptr);

            if (success && bytesOut > 0)
            {
                juce::ScopedLock sl (lock);

                uint8_t* rawData = buffer;
                while (true)
                {
                    FILE_NOTIFY_INFORMATION* fni = (FILE_NOTIFY_INFORMATION*)rawData;

                    Event e;
                    e.file = folder.getChildFile (juce::String (fni->FileName, fni->FileNameLength / sizeof(wchar_t)));

                    switch (fni->Action)
                    {
                        case FILE_ACTION_ADDED:
                            e.fsEvent = fileCreated;
                            break;
                        case FILE_ACTION_RENAMED_NEW_NAME:
                            e.fsEvent = fileRenamedNewName;
                            break;
                        case FILE_ACTION_MODIFIED:
                            e.fsEvent = fileUpdated;
                            break;
                        case FILE_ACTION_REMOVED:
                            e.fsEvent = fileDeleted;
                            break;
                        case FILE_ACTION_RENAMED_OLD_NAME:
                            e.fsEvent = fileRenamedOldName;
                            break;
                    }

                    bool duplicateEvent = false;
                    for (auto existing : events)
                    {
                        if (e == existing)
                        {
                            duplicateEvent = true;
                            break;
                        }
                    }

                    if (! duplicateEvent)
                        events.add (e);

                    if (fni->NextEntryOffset > 0)
                        rawData += fni->NextEntryOffset;
                    else
                        break;
                }

                if (events.size() > 0)
                    triggerAsyncUpdate();
            }
        }
    }

    void handleAsyncUpdate() override
    {
        juce::ScopedLock sl (lock);

        owner.folderChanged (folder);

        for (auto e : events)
            owner.fileChanged (e.file, e.fsEvent);

        events.clear();
    }

    FileSystemWatcher& owner;
    const juce::File folder;

    juce::CriticalSection lock;
    juce::Array<Event> events;

    HANDLE folderHandle;
};
#endif

#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
FileSystemWatcher::FileSystemWatcher()
{
}

FileSystemWatcher::~FileSystemWatcher()
{
}

void FileSystemWatcher::addFolder (const juce::File& folder)
{
    // You can only listen to folders that exist
    jassert (folder.isDirectory());

    if ( ! getWatchedFolders().contains (folder))
        watched.add (new Impl (*this, folder));
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
    listeners.call (&FileSystemWatcher::Listener::folderChanged, folder);
}

void FileSystemWatcher::fileChanged (const juce::File& file, FileSystemEvent fsEvent)
{
    listeners.call (&FileSystemWatcher::Listener::fileChanged, file, fsEvent);
}

juce::Array<juce::File> FileSystemWatcher::getWatchedFolders()
{
    juce::Array<juce::File> res;

    for (auto w : watched)
        res.add (w->folder);

    return res;
}

#endif

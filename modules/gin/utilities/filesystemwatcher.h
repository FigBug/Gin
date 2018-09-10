/*==============================================================================

Copyright 2018 by Roland Rabien
For more information visit www.rabiensoftware.com

==============================================================================*/

#pragma once

/** Watches a folder in the file system for changes */
class FileSystemWatcher : public ChangeBroadcaster
{
public:
    FileSystemWatcher();
    ~FileSystemWatcher();

    void addFolder (const File& folder);
    void removeFolder (const File& folder);

    enum FileSystemEvent
    {
        fileCreated,
        fileDeleted,
        fileUpdated
    };

    class Listener
    {
    public:
        virtual ~Listener()  {}

        virtual void folderChanged (const File) {}
        virtual void fileChanged (const File, FileSystemEvent) {}
    };

    void addListener (Listener* newListener);

    void removeListener (Listener* listener);

private:
    class Impl;

    void folderChanged (const File& folder);
    void fileChanged (const File& file, FileSystemEvent fsEvent);

    ListenerList<Listener> listeners;

    OwnedArray<Impl> watched;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileSystemWatcher)
};

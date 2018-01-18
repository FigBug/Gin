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
    
    class Listener
    {
    public:
        virtual ~Listener()  {}

        virtual void folderChanged (const File)    {}
    };
    
    void addListener (Listener* newListener);

    void removeListener (Listener* listener);
    
private:
    class Impl;
    
    void folderChanged (const File& folder);
    
    ListenerList<Listener> listeners;
    
    OwnedArray<Impl> watched;
};

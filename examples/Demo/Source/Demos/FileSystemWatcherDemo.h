/*
  ==============================================================================

    FileSystemWatcherDemo.h

  ==============================================================================
*/

#pragma once

#include "DemosCommon.h"

//==============================================================================
#if defined JUCE_MAC || defined JUCE_WINDOWS || defined JUCE_LINUX
struct FileSystemWatcherDemo : public DemoComponent,
                               private gin::FileSystemWatcher::Listener
{
    FileSystemWatcherDemo()
    {
        setName ("File System Watcher");

        addAndMakeVisible (contents);
        addAndMakeVisible (events);

        contents.setMultiLine (true);
        events.setMultiLine (true);

        juce::File f = juce::File::getSpecialLocation (juce::File::userDesktopDirectory);
        watcher.addFolder (f);
        watcher.addListener (this);

        folderChanged (f);
    }

    void resized() override
    {
        auto rc = getLocalBounds();
        contents.setBounds (rc.removeFromTop (rc.getHeight() / 2));
        events.setBounds (rc);
    }

    void folderChanged (const juce::File& f) override
    {
        juce::Array<juce::File> files;
        f.findChildFiles (files, juce::File::findFiles, false);
        files.sort();

        contents.clear();

        juce::String txt;
        for (const auto& ff : files)
            txt += ff.getFileName() + "\n";

        contents.setText (txt);
    }

    void fileChanged (const juce::File& f, gin::FileSystemWatcher::FileSystemEvent fsEvent) override
    {
        auto eventToString = [] (gin::FileSystemWatcher::FileSystemEvent evt) -> juce::String
        {
            switch (evt)
            {
                case gin::FileSystemWatcher::fileCreated: return "Created";
                case gin::FileSystemWatcher::fileUpdated: return "Updated";
                case gin::FileSystemWatcher::fileDeleted: return "Deleted";
                case gin::FileSystemWatcher::fileRenamedOldName: return "Renamed From";
                case gin::FileSystemWatcher::fileRenamedNewName: return "Renamed To";
                default: jassertfalse; return {};
            }
        };

        events.moveCaretToEnd (false);
        events.insertTextAtCaret (f.getFullPathName() + ": " + eventToString (fsEvent) + "\n");

        events.scrollEditorToPositionCaret (0, events.getHeight() - 20);
    }

    juce::TextEditor contents, events;
    gin::FileSystemWatcher watcher;
};
#endif

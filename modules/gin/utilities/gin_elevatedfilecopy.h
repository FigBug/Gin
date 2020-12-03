/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

// TODO: no implementation for Linux yet

#if defined JUCE_MAC || defined JUCE_WINDOWS

/** Copies files, creating folders where required
    requesting admin access only if required
  */
class ElevatedFileCopy
{
public:
    enum Result
    {
        success,
        failed,
        cancelled,
        nopermissions
    };

    /** Add a directory to create */
    void createDir (const juce::File& dir);

    /** Add a file to copy. src and dst must be complete file names,
        neither can be a directory */
    void copyFile (const juce::File& src, const juce::File& dst);

    /** Add a file to move. src and dst must be complete file names,
        neither can be a directory */
    void moveFile (const juce::File& src, const juce::File& dst);

    /** Delete a file or directory */
    void deleteFile (const juce::File& f);

    /** Perform the copy

        If launchSelf is false, then on Windows cmd.exe will be executed
        with admin permissions to copy the files. This looks odd to the user
        since Windows Command Processor will ask for permissions to make changes
        rather than your app. Pass true to make your app get launched with admin
        permissions instead. In this case, you must call processCommandLine from
        yours apps initialise instead, and if it returns true, return from initialise
        asap and your app will quit as it's just a temporary process.
    */
    Result execute (bool launchSelf = false);

    /** Clear all files to be copied */
    void clear();

    /** Call from JUCEApplication::initialise() and abort the initialise process
        if returns true */
    static bool processCommandLine (juce::String commandLine);

    /** Run a script as root. See execute for the meaning of launch self */
    static Result runScriptWithAdminAccess (juce::File script, bool launchSelf);

private:
    struct FileItem
    {
        FileItem (juce::File s = {}, juce::File d = {})
            : src (s), dst (d)
        {}

        juce::File src;
        juce::File dst;
    };

    juce::File createScript (const juce::Array<juce::File>& toDelete,
                             const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                             const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                             const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess);

    juce::Array<FileItem> filesToCopy;
    juce::Array<FileItem> filesToMove;
    juce::Array<juce::File> dirsToCreate;
    juce::Array<juce::File> filesToDelete;

    JUCE_LEAK_DETECTOR (ElevatedFileCopy)
};

#endif

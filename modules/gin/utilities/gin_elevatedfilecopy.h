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
    void createDir (const File& dir);

    /** Add a file to copy. src and dst must be complete file names,
        neither can be a directory */
    void copyFile (const File& src, const File& dst);

    /** Delete a file or directory */
    void deleteFile (const File& f);

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
    static Result runScriptWithAdminAccess (File script, bool launchSelf);

private:
    struct FileItem
    {
        FileItem (File s = {}, File d = {})
            : src (s), dst (d)
        {}

        File src;
        File dst;
    };

    File createScript (const Array<File>& toDelete,
                       const Array<File>& dirsThatNeedAdminAccess,
                       const Array<ElevatedFileCopy::FileItem>& filesThatNeedAdminAccess);

    Array<FileItem> filesToCopy;
    Array<File> dirsToCreate;
    Array<File> filesToDelete;

    JUCE_LEAK_DETECTOR (ElevatedFileCopy)
};

#endif

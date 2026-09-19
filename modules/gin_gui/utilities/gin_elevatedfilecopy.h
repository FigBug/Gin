/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

// TODO: no implementation for Linux yet

#if defined JUCE_MAC || defined JUCE_WINDOWS

class ElevatedSession;

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

    /** Perform the copy via a shared elevated session, so admin access is
        prompted for at most once per session rather than once per call */
    Result execute (ElevatedSession& session);

    /** Clear all files to be copied */
    void clear();

    /** Call from JUCEApplication::initialise() and abort the initialise process
        if returns true */
    static bool processCommandLine (juce::String commandLine);

    /** Run a script as root. See execute for the meaning of launch self */
    static Result runScriptWithAdminAccess (juce::File script, bool launchSelf);

private:
    friend class ElevatedSession;

    struct FileItem
    {
        FileItem (juce::File s = {}, juce::File d = {})
            : src (s), dst (d)
        {}

        juce::File src;
        juce::File dst;
    };

    /** Does everything that can be done without admin access, returning
        true if anything is left over that needs it */
    bool classifyWork (juce::Array<juce::File>& toDelete,
                       juce::Array<juce::File>& dirsThatNeedAdminAccess,
                       juce::Array<FileItem>& filesToCopyThatNeedAdminAccess,
                       juce::Array<FileItem>& filesToMoveThatNeedAdminAccess);

    static juce::String createScriptText (const juce::Array<juce::File>& toDelete,
                                          const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                          const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                                          const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess);

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

/** Keeps one elevated worker process alive so the admin password prompt is
    only shown once per app run, rather than once per operation.

    On macOS the worker is a /bin/sh command loop started with
    AuthorizationExecuteWithPrivileges; the only way to reach it is the
    private pipe inherited from this process. On Windows the app is
    relaunched elevated (a single UAC prompt) and talked to over a named
    pipe guarded by a random token; call processCommandLine from your app's
    initialise and return asap if it returns true, as that instance is the
    worker.

    Scripts are sent by content, not path, and the worker writes them to a
    location only it can modify, so nothing runs as root from a world
    writable location.
  */
class ElevatedSession
{
public:
    ElevatedSession () = default;
    ~ElevatedSession ();

    /** Starts the worker, prompting for admin access if it isn't already
        running. Called automatically by runScript */
    ElevatedFileCopy::Result start ();

    /** True if the worker is running */
    bool isRunning ();

    /** Runs the contents of a shell script (macOS) or batch file (Windows)
        as root/admin, starting the worker if required. Blocks until the
        script has finished. The script's output is discarded, so redirect
        anything needed to a file */
    ElevatedFileCopy::Result runScript (const juce::String& contents);

    /** Shuts the worker down. The next runScript prompts again. Also called
        by the destructor, and the worker exits by itself if this process
        dies with the session still open */
    void stop ();

    /** Call from JUCEApplication::initialise() and abort the initialise
        process if returns true */
    static bool processCommandLine (juce::String commandLine);

private:
    ElevatedFileCopy::Result startInternal ();
    ElevatedFileCopy::Result runScriptInternal (const juce::String& contents);
    void stopInternal ();

    bool writeLine (const juce::String& line);
    juce::String readLine (int timeoutMs = -1);

    juce::CriticalSection sessionLock;

   #if JUCE_MAC
    const void* authRef = nullptr;
    FILE* workerPipe = nullptr;
   #else
    std::unique_ptr<juce::NamedPipe> workerPipe;
    void* workerProcess = nullptr;
    std::string readBuffer;
   #endif

    JUCE_DECLARE_NON_COPYABLE (ElevatedSession)
};

#endif

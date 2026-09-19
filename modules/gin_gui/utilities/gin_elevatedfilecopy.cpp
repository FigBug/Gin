 /*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if JUCE_MAC

static ElevatedFileCopy::Result runWithPermissions (juce::String cmd, juce::StringArray params)
{
    OSStatus err = noErr;
    auto path = cmd.toRawUTF8();

    juce::Array<const char*> rawParams;
    for (auto& s : params)
        rawParams.add (s.toRawUTF8());
    rawParams.add (nullptr);

    AuthorizationRef authorizationRef;
    AuthorizationItem item = { kAuthorizationRightExecute, strlen (path), &path, 0 };
    AuthorizationRights rights = { 1, &item };
    AuthorizationFlags flags = kAuthorizationFlagDefaults | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;

    err = AuthorizationCreate (nullptr, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);
    if (err != errAuthorizationSuccess)
        return ElevatedFileCopy::failed;

    err = AuthorizationCopyRights (authorizationRef, &rights, kAuthorizationEmptyEnvironment, flags, nullptr);
    if (err == errAuthorizationCanceled)
        return ElevatedFileCopy::cancelled;

    if (err != errAuthorizationSuccess)
        return ElevatedFileCopy::nopermissions;

    FILE* outputFile = nullptr;

   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    err = AuthorizationExecuteWithPrivileges (authorizationRef, path, kAuthorizationFlagDefaults, (char* const*)rawParams.getRawDataPointer(), &outputFile);
   #pragma clang diagnostic pop

    if (err == noErr)
    {
        while (true)
        {
            char buffer[1024];
            auto bytesRead = read (fileno(outputFile), buffer, sizeof(buffer));
            if (bytesRead < 1)
                break;
        }

        return ElevatedFileCopy::success;
    }
    return ElevatedFileCopy::nopermissions;
}

static juce::String escape (const juce::String& in)
{
    return in.replace (" ", "\\ ");
}

ElevatedFileCopy::Result ElevatedFileCopy::runScriptWithAdminAccess (juce::File script, bool launchSelf)
{
    juce::ignoreUnused (launchSelf);
    return runWithPermissions ("/bin/sh", { script.getFullPathName() });
}

juce::String ElevatedFileCopy::createScriptText (const juce::Array<juce::File>& toDelete,
                                                 const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                                 const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                                                 const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess)
{
    juce::String scriptText;

    scriptText += "#!/bin/sh\n";

    juce::Array<juce::File> dirs;

    for (auto f : toDelete)
        scriptText += "rm -Rf " + escape (f.getFullPathName()) + "\n";

    for (auto f : dirsThatNeedAdminAccess)
        dirs.add (f);

    for (auto f : filesToCopyThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto f : filesToMoveThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto d : dirs)
        scriptText += "mkdir -p " + escape (d.getFullPathName()) + "\n";

    for (auto d : dirsThatNeedAdminAccess)
        scriptText += "chmod 777 " + escape (d.getFullPathName()) + "\n";

    scriptText += "\n";

    for (auto f : filesToCopyThatNeedAdminAccess)
        scriptText += "cp -p " + escape (f.src.getFullPathName()) + " " + escape (f.dst.getFullPathName()) + " || exit 1\n";

    for (auto f : filesToMoveThatNeedAdminAccess)
        scriptText += "mv -p " + escape (f.src.getFullPathName()) + " " + escape (f.dst.getFullPathName()) + " || exit 1\n";

    return scriptText;
}

juce::File ElevatedFileCopy::createScript (const juce::Array<juce::File>& toDelete,
                                           const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                           const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                                           const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess)
{
    auto script = juce::File::getSpecialLocation (juce::File::tempDirectory).getNonexistentChildFile ("copy", ".sh", false);

    script.replaceWithText (createScriptText (toDelete, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess), false, false, "\n");

    return script;
}

//==============================================================================

ElevatedFileCopy::Result ElevatedSession::startInternal()
{
    if (workerPipe != nullptr)
        return ElevatedFileCopy::success;

    // A dead worker must not take the app down with it on the next write
    ::signal (SIGPIPE, SIG_IGN);

    auto path = "/bin/sh";

    AuthorizationRef authorizationRef;
    AuthorizationItem item = { kAuthorizationRightExecute, strlen (path), &path, 0 };
    AuthorizationRights rights = { 1, &item };
    AuthorizationFlags flags = kAuthorizationFlagDefaults | kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;

    auto err = AuthorizationCreate (nullptr, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &authorizationRef);
    if (err != errAuthorizationSuccess)
        return ElevatedFileCopy::failed;

    err = AuthorizationCopyRights (authorizationRef, &rights, kAuthorizationEmptyEnvironment, flags, nullptr);
    if (err == errAuthorizationCanceled)
    {
        AuthorizationFree (authorizationRef, kAuthorizationFlagDefaults);
        return ElevatedFileCopy::cancelled;
    }

    if (err != errAuthorizationSuccess)
    {
        AuthorizationFree (authorizationRef, kAuthorizationFlagDefaults);
        return ElevatedFileCopy::nopermissions;
    }

    // A command loop that stays resident as root. Scripts arrive base64
    // encoded so they can never contain a newline, and run with their output
    // silenced so the pipe stays a clean protocol channel. mktemp creates the
    // script 0600 root-owned, so unlike a script in /tmp nothing else can
    // swap its contents between write and execute. EOF on stdin means this
    // process is gone, so the loop just ends.
    auto loop =
        "while IFS= read -r line; do "
            "case \"$line\" in "
                "RUN\\ *) t=$(/usr/bin/mktemp /tmp/elevated_session.XXXXXX 2>/dev/null) || { echo DONE 126; continue; }; "
                    "printf %s \"${line#RUN }\" | /usr/bin/base64 -D > \"$t\" 2>/dev/null; "
                    "/bin/sh \"$t\" > /dev/null 2>&1; c=$?; "
                    "/bin/rm -f \"$t\"; "
                    "echo DONE $c;; "
                "EXIT*) exit 0;; "
            "esac; "
        "done";

    const char* args[] = { "-c", loop, nullptr };

    FILE* io = nullptr;

   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    err = AuthorizationExecuteWithPrivileges (authorizationRef, path, kAuthorizationFlagDefaults, (char* const*) args, &io);
   #pragma clang diagnostic pop

    if (err != noErr || io == nullptr)
    {
        AuthorizationFree (authorizationRef, kAuthorizationFlagDefaults);
        return ElevatedFileCopy::nopermissions;
    }

    setvbuf (io, nullptr, _IONBF, 0);

    authRef = authorizationRef;
    workerPipe = io;

    return ElevatedFileCopy::success;
}

bool ElevatedSession::writeLine (const juce::String& line)
{
    if (workerPipe == nullptr)
        return false;

    if (fputs ((line + "\n").toRawUTF8(), workerPipe) < 0)
        return false;

    fflush (workerPipe);
    return true;
}

juce::String ElevatedSession::readLine (int timeoutMs)
{
    juce::ignoreUnused (timeoutMs);

    if (workerPipe == nullptr)
        return {};

    juce::MemoryOutputStream out;

    for (;;)
    {
        auto c = fgetc (workerPipe);

        if (c == EOF)
            return {};

        if (c == '\n')
            break;

        out.writeByte (char (c));
    }

    return out.toUTF8().trim();
}

void ElevatedSession::stopInternal()
{
    if (workerPipe != nullptr)
    {
        writeLine ("EXIT");
        fclose (workerPipe);
        workerPipe = nullptr;
    }

    if (authRef != nullptr)
    {
        AuthorizationFree ((AuthorizationRef) authRef, kAuthorizationFlagDefaults);
        authRef = nullptr;
    }
}

#endif

#if JUCE_WINDOWS
static std::wstring toWideString (const std::string& s)
{
    int len = MultiByteToWideChar (CP_UTF8, 0, s.c_str(), (int)s.length() + 1, 0, 0);

    wchar_t* buffer = new wchar_t[len];
    MultiByteToWideChar (CP_UTF8, 0, s.c_str(), (int)s.length() + 1, buffer, len);

    std::wstring res (buffer);
    delete[] buffer;

    return res;
}

ElevatedFileCopy::Result ElevatedFileCopy::runScriptWithAdminAccess (juce::File script, bool launchSelf)
{
    juce::String app;
    juce::String params;

    if (launchSelf)
    {
        app = juce::File::getSpecialLocation (juce::File::currentExecutableFile).getFullPathName();
        params = "--elevatedfilecopy \"" + script.getFullPathName() + "\"";
    }
    else
    {
        app = "cmd.exe";
        params = "/c \"" + script.getFullPathName() +  "\"";
    }

    auto wideParams = toWideString (params.toRawUTF8());
    auto wideApp = toWideString (app.toRawUTF8());

    SHELLEXECUTEINFOW info;
    memset (&info, 0, sizeof (info));
    info.cbSize = sizeof (info);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = L"runas";
    info.lpFile = wideApp.c_str();
    info.lpParameters = wideParams.c_str();
    info.nShow = SW_HIDE;

    if (ShellExecuteExW (&info))
    {
        WaitForSingleObject (info.hProcess, INFINITE);

        DWORD exitCode = 0;
        GetExitCodeProcess (info.hProcess, &exitCode);
        CloseHandle (info.hProcess);

        return exitCode == 0 ? success : failed;
    }
    else
    {
        auto err = GetLastError();
        if (err == ERROR_CANCELLED)
            return cancelled;

        return nopermissions;
    }
}

juce::String ElevatedFileCopy::createScriptText (const juce::Array<juce::File>& toDelete,
                                                 const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                                 const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                                                 const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess)
{
    juce::String scriptText;

    juce::Array<juce::File> dirs;

    for (auto f : toDelete)
    {
        if (f.isDirectory())
            scriptText += "rmdir /s /q " + f.getFullPathName().quoted() + "\r\n";
        else if (f.existsAsFile())
            scriptText += "del /f " + f.getFullPathName().quoted() + "\r\n";
    }

    for (auto f : dirsThatNeedAdminAccess)
        dirs.add (f);

    for (auto f : filesToCopyThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto f : filesToMoveThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());


    for (auto d : dirs)
        scriptText += "if not exist \"" + d.getFullPathName() +  "\" mkdir " + d.getFullPathName().quoted() + "\r\n";

    scriptText += "\r\n";

    for (auto f : filesToCopyThatNeedAdminAccess)
    {
        scriptText += "copy " + f.src.getFullPathName().quoted() + " " + f.dst.getFullPathName().quoted() + "\r\n";
        scriptText += "if %errorlevel% neq 0 goto :error\r\n";
    }

    for (auto f : filesToMoveThatNeedAdminAccess)
    {
        scriptText += "move " + f.src.getFullPathName().quoted() + " " + f.dst.getFullPathName().quoted() + "\r\n";
        scriptText += "if %errorlevel% neq 0 goto :error\r\n";
    }


    scriptText += "exit /b 0\r\n";
    scriptText += ":error\r\n";
    scriptText += "exit /b 1\r\n";

    return scriptText;
}

juce::File ElevatedFileCopy::createScript (const juce::Array<juce::File>& toDelete,
                                           const juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                           const juce::Array<ElevatedFileCopy::FileItem>& filesToCopyThatNeedAdminAccess,
                                           const juce::Array<ElevatedFileCopy::FileItem>& filesToMoveThatNeedAdminAccess)
{
    auto script = juce::File::getSpecialLocation (juce::File::tempDirectory).getNonexistentChildFile ("copy", ".bat", false);

    script.replaceWithText (createScriptText (toDelete, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess));

    return script;
}

//==============================================================================

ElevatedFileCopy::Result ElevatedSession::startInternal()
{
    if (workerPipe != nullptr)
    {
        DWORD code = 0;
        if (workerProcess != nullptr && GetExitCodeProcess ((HANDLE) workerProcess, &code) && code == STILL_ACTIVE)
            return ElevatedFileCopy::success;

        // worker died, start over
        stopInternal();
    }

    auto pipeName = "elevated_session_" + juce::Uuid().toString();
    auto token = juce::Uuid().toString();

    auto p = std::make_unique<juce::NamedPipe>();
    if (! p->createNewPipe (pipeName, true))
        return ElevatedFileCopy::failed;

    auto app = juce::File::getSpecialLocation (juce::File::currentExecutableFile).getFullPathName();
    auto params = "--elevatedsession " + pipeName + " " + token;

    auto wideApp = toWideString (app.toRawUTF8());
    auto wideParams = toWideString (params.toRawUTF8());

    SHELLEXECUTEINFOW info;
    memset (&info, 0, sizeof (info));
    info.cbSize = sizeof (info);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.lpVerb = L"runas";
    info.lpFile = wideApp.c_str();
    info.lpParameters = wideParams.c_str();
    info.nShow = SW_HIDE;

    if (! ShellExecuteExW (&info))
    {
        if (GetLastError() == ERROR_CANCELLED)
            return ElevatedFileCopy::cancelled;

        return ElevatedFileCopy::nopermissions;
    }

    workerPipe = std::move (p);
    workerProcess = info.hProcess;
    readBuffer.clear();

    // The worker proves it's the process we launched before anything is sent
    // to it, since the pipe name is visible to other processes but the token
    // only travelled through the elevated command line
    if (readLine (30 * 1000) != "HELLO " + token)
    {
        stopInternal();
        return ElevatedFileCopy::failed;
    }

    return ElevatedFileCopy::success;
}

bool ElevatedSession::writeLine (const juce::String& line)
{
    if (workerPipe == nullptr)
        return false;

    juce::String withNewline = line + "\n";
    auto utf8 = withNewline.toRawUTF8();
    auto len = (int) strlen (utf8);

    return workerPipe->write (utf8, len, 10 * 1000) == len;
}

juce::String ElevatedSession::readLine (int timeoutMs)
{
    if (workerPipe == nullptr)
        return {};

    for (;;)
    {
        if (auto nl = readBuffer.find ('\n'); nl != std::string::npos)
        {
            auto line = juce::String::fromUTF8 (readBuffer.c_str(), (int) nl).trim();
            readBuffer.erase (0, nl + 1);
            return line;
        }

        char buf[512];
        auto n = workerPipe->read (buf, sizeof (buf), timeoutMs);

        if (n <= 0)
            return {};

        readBuffer.append (buf, (size_t) n);
    }
}

void ElevatedSession::stopInternal()
{
    if (workerPipe != nullptr)
    {
        writeLine ("EXIT");
        workerPipe->close();
        workerPipe = nullptr;
    }

    if (workerProcess != nullptr)
    {
        WaitForSingleObject ((HANDLE) workerProcess, 2000);
        CloseHandle ((HANDLE) workerProcess);
        workerProcess = nullptr;
    }

    readBuffer.clear();
}

/** The read side of the session, running in the elevated instance of the app */
static void elevatedSessionWorkerLoop (const juce::String& pipeName, const juce::String& token)
{
    juce::NamedPipe pipe;
    if (! pipe.openExisting (pipeName))
        return;

    auto writeLine = [&] (const juce::String& line)
    {
        juce::String withNewline = line + "\n";
        auto utf8 = withNewline.toRawUTF8();
        pipe.write (utf8, (int) strlen (utf8), 10 * 1000);
    };

    std::string pending;
    auto readLine = [&]() -> juce::String
    {
        for (;;)
        {
            if (auto nl = pending.find ('\n'); nl != std::string::npos)
            {
                auto line = juce::String::fromUTF8 (pending.c_str(), (int) nl).trim();
                pending.erase (0, nl + 1);
                return line;
            }

            char buf[4096];
            auto n = pipe.read (buf, sizeof (buf), -1);

            if (n <= 0)
                return {};

            pending.append (buf, (size_t) n);
        }
    };

    writeLine ("HELLO " + token);

    for (;;)
    {
        auto line = readLine();

        if (! line.startsWith ("RUN "))
            break;      // EXIT, or the app went away and took the pipe with it

        int code = 1;

        juce::MemoryOutputStream decoded;
        if (juce::Base64::convertFromBase64 (decoded, line.substring (4)))
        {
            // this temp dir belongs to the elevated user, unlike the app's own
            auto script = juce::File::getSpecialLocation (juce::File::tempDirectory).getNonexistentChildFile ("elevated_session", ".bat", false);

            if (script.replaceWithData (decoded.getData(), decoded.getDataSize()))
            {
                juce::ChildProcess cp;
                if (cp.start (juce::StringArray { "cmd.exe", "/c", script.getFullPathName() }))
                {
                    cp.waitForProcessToFinish (-1);
                    code = (int) cp.getExitCode();
                }

                script.deleteFile();
            }
        }

        writeLine ("DONE " + juce::String (code));
    }
}
#endif

#if defined JUCE_MAC || defined JUCE_WINDOWS

void ElevatedFileCopy::createDir (const juce::File& dir)
{
    dirsToCreate.add (dir);
}

void ElevatedFileCopy::copyFile (const juce::File& src, const juce::File& dst)
{
    filesToCopy.add ({ src, dst });
}

void ElevatedFileCopy::moveFile (const juce::File& src, const juce::File& dst)
{
    filesToMove.add ({ src, dst });
}

void ElevatedFileCopy::deleteFile (const juce::File& f)
{
    filesToDelete.add (f);
}

bool ElevatedFileCopy::classifyWork (juce::Array<juce::File>& filesToDeleteThatNeedAdminAccess,
                                     juce::Array<juce::File>& dirsThatNeedAdminAccess,
                                     juce::Array<FileItem>& filesToCopyThatNeedAdminAccess,
                                     juce::Array<FileItem>& filesToMoveThatNeedAdminAccess)
{
    for (auto f : filesToDelete)
    {
        if (f.existsAsFile())
        {
            if (! f.deleteFile())
                filesToDeleteThatNeedAdminAccess.add (f);
        }
        else if (f.isDirectory())
        {
            if (! f.deleteRecursively())
                filesToDeleteThatNeedAdminAccess.add (f);
        }
    }

    for (auto f : dirsToCreate)
    {
        if (! f.isDirectory())
            f.createDirectory();

        bool ok = f.isDirectory();

        if (! ok)
            dirsThatNeedAdminAccess.add (f);
    }

    for (auto f : filesToCopy)
    {
        bool ok = false;

        auto dstDir = f.dst.getParentDirectory();
        if (! dstDir.isDirectory())
            dstDir.createDirectory();

        if (dstDir.isDirectory())
            ok = f.src.copyFileTo (f.dst);

        if (! ok)
            filesToCopyThatNeedAdminAccess.add (f);
    }

    for (auto f : filesToMove)
    {
        bool ok = false;

        auto dstDir = f.dst.getParentDirectory();
        if (! dstDir.isDirectory())
            dstDir.createDirectory();

        if (dstDir.isDirectory())
            ok = f.src.moveFileTo (f.dst);

        if (! ok)
            filesToMoveThatNeedAdminAccess.add (f);
    }

    return filesToDeleteThatNeedAdminAccess.size() > 0 || dirsThatNeedAdminAccess.size() > 0 || filesToCopyThatNeedAdminAccess.size() > 0 || filesToMoveThatNeedAdminAccess.size() > 0;
}

ElevatedFileCopy::Result ElevatedFileCopy::execute (bool launchSelf)
{
    juce::Array<juce::File> filesToDeleteThatNeedAdminAccess;
    juce::Array<juce::File> dirsThatNeedAdminAccess;
    juce::Array<FileItem> filesToCopyThatNeedAdminAccess;
    juce::Array<FileItem> filesToMoveThatNeedAdminAccess;

    if (classifyWork (filesToDeleteThatNeedAdminAccess, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess))
    {
        juce::File script = createScript (filesToDeleteThatNeedAdminAccess, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess);
        auto res = runScriptWithAdminAccess (script, launchSelf);
        script.deleteFile();

        return res;
    }

    return success;
}

ElevatedFileCopy::Result ElevatedFileCopy::execute (ElevatedSession& session)
{
    juce::Array<juce::File> filesToDeleteThatNeedAdminAccess;
    juce::Array<juce::File> dirsThatNeedAdminAccess;
    juce::Array<FileItem> filesToCopyThatNeedAdminAccess;
    juce::Array<FileItem> filesToMoveThatNeedAdminAccess;

    if (classifyWork (filesToDeleteThatNeedAdminAccess, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess))
        return session.runScript (createScriptText (filesToDeleteThatNeedAdminAccess, dirsThatNeedAdminAccess, filesToCopyThatNeedAdminAccess, filesToMoveThatNeedAdminAccess));

    return success;
}

bool ElevatedFileCopy::processCommandLine (juce::String commandLine)
{
   #if JUCE_WINDOWS
    if (commandLine.contains ("--elevatedfilecopy"))
    {
        juce::String script = commandLine.fromFirstOccurrenceOf ("--elevatedfilecopy \"", false, false).upToFirstOccurrenceOf ("\"", false, false);

        if (juce::File (script).existsAsFile())
        {
            juce::String params = "/c \"" + script + "\"";
            auto wideParams = toWideString (params.toRawUTF8());

            SHELLEXECUTEINFOW info;
            memset (&info, 0, sizeof (info));
            info.cbSize = sizeof (info);
            info.fMask = SEE_MASK_NOCLOSEPROCESS;
            info.lpVerb = L"runas";
            info.lpFile = L"cmd.exe";
            info.lpParameters = wideParams.c_str();
            info.nShow = SW_HIDE;

            if (ShellExecuteExW (&info))
            {
                WaitForSingleObject (info.hProcess, INFINITE);

                DWORD exitCode = 0;
                GetExitCodeProcess (info.hProcess, &exitCode);
                CloseHandle (info.hProcess);

                if (auto inst = juce::JUCEApplication::getInstance())
                    inst->setApplicationReturnValue (exitCode);
            }
            else
            {
                if (auto inst = juce::JUCEApplication::getInstance())
                    inst->setApplicationReturnValue (1);
            }
        }

        juce::JUCEApplication::quit();
        return true;
    }
   #else
    ignoreUnused (commandLine);
   #endif
    return false;
}

void ElevatedFileCopy::clear()
{
    filesToMove.clear();
    filesToCopy.clear();
    filesToDelete.clear();
    dirsToCreate.clear();
}

//==============================================================================

ElevatedSession::~ElevatedSession()
{
    stop();
}

ElevatedFileCopy::Result ElevatedSession::start()
{
    juce::ScopedLock sl (sessionLock);
    return startInternal();
}

bool ElevatedSession::isRunning()
{
    juce::ScopedLock sl (sessionLock);
    return workerPipe != nullptr;
}

ElevatedFileCopy::Result ElevatedSession::runScript (const juce::String& contents)
{
    juce::ScopedLock sl (sessionLock);
    return runScriptInternal (contents);
}

ElevatedFileCopy::Result ElevatedSession::runScriptInternal (const juce::String& contents)
{
    if (auto res = startInternal(); res != ElevatedFileCopy::success)
        return res;

    if (! writeLine ("RUN " + juce::Base64::toBase64 (contents)))
    {
        stopInternal();
        return ElevatedFileCopy::failed;
    }

    for (;;)
    {
        auto line = readLine();

        if (line.isEmpty())
        {
            // the worker died mid script
            stopInternal();
            return ElevatedFileCopy::failed;
        }

        if (line.startsWith ("DONE"))
            return line.substring (4).trim().getIntValue() == 0 ? ElevatedFileCopy::success : ElevatedFileCopy::failed;
    }
}

void ElevatedSession::stop()
{
    juce::ScopedLock sl (sessionLock);
    stopInternal();
}

bool ElevatedSession::processCommandLine (juce::String commandLine)
{
   #if JUCE_WINDOWS
    if (! commandLine.contains ("--elevatedsession"))
        return false;

    auto args = juce::StringArray::fromTokens (commandLine.fromFirstOccurrenceOf ("--elevatedsession", false, false), " ", "\"");
    args.removeEmptyStrings();

    if (args.size() >= 2)
        elevatedSessionWorkerLoop (args[0], args[1]);

    juce::JUCEApplication::quit();
    return true;
   #else
    ignoreUnused (commandLine);
    return false;
   #endif
}

#endif

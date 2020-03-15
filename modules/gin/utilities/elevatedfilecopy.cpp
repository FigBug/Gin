 /*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#if JUCE_MAC

ElevatedFileCopy::Result runWithPermissions (String cmd, StringArray params)
{
    OSStatus err = noErr;
    auto path = cmd.toRawUTF8();

    Array<const char*> rawParams;
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
        auto processIdentifier = fcntl (fileno (outputFile), F_GETOWN, 0);

        AuthorizationFree (authorizationRef, kAuthorizationFlagDefaults);

        int status;
        pid_t pid = 0;

        while ((pid = waitpid (processIdentifier, &status, WNOHANG)) == 0)
            Thread::sleep (10);

        fclose (outputFile);

        auto terminationStatus = WEXITSTATUS (status);
        if (terminationStatus == 0)
            return ElevatedFileCopy::success;

        return ElevatedFileCopy::failed;
    }
    return ElevatedFileCopy::nopermissions;
}

static String escape (const String& in)
{
    return in.replace (" ", "\\ ");
}

ElevatedFileCopy::Result ElevatedFileCopy::runScriptWithAdminAccess (File script, bool launchSelf)
{
    ignoreUnused (launchSelf);
    runWithPermissions ("/bin/sh", { script.getFullPathName() });
    return success;
}

File ElevatedFileCopy::createScript (const Array<File>& toDelete,
                                     const Array<File>& dirsThatNeedAdminAccess,
                                     const Array<ElevatedFileCopy::FileItem>& filesThatNeedAdminAccess)
{
    auto script = File::getSpecialLocation (File::tempDirectory).getNonexistentChildFile ("copy", ".sh", false);

    String scriptText;

    scriptText += "#!/bin/sh\n";

    Array<File> dirs;

    for (auto f : toDelete)
        scriptText += "rm -Rf " + escape (f.getFullPathName()) + "\n";

    for (auto f : dirsThatNeedAdminAccess)
        dirs.add (f);

    for (auto f : filesThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto d : dirs)
        scriptText += "mkdir -p " + escape (d.getFullPathName()) + "\n";

    for (auto d : dirsThatNeedAdminAccess)
        scriptText += "chmod 777 " + escape (d.getFullPathName()) + "\n";

    scriptText += "\n";

    for (auto f : filesThatNeedAdminAccess)
        scriptText += "cp -p " + escape (f.src.getFullPathName()) + " " + escape (f.dst.getFullPathName()) + " || exit 1\n";

    script.replaceWithText (scriptText, false, false, "\n");

    return script;
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

ElevatedFileCopy::Result ElevatedFileCopy::runScriptWithAdminAccess (File script, bool launchSelf)
{
    String app;
    String params;

    if (launchSelf)
    {
        app = File::getSpecialLocation (File::currentExecutableFile).getFullPathName();
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

File ElevatedFileCopy::createScript (const Array<File>& toDelete,
                                     const Array<File>& dirsThatNeedAdminAccess,
                                     const Array<ElevatedFileCopy::FileItem>& filesThatNeedAdminAccess)
{
    auto script = File::getSpecialLocation (File::tempDirectory).getNonexistentChildFile ("copy", ".bat", false);

    String scriptText;

    Array<File> dirs;

    for (auto f : toDelete)
    {
        if (f.isDirectory())
            scriptText += "rmdir /s /q " + f.getFullPathName().quoted() + "\r\n";
        else if (f.existsAsFile())
            scriptText += "del /f " + f.getFullPathName().quoted() + "\r\n";
    }

    for (auto f : dirsThatNeedAdminAccess)
        dirs.add (f);

    for (auto f : filesThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto d : dirs)
        scriptText += "if not exist \"" + d.getFullPathName() +  "\" mkdir " + d.getFullPathName().quoted() + "\r\n";

    scriptText += "\r\n";

    for (auto f : filesThatNeedAdminAccess)
    {
        scriptText += "copy " + f.src.getFullPathName().quoted() + " " + f.dst.getFullPathName().quoted() + "\r\n";
        scriptText += "if %errorlevel% neq 0 goto :error\r\n";
    }

    scriptText += "exit /b 0\r\n";
    scriptText += ":error\r\n";
    scriptText += "exit /b 1\r\n";

    script.replaceWithText (scriptText);

    return script;
}
#endif

#if defined JUCE_MAC || defined JUCE_WINDOWS

void ElevatedFileCopy::createDir (const File& dir)
{
    dirsToCreate.add (dir);
}

void ElevatedFileCopy::copyFile (const File& src, const File& dst)
{
    filesToCopy.add ({ src, dst });
}

void ElevatedFileCopy::deleteFile (const File& f)
{
    filesToDelete.add (f);
}

ElevatedFileCopy::Result ElevatedFileCopy::execute (bool launchSelf)
{
    Array<File> filesToDeleteThatNeedAdminAccess;
    Array<File> dirsThatNeedAdminAccess;
    Array<FileItem> filesThatNeedAdminAccess;

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
            filesThatNeedAdminAccess.add (f);
    }

    if (filesToDeleteThatNeedAdminAccess.size() > 0 || dirsThatNeedAdminAccess.size() > 0 || filesThatNeedAdminAccess.size() > 0)
    {
        File script = createScript (filesToDeleteThatNeedAdminAccess, dirsThatNeedAdminAccess, filesThatNeedAdminAccess);
        auto res = runScriptWithAdminAccess (script, launchSelf);
        script.deleteFile();

        return res;
    }

    return success;
}

bool ElevatedFileCopy::processCommandLine (juce::String commandLine)
{
   #if JUCE_WINDOWS
    if (commandLine.contains ("--elevatedfilecopy"))
    {
        String script = commandLine.fromFirstOccurrenceOf ("--elevatedfilecopy \"", false, false).upToFirstOccurrenceOf ("\"", false, false);

        if (File (script).existsAsFile())
        {
            String params = "/c \"" + script + "\"";
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

                if (auto inst = JUCEApplication::getInstance())
                    inst->setApplicationReturnValue (exitCode);
            }
            else
            {
                if (auto inst = JUCEApplication::getInstance())
                    inst->setApplicationReturnValue (1);
            }
        }

        JUCEApplication::quit();
        return true;
    }
   #else
    ignoreUnused (commandLine);
   #endif
    return false;
}

void ElevatedFileCopy::clear()
{
    filesToCopy.clear();
}

#endif

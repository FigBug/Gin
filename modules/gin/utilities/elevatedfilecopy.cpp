 /*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

static std::wstring toWideString (const std::string& s)
{
    int len = MultiByteToWideChar (CP_UTF8, 0, s.c_str(), (int)s.length() + 1, 0, 0);

    wchar_t* buffer = new wchar_t[len];
    MultiByteToWideChar (CP_UTF8, 0, s.c_str(), (int)s.length() + 1, buffer, len);

    std::wstring res (buffer);
    delete[] buffer;

    return res;
}

ElevatedFileCopy::Result ElevatedFileCopy::runScriptWithAdminAccess (File script)
{
    String params = "/c \"" + script.getFullPathName() +  "\"";
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

File ElevatedFileCopy::createScript (const Array<ElevatedFileCopy::FileItem>& filesThatNeedAdminAccess)
{
    auto script = File::getSpecialLocation (File::tempDirectory).getNonexistentChildFile ("copy", ".bat", false);

    String scriptText;

    Array<File> dirs;
    for (auto f : filesThatNeedAdminAccess)
        if (! f.dst.getParentDirectory().isDirectory())
            dirs.addIfNotAlreadyThere (f.dst.getParentDirectory());

    for (auto d : dirs)
        scriptText += "if not exist \"" + d.getFullPathName() +  "\" mkdir \"" + d.getFullPathName() + "\"\r\n";

    scriptText += "\r\n";

    for (auto f : filesThatNeedAdminAccess)
    {
        scriptText += "copy \"" + f.src.getFullPathName() + "\" \"" + f.dst.getFullPathName() + "\r\n";
        scriptText += "if %errorlevel% neq 0 goto :error\r\n";
    }

    scriptText += "exit /b 0\r\n";
    scriptText += ":error\r\n";
    scriptText += "exit /b 1\r\n";

    script.replaceWithText (scriptText);

    return script;
}

void ElevatedFileCopy::addFile (File src, File dst)
{
	filesToCopy.add ({ src, dst });
}

ElevatedFileCopy::Result ElevatedFileCopy::execute()
{
	Array<FileItem> filesThatNeedAdminAccess;

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

    if (filesThatNeedAdminAccess.size() > 0)
    {
        File script = createScript (filesThatNeedAdminAccess);
        auto res = runScriptWithAdminAccess (script);
        script.deleteFile();

        return res;
    }

	return success;
}

void ElevatedFileCopy::clear()
{
	filesToCopy.clear();
}

/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

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

    /** Add a file to copy. src and dst must be complete file names,
        neither can be a directory */
	void addFile (File src, File dst);

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

private:
	struct FileItem
	{
		FileItem (File s = {}, File d = {})
			: src (s), dst (d)
		{}

		File src;
		File dst;
	};

    File createScript (const Array<ElevatedFileCopy::FileItem>& filesThatNeedAdminAccess);
    Result runScriptWithAdminAccess (File script, bool launchSelf);

	Array<FileItem> filesToCopy;

    JUCE_LEAK_DETECTOR (ElevatedFileCopy)
};

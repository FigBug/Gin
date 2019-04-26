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

    /** Perform the copy */
	Result execute();

    /** Clear all files to be copied */
	void clear();

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
    Result runScriptWithAdminAccess (File script);

	Array<FileItem> filesToCopy;

    JUCE_LEAK_DETECTOR (ElevatedFileCopy)
};

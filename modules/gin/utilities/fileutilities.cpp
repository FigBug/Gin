/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

bool overwriteWithText (const juce::File& f, const juce::String& text, bool asUnicode,
                        bool writeUnicodeHeaderBytes,
                        const char* lineEndings)
{
    FileOutputStream out (f);

    if (out.failedToOpen())
        return false;

    out.setPosition (0);
    bool ok = out.writeText (text, asUnicode, writeUnicodeHeaderBytes, lineEndings);
    out.truncate();

    return ok;
}

bool overwriteWithData (const juce::File& f, const juce::MemoryBlock& data)
{
    return overwriteWithData (f, data.getData(), data.getSize());
}

bool overwriteWithData (const juce::File& f, const void* data, size_t size)
{
    FileOutputStream out (f);

    if (out.failedToOpen())
        return false;

    out.setPosition (0);
    bool ok = out.write (data, size);
    out.truncate();

    return ok;
}

int64 getLastModificationTime (const File& f)
{
   #if JUCE_MAC || JUCE_LINUX
    struct stat info;
    stat (f.getFullPathName().toUTF8(), &info);
    return (int64) info.st_mtimespec.tv_sec * 1000 + info.st_mtimespec.tv_nsec / 1000000;
   #else
    return f.getLastModifiedTime().toMilliseconds();
   #endif
}

int64 getLastAccessTime (const File& f)
{
   #if JUCE_MAC || JUCE_LINUX
    struct stat info;
    stat (f.getFullPathName().toUTF8(), &info);
    return (int64) info.st_atimespec.tv_sec * 1000 + info.st_atimespec.tv_nsec / 1000000;
   #else
    return f.getLastAccessTime().toMilliseconds();
   #endif
}

bool setLastModificationTime (const File& f, int64 when)
{
   #if JUCE_MAC || JUCE_LINUX
    struct timeval times[2];
    auto accessTime = getLastAccessTime (f);
    times[0].tv_sec  = accessTime / 1000;
    times[0].tv_usec = (accessTime % 1000) * 1000;

    times[1].tv_sec  = when / 1000;
    times[1].tv_usec = (when % 1000) * 1000;

    const char* name = f.getFullPathName().toRawUTF8();
    return utimes (name, times) == 0;
   #else
    return f.setLastModificationTime (f, when);
   #endif
}

bool setLastAccessTime (const File& f, int64 when)
{
   #if JUCE_MAC || JUCE_LINUX
    struct timeval times[2];
    times[0].tv_sec  = when / 1000;
    times[0].tv_usec = (when % 1000) * 1000;

    auto modificationTime = getLastModificationTime (f);
    times[1].tv_sec  = modificationTime / 1000;
    times[1].tv_usec = (modificationTime % 1000) * 1000;

    const char* name = f.getFullPathName().toRawUTF8();
    return utimes (name, times) == 0;
   #else
    return f.setLastAccessTime (f, when);
   #endif
}

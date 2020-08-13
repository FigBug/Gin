/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

bool overwriteWithText (const juce::File& f, const juce::String& text, bool asUnicode,
                        bool writeUnicodeHeaderBytes,
                        const char* lineEndings)
{
    juce::FileOutputStream out (f);

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
    juce::FileOutputStream out (f);

    if (out.failedToOpen())
        return false;

    out.setPosition (0);
    bool ok = true;
    if (size > 0)
        ok = out.write (data, size);
    out.truncate();

    return ok;
}

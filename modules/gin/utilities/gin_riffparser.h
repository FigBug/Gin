/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Parse all the chunks in RIFF files (like wav, aiff, etc)
 */
class RIFFParser
{
public:
    RIFFParser (const juce::File& f, std::function<void (uint32_t, const uint8_t*, uint32_t)>);
    RIFFParser (const juce::MemoryBlock& m, std::function<void (uint32_t, const uint8_t*, uint32_t)>);

private:
    void parse (const void* data, size_t size);
    void handleChunk (const uint8_t*& d);
    void handleList (const uint8_t*& d);

    std::function<void (uint32_t, const uint8_t*, uint32_t)>  onChunk;
};

inline int getWavetableSize (const juce::MemoryBlock& m)
{
    int wtSize = 0;

    RIFFParser (m, [&] (uint32_t dwFourCC, const uint8_t* data, uint32_t size)
    {
        if (juce::ByteOrder::littleEndianInt ("clm ") == dwFourCC)
        {
            juce::MemoryBlock xfer (data, size);

            auto str = xfer.toString();
            if (str.startsWith ("<!>"))
            {
                str = str.substring (3);
                wtSize = str.upToFirstOccurrenceOf (" ", false, false).getIntValue();
            }
        }
    });

    return wtSize;
}

inline int getWavetableSize (const juce::File& f)
{
    juce::MemoryBlock mb;
    f.loadFileAsData (mb);
    return getWavetableSize (mb);
}

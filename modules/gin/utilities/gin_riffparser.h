/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Parses RIFF (Resource Interchange File Format) files.

    RIFF is a generic file container format used by many audio and video formats
    including WAV, AVI, and WebP. This parser iterates through all chunks in a
    RIFF file and calls a callback for each one.

    Common RIFF-based formats:
    - WAV (WAVE): Audio files
    - AVI: Video files
    - WebP: Image files
    - AIFF: Audio Interchange File Format

    @see getWavetableSize
*/
class RIFFParser
{
public:
    /** Parses a RIFF file and calls the callback for each chunk.

        @param f        The RIFF file to parse
        @param onChunk  Callback function receiving (fourCC, data, size) for each chunk
                        where fourCC is the chunk identifier, data points to chunk contents,
                        and size is the chunk size in bytes
    */
    RIFFParser (const juce::File& f, std::function<void (uint32_t, const uint8_t*, uint32_t)>);

    /** Parses RIFF data from memory and calls the callback for each chunk.

        @param m        A MemoryBlock containing RIFF data
        @param onChunk  Callback function receiving (fourCC, data, size) for each chunk
                        where fourCC is the chunk identifier, data points to chunk contents,
                        and size is the chunk size in bytes
    */
    RIFFParser (const juce::MemoryBlock& m, std::function<void (uint32_t, const uint8_t*, uint32_t)>);

private:
    void parse (const void* data, size_t size);
    void handleChunk (const uint8_t*& d);
    void handleList (const uint8_t*& d);

    std::function<void (uint32_t, const uint8_t*, uint32_t)>  onChunk;
};

/** Extracts wavetable size from a RIFF file in memory.

    Searches for a "clm " chunk in the RIFF data and extracts the wavetable
    size from it. This is used for wavetable synthesis formats that store
    metadata in RIFF chunks.

    @param m  MemoryBlock containing RIFF file data
    @returns  The wavetable size in samples, or 0 if not found
*/
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

/** Extracts wavetable size from a RIFF file.

    Searches for a "clm " chunk in the RIFF file and extracts the wavetable
    size from it. This is used for wavetable synthesis formats that store
    metadata in RIFF chunks.

    @param f  The RIFF file to read
    @returns  The wavetable size in samples, or 0 if not found
*/
inline int getWavetableSize (const juce::File& f)
{
    juce::MemoryBlock mb;
    f.loadFileAsData (mb);
    return getWavetableSize (mb);
}

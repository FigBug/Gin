/*==============================================================================

 Copyright (c) 2018 - 2026 by Roland Rabien.
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Overwrites a file with text content in a single atomic operation.

    Unlike juce::File::replaceWithText(), this function writes directly to the target
    file without creating a temporary file first. This is faster but provides less
    safety if the write operation fails partway through.

    @param f                        The file to overwrite
    @param text                     The text content to write
    @param asUnicode                If true, writes as UTF-16, otherwise as UTF-8
    @param writeUnicodeHeaderBytes  If true and asUnicode is true, writes a BOM (byte order mark)
    @param lineEndings              The line ending style to use (nullptr for platform default)
    @returns                        true if the operation succeeded, false otherwise
*/
bool overwriteWithText (const juce::File& f, const juce::String& text,
                        bool asUnicode = false,
                        bool writeUnicodeHeaderBytes = false,
                        const char* lineEndings = nullptr);

/** Overwrites a file with binary data in a single atomic operation.

    Unlike juce::File::replaceWithData(), this function writes directly to the target
    file without creating a temporary file first. This is faster but provides less
    safety if the write operation fails partway through.

    @param f     The file to overwrite
    @param data  The memory block containing the data to write
    @returns     true if the operation succeeded, false otherwise
*/
bool overwriteWithData (const juce::File& f, const juce::MemoryBlock& data);

/** Overwrites a file with binary data in a single atomic operation.

    Unlike juce::File::replaceWithData(), this function writes directly to the target
    file without creating a temporary file first. This is faster but provides less
    safety if the write operation fails partway through.

    @param f     The file to overwrite
    @param data  Pointer to the data to write
    @param size  Number of bytes to write
    @returns     true if the operation succeeded, false otherwise
*/
bool overwriteWithData (const juce::File& f, const void* data, size_t size);



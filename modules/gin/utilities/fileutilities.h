/*==============================================================================

 Copyright 2019 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/* Replaces a file with text in one operation without creating a temp file
 */
bool overwriteWithText (const juce::File& f, const juce::String& text,
                        bool asUnicode = false,
                        bool writeUnicodeHeaderBytes = false,
                        const char* lineEndings = nullptr);

/* Replaces a file with data in one operation without creating a temp file
 */
bool overwriteWithData (const juce::File& f, const juce::MemoryBlock& data);

bool overwriteWithData (const juce::File& f, const void* data, size_t size);

/* Get / Set file last access / modified times with milliseconf resolution.
   These are required because on Linux/macOS, juce only returns file times with
   1 second accuracy
*/

juce::int64 getLastModificationTime (const juce::File& f);

juce::int64 getLastAccessTime (const juce::File& f);

bool setLastModificationTime (const juce::File& f, juce::int64 when);

bool setLastAccessTime (const juce::File& f, juce::int64 when);



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



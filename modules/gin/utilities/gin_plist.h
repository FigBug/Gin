/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Parses an Apple Property List (plist) file and returns its contents as a juce::var.

    Plist files are commonly used on macOS and iOS for storing application preferences
    and configuration data. Only XML plist format is supported; binary plists are not supported.

    @param f  The plist file to parse
    @returns  A juce::var containing the parsed data structure (typically a DynamicObject)
*/
juce::var parsePlist (const juce::File& f);

/** Parses an Apple Property List (plist) from a string and returns its contents.

    Parses plist data from a string containing XML-formatted plist content.

    @param s  A string containing XML plist data
    @returns  A juce::var containing the parsed data structure (typically a DynamicObject)
*/
juce::var parsePlist (const juce::String& s);

/** Parses an Apple Property List (plist) from XML and returns its contents.

    Converts a parsed XML element representing a plist into a juce::var structure.

    @param f  An XmlElement containing the plist data
    @returns  A juce::var containing the parsed data structure (typically a DynamicObject)
*/
juce::var parsePlist (const juce::XmlElement& f);


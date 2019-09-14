/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Converts to/from MessagePack and juce::var
 
 Due to limitations of juce:var, there are no unsigned ints,
 object keys can only be strings (non strings will be converted
 to string if possible)
 
 All custom types will be returned as juce::MemoryBlock with the
 first byte as the type.
 
 All strings are assumed to be utf8
 */
class MessagePack
{
public:
    static juce::MemoryBlock toMessagePack (const juce::var& obj);
    
    static juce::var parse (const juce::MemoryBlock& data);
};

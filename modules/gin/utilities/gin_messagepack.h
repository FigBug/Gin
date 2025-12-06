/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Converts between MessagePack binary format and juce::var.

    MessagePack is an efficient binary serialization format that's more compact
    than JSON. This class provides bidirectional conversion between MessagePack
    data and JUCE's var type.

    Limitations due to juce::var constraints:
    - No unsigned integer types (all integers are signed)
    - Object keys must be strings (non-string keys will be converted if possible)
    - Custom types are returned as juce::MemoryBlock with the first byte as the type
    - All strings are assumed to be UTF-8 encoded

    @see toMessagePack, parse
*/
class MessagePack
{
public:
    /** Converts a juce::var to MessagePack binary format.

        Serializes a var (and any nested structures) into MessagePack binary format.
        The var can contain primitives, arrays, and objects which will be encoded
        appropriately.

        @param obj  The var to serialize
        @returns    A MemoryBlock containing the MessagePack-encoded data
    */
    static juce::MemoryBlock toMessagePack (const juce::var& obj);

    /** Parses MessagePack binary data into a juce::var.

        Deserializes MessagePack binary data back into a var structure. Arrays
        become var arrays, objects become DynamicObjects, and primitives are
        converted to appropriate var types.

        @param data  The MemoryBlock containing MessagePack-encoded data
        @returns     The deserialized var structure
    */
    static juce::var parse (const juce::MemoryBlock& data);
};

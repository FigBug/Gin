/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class RIFFParserTests : public juce::UnitTest
{
public:
    RIFFParserTests() : juce::UnitTest ("RIFF Parser", "gin") {}

    void runTest() override
    {
        testBasicRIFFParsing();
        testMultipleChunks();
        testListChunks();
        testNestedLists();
        testOddSizedChunks();
        testEmptyRIFF();
        testInvalidData();
    }

private:
    struct ChunkInfo
    {
        uint32_t fourCC;
        juce::MemoryBlock data;
    };

    static uint32_t makeFourCC (const char* str)
    {
        return (uint32_t(str[0]) << 0) | (uint32_t(str[1]) << 8) |
               (uint32_t(str[2]) << 16) | (uint32_t(str[3]) << 24);
    }

    void testBasicRIFFParsing()
    {
        beginTest ("Basic RIFF Parsing");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (12); // Size of rest of file
        stream.writeInt (makeFourCC ("TEST"));

        // Write a simple chunk
        stream.writeInt (makeFourCC ("data"));
        stream.writeInt (4); // Chunk size
        stream.writeInt (0x12345678); // Chunk data

        juce::Array<ChunkInfo> chunks;

        RIFFParser parser (riffData, [&chunks] (uint32_t fourCC, const uint8_t* data, uint32_t size)
        {
            ChunkInfo info;
            info.fourCC = fourCC;
            info.data.append (data, size);
            chunks.add (info);
        });

        expectEquals (chunks.size(), 1, "Should find one chunk");
        expectEquals (chunks[0].fourCC, makeFourCC ("data"), "Chunk FourCC should match");
        expectEquals ((int) chunks[0].data.getSize(), 4, "Chunk data size should match");
    }

    void testMultipleChunks()
    {
        beginTest ("Multiple Chunks");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (32); // Size
        stream.writeInt (makeFourCC ("TEST"));

        // Write first chunk
        stream.writeInt (makeFourCC ("fmt "));
        stream.writeInt (4);
        stream.writeInt (0xAAAAAAAA);

        // Write second chunk
        stream.writeInt (makeFourCC ("data"));
        stream.writeInt (8);
        stream.writeInt (0x11111111);
        stream.writeInt (0x22222222);

        juce::Array<ChunkInfo> chunks;

        RIFFParser parser (riffData, [&chunks] (uint32_t fourCC, const uint8_t* data, uint32_t size)
        {
            ChunkInfo info;
            info.fourCC = fourCC;
            info.data.append (data, size);
            chunks.add (info);
        });

        expectEquals (chunks.size(), 2, "Should find two chunks");
        expectEquals (chunks[0].fourCC, makeFourCC ("fmt "), "First chunk FourCC should match");
        expectEquals (chunks[1].fourCC, makeFourCC ("data"), "Second chunk FourCC should match");
        expectEquals ((int) chunks[0].data.getSize(), 4, "First chunk size should match");
        expectEquals ((int) chunks[1].data.getSize(), 8, "Second chunk size should match");
    }

    void testListChunks()
    {
        beginTest ("LIST Chunks");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (32); // Size
        stream.writeInt (makeFourCC ("TEST"));

        // Write LIST chunk
        stream.writeInt (makeFourCC ("LIST"));
        stream.writeInt (20); // LIST size (includes type + contained chunks)
        stream.writeInt (makeFourCC ("INFO"));

        // Write chunk inside LIST
        stream.writeInt (makeFourCC ("INAM"));
        stream.writeInt (4);
        stream.writeInt (0xBBBBBBBB);

        juce::Array<ChunkInfo> chunks;

        RIFFParser parser (riffData, [&chunks] (uint32_t fourCC, const uint8_t* data, uint32_t size)
        {
            ChunkInfo info;
            info.fourCC = fourCC;
            info.data.append (data, size);
            chunks.add (info);
        });

        expectEquals (chunks.size(), 1, "Should find one chunk inside LIST");
        expectEquals (chunks[0].fourCC, makeFourCC ("INAM"), "Chunk FourCC should match");
    }

    void testNestedLists()
    {
        beginTest ("Nested LIST Chunks");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (48); // Size
        stream.writeInt (makeFourCC ("TEST"));

        // Write outer LIST
        stream.writeInt (makeFourCC ("LIST"));
        stream.writeInt (36); // Outer LIST size
        stream.writeInt (makeFourCC ("OUT1"));

        // Write inner LIST
        stream.writeInt (makeFourCC ("LIST"));
        stream.writeInt (20); // Inner LIST size
        stream.writeInt (makeFourCC ("IN01"));

        // Write chunk inside inner LIST
        stream.writeInt (makeFourCC ("test"));
        stream.writeInt (4);
        stream.writeInt (0xCCCCCCCC);

        juce::Array<ChunkInfo> chunks;

        RIFFParser parser (riffData, [&chunks] (uint32_t fourCC, const uint8_t* data, uint32_t size)
        {
            ChunkInfo info;
            info.fourCC = fourCC;
            info.data.append (data, size);
            chunks.add (info);
        });

        expectEquals (chunks.size(), 1, "Should find chunk in nested LIST");
        expectEquals (chunks[0].fourCC, makeFourCC ("test"), "Nested chunk FourCC should match");
    }

    void testOddSizedChunks()
    {
        beginTest ("Odd-Sized Chunks with Padding");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (26); // Size
        stream.writeInt (makeFourCC ("TEST"));

        // Write odd-sized chunk (3 bytes)
        stream.writeInt (makeFourCC ("odd "));
        stream.writeInt (3); // Odd size
        stream.writeByte (0xAA);
        stream.writeByte (0xBB);
        stream.writeByte (0xCC);
        stream.writeByte (0x00); // Padding byte

        // Write another chunk after padding
        stream.writeInt (makeFourCC ("next"));
        stream.writeInt (2);
        stream.writeShort (0x1234);

        juce::Array<ChunkInfo> chunks;

        RIFFParser parser (riffData, [&chunks] (uint32_t fourCC, const uint8_t* data, uint32_t size)
        {
            ChunkInfo info;
            info.fourCC = fourCC;
            info.data.append (data, size);
            chunks.add (info);
        });

        expectEquals (chunks.size(), 2, "Should find both chunks despite odd size");
        expectEquals (chunks[0].fourCC, makeFourCC ("odd "), "First chunk FourCC should match");
        expectEquals ((int) chunks[0].data.getSize(), 3, "Odd chunk size should be 3");
        expectEquals (chunks[1].fourCC, makeFourCC ("next"), "Second chunk should be parsed correctly");
    }

    void testEmptyRIFF()
    {
        beginTest ("Empty RIFF");

        juce::MemoryBlock riffData;
        juce::MemoryOutputStream stream (riffData, false);

        // Write RIFF header with no chunks
        stream.writeInt (makeFourCC ("RIFF"));
        stream.writeInt (4); // Just the type, no chunks
        stream.writeInt (makeFourCC ("TEST"));

        int callbackCount = 0;

        RIFFParser parser (riffData, [&callbackCount] (uint32_t, const uint8_t*, uint32_t)
        {
            callbackCount++;
        });

        expectEquals (callbackCount, 0, "Should not call callback for empty RIFF");
    }

    void testInvalidData()
    {
        beginTest ("Invalid Data");

        // Test with data too small for RIFF header
        juce::MemoryBlock smallData;
        smallData.setSize (8); // Less than sizeof(RIFF)

        int callbackCount = 0;

        RIFFParser parser1 (smallData, [&callbackCount] (uint32_t, const uint8_t*, uint32_t)
        {
            callbackCount++;
        });

        expectEquals (callbackCount, 0, "Should not parse data smaller than RIFF header");

        // Test with invalid RIFF signature
        juce::MemoryBlock invalidRiff;
        juce::MemoryOutputStream stream (invalidRiff, false);
        stream.writeInt (makeFourCC ("JUNK")); // Not "RIFF"
        stream.writeInt (4);
        stream.writeInt (makeFourCC ("TEST"));

        callbackCount = 0;

        RIFFParser parser2 (invalidRiff, [&callbackCount] (uint32_t, const uint8_t*, uint32_t)
        {
            callbackCount++;
        });

        expectEquals (callbackCount, 0, "Should not parse data without RIFF signature");
    }
};

static RIFFParserTests riffParserTests;

#endif

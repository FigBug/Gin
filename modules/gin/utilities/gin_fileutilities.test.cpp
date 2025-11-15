/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class FileUtilitiesTests : public juce::UnitTest
{
public:
    FileUtilitiesTests() : juce::UnitTest ("File Utilities", "gin") {}

    void runTest() override
    {
        testOverwriteWithTextASCII();
        testOverwriteWithTextUnicode();
        testOverwriteWithTextLineEndings();
        testOverwriteWithDataMemoryBlock();
        testOverwriteWithDataPointer();
        testOverwriteExistingFile();
        testEmptyFiles();
    }

private:
    void testOverwriteWithTextASCII()
    {
        beginTest ("Overwrite with Text - ASCII");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".txt");

        juce::String testText = "Hello, World!\nThis is a test.";

        expect (overwriteWithText (tempFile, testText, false, false, nullptr),
                "Should write ASCII text successfully");
        expect (tempFile.existsAsFile(), "File should exist");

        juce::String readBack = tempFile.loadFileAsString();
        expectEquals (readBack, testText, "Read text should match written text");

        tempFile.deleteFile();
    }

    void testOverwriteWithTextUnicode()
    {
        beginTest ("Overwrite with Text - Unicode");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".txt");

        juce::String testText = "Hello 世界! Emoji: 🎵";

        expect (overwriteWithText (tempFile, testText, true, true, nullptr),
                "Should write Unicode text successfully");
        expect (tempFile.existsAsFile(), "File should exist");

        juce::String readBack = tempFile.loadFileAsString();
        expectEquals (readBack, testText, "Unicode text should be preserved");

        tempFile.deleteFile();
    }

    void testOverwriteWithTextLineEndings()
    {
        beginTest ("Overwrite with Text - Line Endings");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".txt");

        juce::String testText = "Line 1\nLine 2\nLine 3";

        // Test with CRLF line endings
        expect (overwriteWithText (tempFile, testText, false, false, "\r\n"),
                "Should write with CRLF line endings");

        juce::MemoryBlock fileData;
        tempFile.loadFileAsData (fileData);
        juce::String readBack = fileData.toString();
        expect (readBack.contains ("\r\n"), "Should contain CRLF line endings");

        tempFile.deleteFile();
    }

    void testOverwriteWithDataMemoryBlock()
    {
        beginTest ("Overwrite with Data - MemoryBlock");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".bin");

        juce::MemoryBlock testData;
        testData.append ("Binary data test", 16);
        testData.append ("\0\xFF\xAA\x55", 4);

        expect (overwriteWithData (tempFile, testData),
                "Should write binary data successfully");
        expect (tempFile.existsAsFile(), "File should exist");
        expectEquals (tempFile.getSize(), (juce::int64) testData.getSize(),
                     "File size should match data size");

        juce::MemoryBlock readBack;
        tempFile.loadFileAsData (readBack);
        expect (readBack == testData, "Read data should match written data");

        tempFile.deleteFile();
    }

    void testOverwriteWithDataPointer()
    {
        beginTest ("Overwrite with Data - Pointer");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".bin");

        const char testData[] = "Raw pointer data\0\xFF\xAA";
        size_t dataSize = sizeof (testData);

        expect (overwriteWithData (tempFile, testData, dataSize),
                "Should write data from pointer successfully");
        expect (tempFile.existsAsFile(), "File should exist");
        expectEquals (tempFile.getSize(), (juce::int64) dataSize,
                     "File size should match data size");

        juce::MemoryBlock readBack;
        tempFile.loadFileAsData (readBack);
        expectEquals ((int) readBack.getSize(), (int) dataSize, "Size should match");
        expect (memcmp (readBack.getData(), testData, dataSize) == 0,
                "Read data should match written data");

        tempFile.deleteFile();
    }

    void testOverwriteExistingFile()
    {
        beginTest ("Overwrite Existing File");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".txt");

        // Write initial data (larger)
        juce::String initialText = "This is a long initial text that will be overwritten with something shorter.";
        tempFile.replaceWithText (initialText);

        juce::int64 initialSize = tempFile.getSize();
        expect (initialSize > 0, "Initial file should have content");

        // Overwrite with shorter data - should truncate
        juce::String newText = "Short";
        expect (overwriteWithText (tempFile, newText, false, false, nullptr),
                "Should overwrite successfully");

        juce::int64 newSize = tempFile.getSize();
        expect (newSize < initialSize, "File should be truncated");
        expectEquals (tempFile.loadFileAsString(), newText,
                     "New text should replace old text completely");

        tempFile.deleteFile();
    }

    void testEmptyFiles()
    {
        beginTest ("Empty Files");

        auto tempFile = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("gin_test_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()) + ".txt");

        // Write empty text
        expect (overwriteWithText (tempFile, juce::String(), false, false, nullptr),
                "Should write empty text successfully");
        expect (tempFile.existsAsFile(), "File should exist");
        expectEquals (tempFile.getSize(), (juce::int64) 0, "File should be empty");

        // Write empty binary data
        juce::MemoryBlock emptyData;
        expect (overwriteWithData (tempFile, emptyData),
                "Should write empty data successfully");
        expectEquals (tempFile.getSize(), (juce::int64) 0, "File should be empty");

        // Write zero bytes from pointer
        expect (overwriteWithData (tempFile, nullptr, 0),
                "Should write zero bytes successfully");
        expectEquals (tempFile.getSize(), (juce::int64) 0, "File should be empty");

        tempFile.deleteFile();
    }
};

static FileUtilitiesTests fileUtilitiesTests;

#endif

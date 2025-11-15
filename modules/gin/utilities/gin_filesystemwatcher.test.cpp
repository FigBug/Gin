/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class FileSystemWatcherTests : public juce::UnitTest
{
public:
    FileSystemWatcherTests() : juce::UnitTest ("File System Watcher", "gin") {}

    void runTest() override
    {
#if JUCE_MAC || JUCE_LINUX  // Only test on supported platforms
        testBasicConstruction();
        testFileCreation();
        testFileModification();
        testFileDeletion();
        testFolderChanged();
#endif
    }

private:
#if JUCE_MAC || JUCE_LINUX
    class TestWatcher : public FileSystemWatcher
    {
    public:
        TestWatcher (const juce::File& folder) : FileSystemWatcher (folder) {}

        std::atomic<int> fileChangedCount {0};
        std::atomic<int> folderChangedCount {0};
        FileSystemEvent lastEvent = FileSystemEvent::undefined;
        juce::File lastFile;

        void fileChanged (const juce::File& f, FileSystemEvent fsEvent) override
        {
            fileChangedCount++;
            lastFile = f;
            lastEvent = fsEvent;
        }

        void folderChanged (const juce::File&) override
        {
            folderChangedCount++;
        }
    };

    void testBasicConstruction()
    {
        beginTest ("File System Watcher - Basic Construction");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        TestWatcher watcher (testDir);

        // Just verify it constructs without crashing
        expect (true, "Watcher constructed successfully");

        testDir.deleteRecursively();
    }

    void testFileCreation()
    {
        beginTest ("File System Watcher - File Creation");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        TestWatcher watcher (testDir);

        // Create a file
        auto testFile = testDir.getChildFile ("test.txt");
        testFile.replaceWithText ("test content");

        // Wait for event to be processed
        int timeout = 0;
        while (watcher.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (watcher.fileChangedCount > 0, "Should detect file creation");
        expect (watcher.folderChangedCount > 0, "Should detect folder change");

        testDir.deleteRecursively();
    }

    void testFileModification()
    {
        beginTest ("File System Watcher - File Modification");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        auto testFile = testDir.getChildFile ("test.txt");
        testFile.replaceWithText ("initial content");

        // Wait a bit to let initial creation settle
        juce::Thread::sleep (100);

        TestWatcher watcher (testDir);

        // Reset counters after construction
        juce::Thread::sleep (100);
        watcher.fileChangedCount = 0;
        watcher.folderChangedCount = 0;

        // Modify the file
        testFile.replaceWithText ("modified content");

        // Wait for event
        int timeout = 0;
        while (watcher.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (watcher.fileChangedCount > 0, "Should detect file modification");

        testDir.deleteRecursively();
    }

    void testFileDeletion()
    {
        beginTest ("File System Watcher - File Deletion");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        auto testFile = testDir.getChildFile ("test.txt");
        testFile.replaceWithText ("content");

        // Wait for initial creation to settle
        juce::Thread::sleep (100);

        TestWatcher watcher (testDir);

        // Reset counters
        juce::Thread::sleep (100);
        watcher.fileChangedCount = 0;

        // Delete the file
        testFile.deleteFile();

        // Wait for event
        int timeout = 0;
        while (watcher.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (watcher.fileChangedCount > 0, "Should detect file deletion");

        testDir.deleteRecursively();
    }

    void testFolderChanged()
    {
        beginTest ("File System Watcher - Folder Changed Callback");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        TestWatcher watcher (testDir);

        // Create a file to trigger folder change
        auto testFile = testDir.getChildFile ("trigger.txt");
        testFile.replaceWithText ("trigger");

        // Wait for folder changed event
        int timeout = 0;
        while (watcher.folderChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (watcher.folderChangedCount > 0, "Should call folderChanged callback");

        testDir.deleteRecursively();
    }
#endif
};

static FileSystemWatcherTests fileSystemWatcherTests;

#endif

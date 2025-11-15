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
        juce::ScopedJuceInitialiser_GUI initialiser;

        testBasicConstruction();
        testFileCreation();
        testFileModification();
        testFileDeletion();
        testFolderChanged();
#endif
    }

private:
#if JUCE_MAC || JUCE_LINUX
    class TestListener : public FileSystemWatcher::Listener
    {
    public:
        std::atomic<int> fileChangedCount {0};
        std::atomic<int> folderChangedCount {0};
        FileSystemWatcher::FileSystemEvent lastEvent = FileSystemWatcher::FileSystemEvent::undefined;
        juce::File lastFile;

        void fileChanged (const juce::File& f, FileSystemWatcher::FileSystemEvent fsEvent) override
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

        FileSystemWatcher watcher;
        watcher.addFolder (testDir);

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

        FileSystemWatcher watcher;
        TestListener listener;
        watcher.addFolder (testDir);
        watcher.addListener (&listener);

        // Create a file
        auto testFile = testDir.getChildFile ("test.txt");
        testFile.replaceWithText ("test content");

        // Wait for event to be processed
        int timeout = 0;
        while (listener.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (listener.fileChangedCount > 0, "Should detect file creation");
        expect (listener.folderChangedCount > 0, "Should detect folder change");

        watcher.removeListener (&listener);
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

        FileSystemWatcher watcher;
        TestListener listener;
        watcher.addFolder (testDir);
        watcher.addListener (&listener);

        // Reset counters after construction
        juce::Thread::sleep (100);
        listener.fileChangedCount = 0;
        listener.folderChangedCount = 0;

        // Modify the file
        testFile.replaceWithText ("modified content");

        // Wait for event
        int timeout = 0;
        while (listener.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (listener.fileChangedCount > 0, "Should detect file modification");

        watcher.removeListener (&listener);
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

        FileSystemWatcher watcher;
        TestListener listener;
        watcher.addFolder (testDir);
        watcher.addListener (&listener);

        // Reset counters
        juce::Thread::sleep (100);
        listener.fileChangedCount = 0;

        // Delete the file
        testFile.deleteFile();

        // Wait for event
        int timeout = 0;
        while (listener.fileChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (listener.fileChangedCount > 0, "Should detect file deletion");

        watcher.removeListener (&listener);
        testDir.deleteRecursively();
    }

    void testFolderChanged()
    {
        beginTest ("File System Watcher - Folder Changed Callback");

        auto testDir = juce::File::getSpecialLocation (juce::File::tempDirectory)
            .getChildFile ("GinFSWTest_" + juce::String::toHexString (juce::Random::getSystemRandom().nextInt()));

        testDir.createDirectory();

        FileSystemWatcher watcher;
        TestListener listener;
        watcher.addFolder (testDir);
        watcher.addListener (&listener);

        // Create a file to trigger folder change
        auto testFile = testDir.getChildFile ("trigger.txt");
        testFile.replaceWithText ("trigger");

        // Wait for folder changed event
        int timeout = 0;
        while (listener.folderChangedCount == 0 && timeout < 2000)
        {
            juce::MessageManager::getInstance()->runDispatchLoopUntil (10);
            timeout++;
        }

        expect (listener.folderChangedCount > 0, "Should call folderChanged callback");

        watcher.removeListener (&listener);
        testDir.deleteRecursively();
    }
#endif
};

static FileSystemWatcherTests fileSystemWatcherTests;

#endif

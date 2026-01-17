/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
#if GIN_UNIT_TESTS

class MidiFilePlayerTests : public juce::UnitTest
{
public:
    MidiFilePlayerTests() : juce::UnitTest ("MIDI File Player", "gin_dsp") {}

    void runTest() override
    {
        testDefaultConstruction();
        testPlayStop();
        testLooping();
        testSampleRate();
        testFallbackBpm();
        testPlayheadPosition();
        testLoadMidiFile();
        testProcessBlock();
        testProcessBlockWithNotes();
        testLoopingPlayback();
        testClear();
        testPopulatePositionInfo();

        // Cleanup
        for (auto& f : tempFiles)
            f.deleteFile();
        tempFiles.clear();
    }

private:
    juce::Array<juce::File> tempFiles;

    juce::File createTestMidiFile (const juce::String& filename, int numNotes = 4, double noteDurationTicks = 480.0)
    {
        auto tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory);
        auto file = tempDir.getChildFile (filename);
        file.deleteFile();

        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote (480);

        juce::MidiMessageSequence track;

        // Add tempo event (120 BPM = 500000 microseconds per quarter note)
        auto tempoEvent = juce::MidiMessage::tempoMetaEvent (500000);
        tempoEvent.setTimeStamp (0.0);
        track.addEvent (tempoEvent);

        // Add notes
        for (int i = 0; i < numNotes; ++i)
        {
            double startTick = i * noteDurationTicks;
            double endTick = startTick + noteDurationTicks - 1;

            auto noteOn = juce::MidiMessage::noteOn (1, 60 + i, 0.8f);
            noteOn.setTimeStamp (startTick);
            track.addEvent (noteOn);

            auto noteOff = juce::MidiMessage::noteOff (1, 60 + i);
            noteOff.setTimeStamp (endTick);
            track.addEvent (noteOff);
        }

        track.updateMatchedPairs();
        midiFile.addTrack (track);

        juce::FileOutputStream stream (file);
        if (stream.openedOk())
            midiFile.writeTo (stream);

        tempFiles.add (file);
        return file;
    }

    void testDefaultConstruction()
    {
        beginTest ("Default Construction");

        MidiFilePlayer player;

        expect (! player.isPlaying(), "Should not be playing initially");
        expect (! player.isLooping(), "Should not be looping by default");
        expect (! player.hasFileLoaded(), "Should not have file loaded initially");
        expectEquals (player.getPlayheadPosition(), 0.0, "Playhead should be at 0");
        expectEquals (player.getLengthInSeconds(), 0.0, "Length should be 0");
    }

    void testPlayStop()
    {
        beginTest ("Play and Stop");

        MidiFilePlayer player;

        expect (! player.isPlaying(), "Should not be playing initially");

        player.play();
        expect (player.isPlaying(), "Should be playing after play()");

        player.stop();
        expect (! player.isPlaying(), "Should not be playing after stop()");
    }

    void testLooping()
    {
        beginTest ("Looping");

        MidiFilePlayer player;

        expect (! player.isLooping(), "Should not be looping by default");

        player.setLooping (true);
        expect (player.isLooping(), "Should be looping after setLooping(true)");

        player.setLooping (false);
        expect (! player.isLooping(), "Should not be looping after setLooping(false)");
    }

    void testSampleRate()
    {
        beginTest ("Sample Rate");

        MidiFilePlayer player;

        player.setSampleRate (48000.0);
        // Sample rate is used internally; verify by checking it doesn't crash

        player.setSampleRate (44100.0);
    }

    void testFallbackBpm()
    {
        beginTest ("Fallback BPM");

        MidiFilePlayer player;

        player.setFallbackBpm (140.0);
        // Fallback BPM is used when file has no tempo events

        player.setFallbackBpm (120.0);
    }

    void testPlayheadPosition()
    {
        beginTest ("Playhead Position");

        MidiFilePlayer player;

        expectEquals (player.getPlayheadPosition(), 0.0, "Initial playhead should be 0");

        player.setPlayheadPosition (1.5);
        // Note: setPlayheadPosition sets a pending seek, actual position updates in processBlock
    }

    void testLoadMidiFile()
    {
        beginTest ("Load MIDI File");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);

        auto file = createTestMidiFile ("test_load.mid");

        bool loaded = player.load (file);
        expect (loaded, "Should load successfully");
        expect (player.hasFileLoaded(), "Should have file loaded");
        expect (player.getLengthInSeconds() > 0.0, "Length should be > 0");
        expect (player.getLoadedFile() == file, "Should report correct loaded file");
    }

    void testProcessBlock()
    {
        beginTest ("Process Block");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);

        auto file = createTestMidiFile ("test_process.mid");
        player.load (file);
        player.play();

        juce::MidiBuffer midiBuffer;
        player.processBlock (512, midiBuffer);

        expect (player.getPlayheadPosition() > 0.0, "Playhead should advance");
    }

    void testProcessBlockWithNotes()
    {
        beginTest ("Process Block With Notes");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);

        // Create a file with notes starting immediately
        auto file = createTestMidiFile ("test_notes.mid", 1, 480.0);
        player.load (file);
        player.play();

        // Process enough samples to trigger the first note
        // At 120 BPM, 480 ticks = 1 quarter note = 0.5 seconds = 22050 samples at 44100 Hz
        juce::MidiBuffer midiBuffer;
        int noteOnCount = 0;

        // Process in chunks until we get some MIDI events
        for (int i = 0; i < 100 && noteOnCount == 0; ++i)
        {
            midiBuffer.clear();
            player.processBlock (512, midiBuffer);

            for (const auto metadata : midiBuffer)
            {
                if (metadata.getMessage().isNoteOn())
                    noteOnCount++;
            }
        }

        expect (noteOnCount > 0, "Should have received note on events");
    }

    void testLoopingPlayback()
    {
        beginTest ("Looping Playback");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);
        player.setLooping (true);

        // Create a very short MIDI file
        auto file = createTestMidiFile ("test_loop.mid", 1, 240.0);
        player.load (file);
        player.play();

        double length = player.getLengthInSeconds();
        expect (length > 0.0, "Should have a length");

        // Process enough to go past the end
        juce::MidiBuffer midiBuffer;
        double processedSeconds = 0.0;
        int iterations = 0;
        const int maxIterations = 1000;

        while (processedSeconds < length * 2 && iterations < maxIterations)
        {
            midiBuffer.clear();
            player.processBlock (512, midiBuffer);
            processedSeconds += 512.0 / 44100.0;
            iterations++;
        }

        expect (player.isPlaying(), "Should still be playing (looping)");
    }

    void testClear()
    {
        beginTest ("Clear");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);

        auto file = createTestMidiFile ("test_clear.mid");
        player.load (file);
        player.play();

        expect (player.hasFileLoaded(), "Should have file loaded");
        expect (player.isPlaying(), "Should be playing");

        player.clear();

        expect (! player.hasFileLoaded(), "Should not have file loaded after clear");
        expect (! player.isPlaying(), "Should not be playing after clear");
        expectEquals (player.getLengthInSeconds(), 0.0, "Length should be 0 after clear");
    }

    void testPopulatePositionInfo()
    {
        beginTest ("Populate Position Info");

        MidiFilePlayer player;
        player.setSampleRate (44100.0);
        player.setLooping (true);

        auto file = createTestMidiFile ("test_position_info.mid");
        player.load (file);
        player.play();

        // Process a bit to advance playhead
        juce::MidiBuffer midiBuffer;
        player.processBlock (4410, midiBuffer);

        auto info = player.populatePositionInfo();

        expect (info.getIsPlaying(), "Should report playing");
        expect (info.getIsLooping(), "Should report looping");
        expect (! info.getIsRecording(), "Should not report recording");

        auto timeInSeconds = info.getTimeInSeconds();
        expect (timeInSeconds.hasValue(), "Should have time in seconds");
        if (timeInSeconds.hasValue())
            expect (*timeInSeconds > 0.0, "Time should be > 0");

        auto bpm = info.getBpm();
        expect (bpm.hasValue(), "Should have BPM");
        if (bpm.hasValue())
            expectWithinAbsoluteError (*bpm, 120.0, 1.0, "BPM should be ~120");
    }
};

static MidiFilePlayerTests midiFilePlayerTests;

#endif

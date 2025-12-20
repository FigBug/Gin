/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Plays back MIDI files with configurable tempo.

    MidiFilePlayer loads a standard MIDI file and plays it back through
    processBlock calls, filling a MidiBuffer with the appropriate events
    for each audio block based on the current playhead position.

    Thread Safety:
    - load(), setBpm() should be called from the message thread
    - processBlock() is called from the audio thread
    - play(), stop(), setPlayheadPosition() can be called from either thread
    - Getters are safe to call from any thread

    Key Features:
    - Load MIDI files from juce::File
    - Configurable BPM (tempo)
    - Sample-accurate playback
    - Play/stop controls with proper note-off handling
    - Playhead position tracking

    Usage:
    @code
    MidiFilePlayer player;
    player.load(midiFile);
    player.setSampleRate(44100.0);
    player.setBpm(120.0);
    player.play();

    // In audio callback
    juce::MidiBuffer midiBuffer;
    player.processBlock(numSamples, midiBuffer);
    @endcode

    @see juce::MidiFile, juce::MidiBuffer
*/
class MidiFilePlayer
{
public:
    MidiFilePlayer() = default;

    /** Loads a MIDI file for playback.
        Call from the message thread only.
        @param file The MIDI file to load
        @return true if the file was loaded successfully
    */
    bool load (const juce::File& file);

    /** Sets the sample rate for playback timing.
        @param sr The sample rate in Hz
    */
    void setSampleRate (double sr);

    /** Sets the playback tempo.
        Call from the message thread only.
        @param bpm The tempo in beats per minute
    */
    void setBpm (double bpm);

    /** Starts playback from the current position. */
    void play();

    /** Stops playback and sends note-off messages for any playing notes. */
    void stop();

    /** Returns true if currently playing. */
    bool isPlaying() const { return playing.load(); }

    /** Returns the current playhead position in seconds. */
    double getPlayheadPosition() const { return playheadSeconds.load(); }

    /** Sets the playhead position in seconds.
        @param positionInSeconds The new playhead position
    */
    void setPlayheadPosition (double positionInSeconds);

    /** Processes a block, filling the buffer with MIDI events.
        Call from the audio thread only.
        @param numSamples The number of samples in this block
        @param midiBuffer The buffer to fill with MIDI events
    */
    void processBlock (int numSamples, juce::MidiBuffer& midiBuffer);

    /** Returns the total length of the MIDI file in seconds. */
    double getLengthInSeconds() const { return lengthInSeconds.load(); }

    /** Sets whether playback should loop. */
    void setLooping (bool shouldLoop) { looping.store (shouldLoop); }

    /** Returns true if looping is enabled. */
    bool isLooping() const { return looping.load(); }

    /** Returns the merged MIDI sequence for display purposes.
        Note: Only safe to call when not loading a new file.
    */
    const juce::MidiMessageSequence& getSequence() const { return sequence; }

    /** Returns the length in ticks (for display purposes). */
    double getLengthInTicks() const { return lengthInTicks.load(); }

    /** Returns true if a file has been loaded. */
    bool hasFileLoaded() const { return fileLoaded.load(); }

    /** Returns the path of the currently loaded file, or empty if none. */
    juce::File getLoadedFile() const { return loadedFilePath; }

    /** Clears the currently loaded file. */
    void clear();

private:
    void buildSequence();
    void performSeek (double positionInSeconds, juce::MidiBuffer& midiBuffer);
    void sendAllNotesOff (juce::MidiBuffer& midiBuffer, int samplePosition);

    juce::SpinLock lock;

    juce::File loadedFilePath;
    juce::MidiFile midiFile;
    juce::MidiMessageSequence sequence;

    std::atomic<double> sampleRate { 44100.0 };
    std::atomic<double> bpm { 120.0 };
    std::atomic<double> playheadSeconds { 0.0 };
    std::atomic<double> lengthInSeconds { 0.0 };
    std::atomic<double> lengthInTicks { 0.0 };

    std::atomic<int> currentEventIndex { 0 };
    std::atomic<bool> playing { false };
    std::atomic<bool> looping { false };
    std::atomic<bool> fileLoaded { false };
    std::atomic<bool> pendingStop { false };
    std::atomic<bool> pendingSeek { false };
    std::atomic<double> seekTargetSeconds { 0.0 };

    // Track which notes are currently on (for proper note-off on stop)
    std::set<std::pair<int, int>> activeNotes;  // channel, note

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiFilePlayer)
};

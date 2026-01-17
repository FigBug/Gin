/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Displays the contents of a MidiFilePlayer as a piano roll view.

    MidiFileComponent renders MIDI notes from a MidiFilePlayer as horizontal
    bars on a piano roll style display. It also shows a playhead indicating
    the current playback position.

    Key Features:
    - Piano roll style MIDI visualization
    - Playhead display
    - Automatic refresh during playback
    - Customizable colors via ColourIds

    Usage:
    @code
    MidiFilePlayer player;
    player.load(midiFile);

    MidiFileComponent display(player);
    addAndMakeVisible(display);
    @endcode

    @see MidiFilePlayer
*/
class MidiFileComponent : public juce::Component,
                          public juce::Timer,
                          public juce::FileDragAndDropTarget
{
public:
    MidiFileComponent (MidiFilePlayer& player);
    ~MidiFileComponent() override;

    enum ColourIds
    {
        backgroundColourId   = 0x1292e10,
        noteColourId         = 0x1292e11,
        playheadColourId     = 0x1292e12,
        outlineColourId      = 0x1292e13,
        dropHighlightColourId = 0x1292e14,
    };

    void paint (juce::Graphics& g) override;
    void timerCallback() override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void fileDragEnter (const juce::StringArray& files, int x, int y) override;
    void fileDragExit (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;

    /** Called when a file is loaded via drag & drop. */
    std::function<void()> onFileDropped;

    /** Called when playback state changes (started, stopped, or reached end). */
    std::function<void()> onPlaybackStateChanged;

private:
    MidiFilePlayer& midiPlayer;
    bool isDragOver = false;
    bool wasPlaying = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiFileComponent)
};

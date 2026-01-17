/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

MidiFileComponent::MidiFileComponent (MidiFilePlayer& player)
    : midiPlayer (player)
{
    setColour (backgroundColourId, juce::Colours::black);
    setColour (noteColourId, juce::Colours::white);
    setColour (playheadColourId, juce::Colours::red);
    setColour (outlineColourId, juce::Colours::grey);
    setColour (dropHighlightColourId, juce::Colours::white.withAlpha (0.2f));

    startTimerHz (30);
}

MidiFileComponent::~MidiFileComponent()
{
    stopTimer();
}

void MidiFileComponent::timerCallback()
{
    const bool isPlaying = midiPlayer.isPlaying();

    // Detect playback state change (e.g., stopped at end)
    if (wasPlaying && ! isPlaying)
    {
        if (onPlaybackStateChanged)
            onPlaybackStateChanged();
    }

    if (isPlaying || wasPlaying != isPlaying)
        repaint();

    wasPlaying = isPlaying;
}

bool MidiFileComponent::isInterestedInFileDrag (const juce::StringArray& files)
{
    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase (".mid") || file.endsWithIgnoreCase (".midi"))
            return true;
    }
    return false;
}

void MidiFileComponent::fileDragEnter (const juce::StringArray&, int, int)
{
    isDragOver = true;
    repaint();
}

void MidiFileComponent::fileDragExit (const juce::StringArray&)
{
    isDragOver = false;
    repaint();
}

void MidiFileComponent::filesDropped (const juce::StringArray& files, int, int)
{
    isDragOver = false;

    for (const auto& file : files)
    {
        if (file.endsWithIgnoreCase (".mid") || file.endsWithIgnoreCase (".midi"))
        {
            if (midiPlayer.load (juce::File (file)))
            {
                if (onFileDropped)
                    onFileDropped();
            }
            break;
        }
    }

    repaint();
}

void MidiFileComponent::paint (juce::Graphics& g)
{
    g.fillAll (findColour (backgroundColourId));

    const auto& sequence = midiPlayer.getSequence();

    if (midiPlayer.hasFileLoaded() && sequence.getNumEvents() > 0)
    {
        g.setColour (findColour (noteColourId));

        const double lengthInTicks = midiPlayer.getLengthInTicks();
        const double lengthInSeconds = midiPlayer.getLengthInSeconds();
        if (lengthInTicks <= 0.0 || lengthInSeconds <= 0.0)
            return;

        const float w = static_cast<float> (getWidth());
        const float h = static_cast<float> (getHeight());

        // Find note range to scale display
        int minNote = 127;
        int maxNote = 0;

        for (int i = 0; i < sequence.getNumEvents(); ++i)
        {
            auto* evt = sequence.getEventPointer (i);
            if (evt->message.isNoteOn())
            {
                const int note = evt->message.getNoteNumber();
                minNote = std::min (minNote, note);
                maxNote = std::max (maxNote, note);
            }
        }

        // Add some padding to the note range
        minNote = std::max (0, minNote - 2);
        maxNote = std::min (127, maxNote + 2);
        const float noteRange = static_cast<float> (maxNote - minNote + 1);
        const float noteHeight = std::max (1.0f, h / noteRange);

        // Draw notes (timestamps are in ticks)
        for (int i = 0; i < sequence.getNumEvents(); ++i)
        {
            auto* evtOn = sequence.getEventPointer (i);

            if (evtOn->message.isNoteOn() && evtOn->noteOffObject != nullptr)
            {
                auto* evtOff = evtOn->noteOffObject;

                const float x1 = static_cast<float> (evtOn->message.getTimeStamp() / lengthInTicks) * w;
                const float x2 = std::max (x1 + 1.0f, static_cast<float> (evtOff->message.getTimeStamp() / lengthInTicks) * w);

                const int note = evtOn->message.getNoteNumber();
                const float y = (1.0f - static_cast<float> (note - minNote) / noteRange) * h - noteHeight * 0.5f;

                g.fillRect (x1, y, x2 - x1, noteHeight);
            }
        }

        // Draw playhead (playhead position is in seconds)
        if (midiPlayer.isPlaying() || midiPlayer.getPlayheadPosition() > 0.0)
        {
            const float playheadX = static_cast<float> (midiPlayer.getPlayheadPosition() / lengthInSeconds) * w;

            g.setColour (juce::Colours::black.withAlpha (0.5f));
            g.fillRect (playheadX - 0.5f, 0.0f, 3.0f, h);

            g.setColour (findColour (playheadColourId).brighter());
            g.fillRect (playheadX, 0.0f, 2.0f, h);
        }
    }
    else
    {
        g.setColour (findColour (noteColourId).withAlpha (0.5f));
        g.setFont (12.0f);
        g.drawFittedText ("No MIDI File Loaded", getLocalBounds(), juce::Justification::centred, 1);
    }

    // Draw drop highlight overlay
    if (isDragOver)
    {
        g.setColour (findColour (dropHighlightColourId));
        g.fillAll();
    }

    g.setColour (findColour (outlineColourId));
    g.drawRect (getLocalBounds());
}

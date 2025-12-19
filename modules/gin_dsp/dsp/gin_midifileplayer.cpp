/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2025 - Roland Rabien.

 ==============================================================================
 */

bool MidiFilePlayer::load (const juce::File& file)
{
    juce::FileInputStream stream (file);

    if (! stream.openedOk())
        return false;

    juce::MidiFile newMidiFile;
    if (! newMidiFile.readFrom (stream))
        return false;

    // Stop playback - pendingStop will send note-offs in the next processBlock
    // before activeNotes is accessed, since we use ScopedTryLock in processBlock
    pendingStop.store (true);
    playing.store (false);

    {
        const juce::SpinLock::ScopedLockType sl (lock);

        loadedFilePath = file;
        midiFile = std::move (newMidiFile);
        fileLoaded.store (true);

        buildSequence();
    }

    return true;
}

void MidiFilePlayer::buildSequence()
{
    // Called with lock held

    sequence.clear();

    // Merge all tracks into a single sequence
    for (int i = 0; i < midiFile.getNumTracks(); ++i)
    {
        const auto* track = midiFile.getTrack (i);
        if (track != nullptr)
            sequence.addSequence (*track, 0.0);
    }

    sequence.sort();
    sequence.updateMatchedPairs();

    // Calculate length - find the last event timestamp
    double newLength = 0.0;
    double newLengthTicks = 0.0;

    if (sequence.getNumEvents() > 0)
    {
        auto lastEvent = sequence.getEventPointer (sequence.getNumEvents() - 1);
        if (lastEvent != nullptr)
        {
            newLengthTicks = lastEvent->message.getTimeStamp();
            const short timeFormat = midiFile.getTimeFormat();

            if (timeFormat > 0)
            {
                // Ticks per beat (quarter note)
                const double ticksPerBeat = static_cast<double> (timeFormat);
                const double beatsPerSecond = bpm.load() / 60.0;
                const double ticksPerSecond = ticksPerBeat * beatsPerSecond;
                newLength = newLengthTicks / ticksPerSecond;
            }
            else
            {
                // SMPTE format - frames per second
                newLength = newLengthTicks;
            }
        }
    }

    lengthInSeconds.store (newLength);
    lengthInTicks.store (newLengthTicks);
    currentEventIndex.store (0);
    playheadSeconds.store (0.0);
    // Don't clear activeNotes here - pendingStop will handle sending note-offs
    // and clearing activeNotes in processBlock
}

void MidiFilePlayer::setSampleRate (double sr)
{
    sampleRate.store (sr);
}

void MidiFilePlayer::setBpm (double newBpm)
{
    if (! juce::approximatelyEqual (bpm.load(), newBpm))
    {
        const juce::SpinLock::ScopedLockType sl (lock);

        bpm.store (newBpm);

        // Recalculate length with new tempo
        if (fileLoaded.load())
            buildSequence();
    }
}

void MidiFilePlayer::clear()
{
    playing.store (false);
    pendingStop.store (true);

    {
        const juce::SpinLock::ScopedLockType sl (lock);

        loadedFilePath = juce::File();
        midiFile = juce::MidiFile();
        sequence.clear();
        fileLoaded.store (false);
        lengthInSeconds.store (0.0);
        lengthInTicks.store (0.0);
        currentEventIndex.store (0);
        playheadSeconds.store (0.0);
        activeNotes.clear();
    }
}

void MidiFilePlayer::play()
{
    playing.store (true);
}

void MidiFilePlayer::stop()
{
    pendingStop.store (true);
    playing.store (false);
}

void MidiFilePlayer::setPlayheadPosition (double positionInSeconds)
{
    seekTargetSeconds.store (positionInSeconds);
    pendingSeek.store (true);
}

void MidiFilePlayer::performSeek (double positionInSeconds, juce::MidiBuffer& midiBuffer)
{
    // Called with lock held, from processBlock

    // Send note-offs for any active notes
    sendAllNotesOff (midiBuffer, 0);

    playheadSeconds.store (positionInSeconds);

    // Find the event index for this position
    int newEventIndex = 0;

    if (! fileLoaded.load())
    {
        currentEventIndex.store (0);
        return;
    }

    const short timeFormat = midiFile.getTimeFormat();
    if (timeFormat <= 0)
    {
        currentEventIndex.store (0);
        return;
    }

    const double ticksPerBeat = static_cast<double> (timeFormat);
    const double beatsPerSecond = bpm.load() / 60.0;
    const double ticksPerSecond = ticksPerBeat * beatsPerSecond;
    const double targetTicks = positionInSeconds * ticksPerSecond;

    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        auto* event = sequence.getEventPointer (i);
        if (event != nullptr && event->message.getTimeStamp() >= targetTicks)
        {
            newEventIndex = i;
            break;
        }
        newEventIndex = i + 1;
    }

    currentEventIndex.store (newEventIndex);

    // If playing, trigger any notes that should be sounding at this position
    if (playing.load())
    {
        for (int i = 0; i < sequence.getNumEvents(); ++i)
        {
            auto* event = sequence.getEventPointer (i);
            if (event == nullptr)
                continue;

            // Only look at note-ons before our position
            if (event->message.getTimeStamp() >= targetTicks)
                break;

            if (event->message.isNoteOn() && event->message.getVelocity() > 0 && event->noteOffObject != nullptr)
            {
                // Check if this note's off event is after our target position
                if (event->noteOffObject->message.getTimeStamp() > targetTicks)
                {
                    // This note should be sounding - trigger it
                    midiBuffer.addEvent (event->message, 0);
                    activeNotes.insert ({ event->message.getChannel(), event->message.getNoteNumber() });
                }
            }
        }
    }
}

void MidiFilePlayer::sendAllNotesOff (juce::MidiBuffer& midiBuffer, int samplePosition)
{
    for (const auto& note : activeNotes)
    {
        auto noteOff = juce::MidiMessage::noteOff (note.first, note.second);
        midiBuffer.addEvent (noteOff, samplePosition);
    }
    activeNotes.clear();
}

void MidiFilePlayer::processBlock (int numSamples, juce::MidiBuffer& midiBuffer)
{
    if (numSamples <= 0)
        return;

    const juce::SpinLock::ScopedTryLockType sl (lock);
    if (! sl.isLocked())
        return;  // Skip this block if we can't get the lock (loading in progress)

    // Handle pending stop - send note offs (must be inside lock since activeNotes is protected)
    if (pendingStop.exchange (false))
        sendAllNotesOff (midiBuffer, 0);

    // Handle pending seek
    if (pendingSeek.exchange (false))
        performSeek (seekTargetSeconds.load(), midiBuffer);

    if (! playing.load() || ! fileLoaded.load())
        return;

    const short timeFormat = midiFile.getTimeFormat();
    if (timeFormat <= 0)
        return;

    const double currentBpm = bpm.load();
    const double currentSampleRate = sampleRate.load();
    const double ticksPerBeat = static_cast<double> (timeFormat);
    const double beatsPerSecond = currentBpm / 60.0;
    const double ticksPerSecond = ticksPerBeat * beatsPerSecond;
    const double secondsPerSample = 1.0 / currentSampleRate;

    const double blockStartSeconds = playheadSeconds.load();
    const double blockEndSeconds = blockStartSeconds + (numSamples * secondsPerSample);

    const double blockStartTicks = blockStartSeconds * ticksPerSecond;
    const double blockEndTicks = blockEndSeconds * ticksPerSecond;

    int eventIndex = currentEventIndex.load();

    // Process events that fall within this block
    while (eventIndex < sequence.getNumEvents())
    {
        auto* event = sequence.getEventPointer (eventIndex);
        if (event == nullptr)
            break;

        const double eventTicks = event->message.getTimeStamp();

        if (eventTicks >= blockEndTicks)
            break;

        if (eventTicks >= blockStartTicks)
        {
            // Calculate sample position within this block
            const double eventSeconds = eventTicks / ticksPerSecond;
            const double offsetSeconds = eventSeconds - blockStartSeconds;
            const int samplePos = std::max (0, std::min (numSamples - 1,
                static_cast<int> (offsetSeconds * currentSampleRate)));

            // Don't add meta events to the buffer
            if (! event->message.isMetaEvent())
            {
                midiBuffer.addEvent (event->message, samplePos);

                // Track note on/off for proper cleanup on stop
                // Note: isNoteOn() returns true even for velocity 0, which is actually a note-off
                if (event->message.isNoteOn() && event->message.getVelocity() > 0)
                    activeNotes.insert ({ event->message.getChannel(), event->message.getNoteNumber() });
                else if (event->message.isNoteOff() || (event->message.isNoteOn() && event->message.getVelocity() == 0))
                    activeNotes.erase ({ event->message.getChannel(), event->message.getNoteNumber() });
            }
        }

        eventIndex++;
    }

    currentEventIndex.store (eventIndex);
    playheadSeconds.store (blockEndSeconds);

    // Handle looping or stopping at end
    if (blockEndSeconds >= lengthInSeconds.load())
    {
        if (looping.load())
        {
            sendAllNotesOff (midiBuffer, numSamples - 1);
            playheadSeconds.store (0.0);
            currentEventIndex.store (0);
            activeNotes.clear();
        }
        else
        {
            sendAllNotesOff (midiBuffer, numSamples - 1);
            playing.store (false);
        }
    }
}

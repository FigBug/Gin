/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

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

    // Build tempo map from the merged sequence
    buildTempoMap();

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
                // Use tempo map for accurate conversion
                newLength = ticksToSeconds (newLengthTicks);
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

void MidiFilePlayer::buildTempoMap()
{
    // Called with lock held
    tempoMap.clear();

    // Extract tempo events from the sequence
    for (int i = 0; i < sequence.getNumEvents(); ++i)
    {
        auto* event = sequence.getEventPointer (i);
        if (event != nullptr && event->message.isTempoMetaEvent())
        {
            double tickPos = event->message.getTimeStamp();
            double microsecondsPerQuarterNote = event->message.getTempoSecondsPerQuarterNote() * 1000000.0;
            tempoMap.push_back ({ tickPos, microsecondsPerQuarterNote });
        }
    }

    // If no tempo events found, use the fallback BPM
    if (tempoMap.empty())
    {
        double microsecondsPerQuarterNote = 60000000.0 / fallbackBpm.load();
        tempoMap.push_back ({ 0.0, microsecondsPerQuarterNote });
    }
}

double MidiFilePlayer::ticksToSeconds (double ticks) const
{
    // Called with lock held (or during initialization)
    const short timeFormat = midiFile.getTimeFormat();
    if (timeFormat <= 0)
        return ticks;  // SMPTE format, ticks are already time-based

    const double ticksPerQuarterNote = static_cast<double> (timeFormat);
    double seconds = 0.0;
    double lastTick = 0.0;

    for (size_t i = 0; i < tempoMap.size(); ++i)
    {
        double tempoStartTick = tempoMap[i].tickPosition;
        double microsecondsPerQuarterNote = tempoMap[i].microsecondsPerQuarterNote;

        // Find where this tempo region ends
        double tempoEndTick = (i + 1 < tempoMap.size()) ? tempoMap[i + 1].tickPosition : ticks;
        if (tempoEndTick > ticks)
            tempoEndTick = ticks;

        if (tempoEndTick <= lastTick)
            continue;

        // Calculate time for the portion of this tempo region up to our target
        double regionStartTick = std::max (lastTick, tempoStartTick);
        double ticksInRegion = tempoEndTick - regionStartTick;

        double secondsPerTick = (microsecondsPerQuarterNote / 1000000.0) / ticksPerQuarterNote;
        seconds += ticksInRegion * secondsPerTick;

        lastTick = tempoEndTick;

        if (lastTick >= ticks)
            break;
    }

    return seconds;
}

double MidiFilePlayer::secondsToTicks (double seconds) const
{
    // Called with lock held
    const short timeFormat = midiFile.getTimeFormat();
    if (timeFormat <= 0)
        return seconds;  // SMPTE format, ticks are already time-based

    const double ticksPerQuarterNote = static_cast<double> (timeFormat);
    double ticks = 0.0;
    double accumulatedSeconds = 0.0;

    for (size_t i = 0; i < tempoMap.size(); ++i)
    {
        double tempoStartTick = tempoMap[i].tickPosition;
        double microsecondsPerQuarterNote = tempoMap[i].microsecondsPerQuarterNote;
        double secondsPerTick = (microsecondsPerQuarterNote / 1000000.0) / ticksPerQuarterNote;

        // Calculate seconds at start of next tempo region (or end of file)
        double nextTempoTick = (i + 1 < tempoMap.size()) ? tempoMap[i + 1].tickPosition : std::numeric_limits<double>::max();
        double ticksInRegion = nextTempoTick - tempoStartTick;
        double secondsInRegion = ticksInRegion * secondsPerTick;

        if (accumulatedSeconds + secondsInRegion >= seconds)
        {
            // Target is in this region
            double secondsIntoRegion = seconds - accumulatedSeconds;
            ticks = tempoStartTick + (secondsIntoRegion / secondsPerTick);
            return ticks;
        }

        accumulatedSeconds += secondsInRegion;
        ticks = nextTempoTick;
    }

    return ticks;
}

void MidiFilePlayer::setSampleRate (double sr)
{
    sampleRate.store (sr);
}

void MidiFilePlayer::setFallbackBpm (double newBpm)
{
    if (! juce::approximatelyEqual (fallbackBpm.load(), newBpm))
    {
        const juce::SpinLock::ScopedLockType sl (lock);

        fallbackBpm.store (newBpm);

        // Recalculate tempo map and length if no tempo events in file
        if (fileLoaded.load() && tempoMap.empty())
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
        tempoMap.clear();
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

    // Use tempo map for accurate conversion
    const double targetTicks = secondsToTicks (positionInSeconds);

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

juce::AudioPlayHead::PositionInfo MidiFilePlayer::populatePositionInfo()
{
    juce::AudioPlayHead::PositionInfo info;

    info.setIsPlaying (playing.load());
    info.setIsRecording (false);
    info.setIsLooping (looping.load());

    const double positionSeconds = playheadSeconds.load();
    info.setTimeInSeconds (positionSeconds);

    const double sr = sampleRate.load();
    info.setTimeInSamples (static_cast<int64_t> (positionSeconds * sr));

    // Get BPM from tempo map at current position
    double bpm = fallbackBpm.load();
    if (fileLoaded.load())
    {
        const juce::SpinLock::ScopedTryLockType sl (lock);
        if (sl.isLocked() && ! tempoMap.empty())
        {
            const double currentTicks = secondsToTicks (positionSeconds);

            // Find the tempo event that applies to our current position
            for (size_t i = tempoMap.size(); i > 0; --i)
            {
                if (tempoMap[i - 1].tickPosition <= currentTicks)
                {
                    double microsecondsPerQuarterNote = tempoMap[i - 1].microsecondsPerQuarterNote;
                    bpm = 60000000.0 / microsecondsPerQuarterNote;
                    break;
                }
            }
        }
    }
    info.setBpm (bpm);

    // Calculate PPQ position (quarter notes from start)
    const short timeFormat = midiFile.getTimeFormat();
    if (timeFormat > 0 && fileLoaded.load())
    {
        const juce::SpinLock::ScopedTryLockType sl (lock);
        if (sl.isLocked())
        {
            const double ticks = secondsToTicks (positionSeconds);
            const double ppq = ticks / static_cast<double> (timeFormat);
            info.setPpqPosition (ppq);

            // Calculate bar start position (assuming 4/4 time signature)
            const double beatsPerBar = 4.0;
            const double barNumber = std::floor (ppq / beatsPerBar);
            info.setPpqPositionOfLastBarStart (barNumber * beatsPerBar);
        }
    }

    // Set time signature (default 4/4 - MIDI files may have time sig events but we use default)
    info.setTimeSignature (juce::AudioPlayHead::TimeSignature { 4, 4 });

    if (looping.load())
    {
        juce::AudioPlayHead::LoopPoints loopPoints;
        loopPoints.ppqStart = 0.0;

        if (timeFormat > 0)
            loopPoints.ppqEnd = lengthInTicks.load() / static_cast<double> (timeFormat);
        else
            loopPoints.ppqEnd = lengthInSeconds.load() * (bpm / 60.0);

        info.setLoopPoints (loopPoints);
    }

    return info;
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

    const double currentSampleRate = sampleRate.load();
    const double secondsPerSample = 1.0 / currentSampleRate;

    const double blockStartSeconds = playheadSeconds.load();
    const double blockEndSeconds = blockStartSeconds + (numSamples * secondsPerSample);

    // Convert to ticks using tempo map
    const double blockStartTicks = secondsToTicks (blockStartSeconds);
    const double blockEndTicks = secondsToTicks (blockEndSeconds);

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
            // Calculate sample position within this block using tempo map
            const double eventSeconds = ticksToSeconds (eventTicks);
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

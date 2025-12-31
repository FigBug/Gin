/*
  ==============================================================================

  This file is based on part of the dRowAudio JUCE module
  Copyright 2004-13 by dRowAudio.
  dRowAudio is provided under the terms of The MIT License (MIT):

  ==============================================================================
*/


TriggeredScope::TriggeredScope (AudioFifo& f) :
    fifo (f)
{
    setNumChannels (1);

    for (int i = 0; i < 32; i++)
        setColour (traceColourId + i, juce::Colours::white);

    for (int i = 0; i < 32; i++)
        setColour (envelopeColourId + i, juce::Colours::white.withAlpha (0.5f));

    startTimerHz (60);
}

TriggeredScope::~TriggeredScope()
{
    stopTimer();
}

void TriggeredScope::setNumChannels (int num)
{
    if (circularBuffer.getNumChannels() != num)
    {
        circularBuffer.setSize (num, bufferSize);
        circularBuffer.clear();
        writePos = 0;
    }
}

void TriggeredScope::setNumSamplesPerPixel (const float newNumSamplesPerPixel)
{
    numSamplesPerPixel = newNumSamplesPerPixel;
    repaint();
}

void TriggeredScope::setVerticalZoomFactor (const float newVerticalZoomFactor)
{
    verticalZoomFactor = newVerticalZoomFactor;
    repaint();
}

void TriggeredScope::setVerticalZoomOffset (float newVerticalZoomOffset, int ch)
{
    if (verticalZoomOffset.size() < ch + 1)
        verticalZoomOffset.resize (ch + 1);

    verticalZoomOffset.set (ch, newVerticalZoomOffset);
    repaint();
}

void TriggeredScope::setTriggerMode (const TriggerMode newTriggerMode)
{
    triggerMode = newTriggerMode;
}

float TriggeredScope::getSample (int channel, int pos) const
{
    if (channel < 0 || channel >= circularBuffer.getNumChannels())
        return 0.0f;

    int wrappedPos = ((pos % bufferSize) + bufferSize) % bufferSize;
    return circularBuffer.getSample (channel, wrappedPos);
}

float TriggeredScope::getAverageSample (int pos) const
{
    int numChannels = circularBuffer.getNumChannels();
    if (numChannels == 0)
        return 0.0f;

    int wrappedPos = ((pos % bufferSize) + bufferSize) % bufferSize;
    float sum = 0.0f;
    for (int ch = 0; ch < numChannels; ch++)
        sum += circularBuffer.getSample (ch, wrappedPos);

    return sum / float (numChannels);
}

void TriggeredScope::addSamples (const juce::AudioSampleBuffer& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin (buffer.getNumChannels(), circularBuffer.getNumChannels());

    for (int ch = 0; ch < numChannels; ch++)
    {
        const float* src = buffer.getReadPointer (ch);
        float* dst = circularBuffer.getWritePointer (ch);

        int pos = writePos;
        for (int i = 0; i < numSamples; i++)
        {
            dst[pos] = src[i];
            pos = (pos + 1) % bufferSize;
        }
    }

    writePos = (writePos + numSamples) % bufferSize;
}

//==============================================================================

void TriggeredScope::paint (juce::Graphics& g)
{
    render (g);

    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());

    // Draw trigger level indicator
    if (triggerMode != None && drawTriggerPos)
    {
        g.setColour (findColour (lineColourId).withMultipliedAlpha (0.7f));
        const int w = getWidth();
        const int h = getHeight();

        int ch = std::max (0, triggerChannel);
        float offset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;
        const float y = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset + triggerLevel)))) * float (h);

        g.drawHorizontalLine (juce::roundToInt (y), 0.0f, float (w));
        g.drawVerticalLine (juce::roundToInt (float (w) * triggerPos), 0.0f, float (h));
    }

    // Draw cursor info
    if (drawCursorInfo && mousePos.has_value() && circularBuffer.getNumChannels() > 0)
    {
        const int w = getWidth();
        const int h = getHeight();

        // Lambda to calculate sample position from screen X
        auto getSamplePosFromScreenX = [&] (int screenX) -> int
        {
            int triggerSamplePos = findTriggerPoint();
            int startSamplePos = triggerSamplePos - juce::roundToInt (float (w) * triggerPos * numSamplesPerPixel);

            return startSamplePos + juce::roundToInt (float (screenX) * numSamplesPerPixel);
        };

        // Lambda to find closest channel at a given position
        auto findClosestChannel = [&] (int samplePos, int screenY) -> int
        {
            int closest = 0;
            float closestDist = std::numeric_limits<float>::max();

            for (int ch = 0; ch < circularBuffer.getNumChannels(); ch++)
            {
                float offset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;
                float val = getSample (ch, samplePos);
                float chY = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset + val)))) * float (h);
                float dist = std::abs (chY - float (screenY));

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closest = ch;
                }
            }
            return closest;
        };

        // Lambda to draw cursor
        auto drawCursor = [&] (int samplePos, int screenX, int ch, juce::Colour colour)
        {
            float val = getSample (ch, samplePos);
            float offset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;
            float dotY = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset + val)))) * float (h);

            g.setColour (juce::Colours::lightgrey.withAlpha (0.5f));
            g.drawVerticalLine (screenX, 0.0f, float (h));
            g.drawHorizontalLine (juce::roundToInt (dotY), 0.0f, float (w));

            g.setColour (colour);
            g.fillEllipse (float (screenX) - 4.0f, dotY - 4.0f, 8.0f, 8.0f);
        };

        int samplePos1 = getSamplePosFromScreenX (mousePos->x);
        int ch1 = findClosestChannel (samplePos1, mousePos->y);
        float val1 = getSample (ch1, samplePos1);
        float absVal1 = std::abs (val1);
        float dB1 = absVal1 > 0.0f ? 20.0f * std::log10 (absVal1) : -100.0f;
        double timeInMs1 = (double (mousePos->x) * numSamplesPerPixel / sampleRate) * 1000.0;

        if (isDragging && anchorPos.has_value())
        {
            int samplePos2 = getSamplePosFromScreenX (anchorPos->x);
            int ch2 = findClosestChannel (samplePos2, anchorPos->y);
            float val2 = getSample (ch2, samplePos2);
            float absVal2 = std::abs (val2);
            float dB2 = absVal2 > 0.0f ? 20.0f * std::log10 (absVal2) : -100.0f;
            double timeInMs2 = (double (anchorPos->x) * numSamplesPerPixel / sampleRate) * 1000.0;

            // Draw anchor cursor (dimmer)
            drawCursor (samplePos2, anchorPos->x, ch2, findColour (traceColourId + ch2).withAlpha (0.6f));

            // Draw current cursor
            drawCursor (samplePos1, mousePos->x, ch1, findColour (traceColourId + ch1));

            // Draw line between dots
            float offset1 = ch1 < verticalZoomOffset.size() ? verticalZoomOffset[ch1] : 0.0f;
            float dotY1 = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset1 + val1)))) * float (h);
            float offset2 = ch2 < verticalZoomOffset.size() ? verticalZoomOffset[ch2] : 0.0f;
            float dotY2 = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset2 + val2)))) * float (h);

            g.setColour (juce::Colours::white.withAlpha (0.5f));
            g.drawLine (float (mousePos->x), dotY1, float (anchorPos->x), dotY2, 1.0f);

            // Calculate deltas
            double deltaTime = std::abs (timeInMs1 - timeInMs2);
            float deltadB = dB1 - dB2;

            // Format delta text
            juce::String text = "delta: ";
            if (deltaTime < 1.0)
                text += juce::String (deltaTime * 1000.0, 1) + " us, ";
            else
                text += juce::String (deltaTime, 2) + " ms, ";
            text += juce::String (deltadB, 1) + " dB";

            // Calculate frequency from time delta
            if (deltaTime > 0.0)
            {
                double freqHz = 1000.0 / deltaTime;
                if (freqHz >= 20.0 && freqHz <= 20000.0)
                {
                    text += " (" + juce::String (freqHz, 1) + " Hz, ";

                    float exactNote = 12.0f * std::log2 (float (freqHz) / 440.0f) + 69.0f;
                    int midiNote = juce::roundToInt (exactNote);
                    int cents = juce::roundToInt ((exactNote - float (midiNote)) * 100.0f);

                    static const char* noteNames[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
                    int octave = (midiNote / 12) - 1;
                    int noteIndex = midiNote % 12;
                    if (noteIndex < 0) noteIndex += 12;

                    text += juce::String (noteNames[noteIndex]) + juce::String (octave);
                    text += juce::String (" ") + (cents >= 0 ? "+" : "") + juce::String (cents) + ")";
                }
            }

            juce::GlyphArrangement glyphs;
            glyphs.addLineOfText (g.getCurrentFont(), text, 0.0f, 0.0f);
            float textWidth = glyphs.getBoundingBox (0, -1, true).getWidth() + 12.0f;
            float textX = float (w) - textWidth - 4.0f;
            float textY = 4.0f;

            g.setColour (juce::Colours::black.withAlpha (0.7f));
            g.fillRoundedRectangle (textX, textY, textWidth, 16.0f, 3.0f);
            g.setColour (juce::Colours::white);
            g.drawText (text, juce::Rectangle<float> (textX, textY, textWidth, 16.0f), juce::Justification::centred);
        }
        else
        {
            // Single cursor mode
            drawCursor (samplePos1, mousePos->x, ch1, findColour (traceColourId + ch1));

            juce::String text;
            if (timeInMs1 < 1.0)
                text = juce::String (timeInMs1 * 1000.0, 1) + " us, ";
            else
                text = juce::String (timeInMs1, 2) + " ms, ";
            text += juce::String (dB1, 1) + " dB";

            juce::GlyphArrangement glyphs;
            glyphs.addLineOfText (g.getCurrentFont(), text, 0.0f, 0.0f);
            float textWidth = glyphs.getBoundingBox (0, -1, true).getWidth() + 12.0f;
            float textX = float (w) - textWidth - 4.0f;
            float textY = 4.0f;

            g.setColour (juce::Colours::black.withAlpha (0.7f));
            g.fillRoundedRectangle (textX, textY, textWidth, 16.0f, 3.0f);
            g.setColour (juce::Colours::white);
            g.drawText (text, juce::Rectangle<float> (textX, textY, textWidth, 16.0f), juce::Justification::centred);
        }
    }
}

void TriggeredScope::mouseMove (const juce::MouseEvent& e)
{
    if (drawCursorInfo && ! isDragging)
    {
        mousePos = e.getPosition();
        repaint();
    }
}

void TriggeredScope::mouseDown (const juce::MouseEvent& e)
{
    if (drawCursorInfo)
    {
        anchorPos = e.getPosition();
        mousePos = e.getPosition();
        isDragging = true;
        repaint();
    }
}

void TriggeredScope::mouseDrag (const juce::MouseEvent& e)
{
    if (drawCursorInfo && isDragging)
    {
        mousePos = e.getPosition();
        repaint();
    }
}

void TriggeredScope::mouseUp (const juce::MouseEvent&)
{
    if (isDragging)
    {
        isDragging = false;
        anchorPos.reset();
        repaint();
    }
}

void TriggeredScope::mouseExit (const juce::MouseEvent&)
{
    mousePos.reset();
    anchorPos.reset();
    isDragging = false;
    repaint();
}

void TriggeredScope::setPaused (bool shouldBePaused)
{
    setPaused (shouldBePaused, -1);
}

void TriggeredScope::setPaused (bool shouldBePaused, int lockTriggerPoint)
{
    if (paused == shouldBePaused)
        return;

    if (shouldBePaused)
    {
        // Pausing: lock the trigger point (use provided one or find current)
        triggerPoint = lockTriggerPoint >= 0 ? lockTriggerPoint : findTriggerPoint();
        dataDiscardedWhilePaused = false;
    }
    else
    {
        // Unpausing: just clear the trigger point, buffer continues naturally
        triggerPoint = -1;
        dataDiscardedWhilePaused = false;
    }

    paused = shouldBePaused;
}

void TriggeredScope::timerCallback()
{
    if (fifo.getNumChannels() != circularBuffer.getNumChannels())
        setNumChannels (fifo.getNumChannels());

    while (fifo.getNumReady() > 0)
    {
        ScratchBuffer buffer (fifo.getNumChannels(), std::min (512, fifo.getNumReady()));

        fifo.read (buffer);

        if (paused && triggerPoint >= 0)
        {
            // When paused, keep filling until trigger point is roughly centered in buffer
            // Calculate how far trigger point is from writePos
            int distanceFromWrite = writePos - triggerPoint;
            if (distanceFromWrite < 0) distanceFromWrite += bufferSize;

            // We want trigger point to be about half a buffer behind writePos
            int targetDistance = bufferSize / 2;

            if (distanceFromWrite < targetDistance)
            {
                // Still need more data after trigger point
                addSamples (buffer);
            }
            else
            {
                // We have enough data, mark that we're discarding
                dataDiscardedWhilePaused = true;
            }
        }
        else if (! paused)
        {
            // Remember where new samples start
            int newSamplesStart = writePos;
            int numNewSamples = buffer.getNumSamples();

            addSamples (buffer);

            // Single trigger mode: check if new samples contain a trigger crossing
            if (singleTrigger && triggerMode != None)
            {
                auto getTriggerSample = [&] (int pos) -> float
                {
                    if (triggerChannel == -1)
                        return getAverageSample (pos);
                    return getSample (triggerChannel, pos);
                };

                // Search through the new samples for a trigger crossing
                for (int i = 0; i < numNewSamples; i++)
                {
                    int pos = (newSamplesStart + i) % bufferSize;
                    int prevPos = (pos - 1 + bufferSize) % bufferSize;

                    float prevVal = getTriggerSample (prevPos);
                    float curVal = getTriggerSample (pos);

                    bool triggered = false;
                    if (triggerMode == Up || triggerMode == Auto)
                        triggered = prevVal < curVal && prevVal <= triggerLevel && curVal > triggerLevel;
                    else if (triggerMode == Down)
                        triggered = prevVal > curVal && prevVal >= triggerLevel && curVal < triggerLevel;

                    if (triggered)
                    {
                        setPaused (true, pos);
                        break;
                    }
                }
            }
        }
        // else: paused but no trigger point yet, just discard
    }

    repaint();

    // Update playhead from source if available
    if (playheadSource && beatSyncBeats > 0)
    {
        auto [ppq, bpm, playing] = playheadSource();
        updatePlayhead (ppq, bpm, playing);
    }
}

void TriggeredScope::updatePlayhead (double ppqPosition, double bpm, bool isPlaying)
{
    currentBpm.store (bpm);
    currentPpq.store (ppqPosition);

    bool wasPlaying = hostIsPlaying.exchange (isPlaying);

    if (beatSyncBeats > 0 && isPlaying && circularBuffer.getNumChannels() > 0)
    {
        // Detect beat cycle boundaries
        int currentCycle = int (std::floor (ppqPosition / double (beatSyncBeats)));

        if (currentCycle != lastBeatCycle)
        {
            lastBeatCycle = currentCycle;
            beatSyncCycleStartPos.store (writePos);
            beatSyncCycleStartPpq.store (double (currentCycle * beatSyncBeats));
        }
    }
    else
    {
        beatSyncCycleStartPos.store (-1);
    }

    if (! isPlaying && wasPlaying)
    {
        beatSyncCycleStartPos.store (-1);
        lastBeatCycle = -1;
    }
}

//==============================================================================
int TriggeredScope::findTriggerPoint()
{
    if (circularBuffer.getNumChannels() == 0)
        return 0;

    // If we have a locked trigger point (single trigger mode), use it
    if (triggerPoint >= 0)
        return triggerPoint;

    if (triggerMode == Auto)
    {
        updateAutoTrigger();
        return autoTriggerPos;
    }

    // Calculate samples needed to fill the screen
    // Use the same rounding as the render loop to avoid reading past writePos
    int samplesPerPixel = juce::jmax (1, juce::roundToInt (numSamplesPerPixel));
    int screenSamples = getWidth() * samplesPerPixel;

    // Start searching from one screenful back from writePos to ensure we have data to display
    int searchStart = writePos - screenSamples;
    if (searchStart < 0) searchStart += bufferSize;

    if (triggerMode == None)
        return searchStart;

    // Search backwards for trigger point - limit to 1 second for performance
    int maxSearchSamples = juce::roundToInt (sampleRate);
    int searchSize = juce::jmin (maxSearchSamples, bufferSize - screenSamples);

    auto getTriggerSample = [&] (int pos) -> float
    {
        if (triggerChannel == -1)
            return getAverageSample (pos);
        return getSample (triggerChannel, pos);
    };

    int pos = searchStart;
    for (int i = 0; i < searchSize; i++)
    {
        int prevPos = pos - 1;
        if (prevPos < 0) prevPos += bufferSize;

        float prevVal = getTriggerSample (prevPos);
        float curVal = getTriggerSample (pos);

        if (triggerMode == Up)
        {
            if (prevVal < curVal && prevVal <= triggerLevel && curVal > triggerLevel)
                return pos;
        }
        else if (triggerMode == Down)
        {
            if (prevVal > curVal && prevVal >= triggerLevel && curVal < triggerLevel)
                return pos;
        }

        pos = prevPos;
    }

    // No trigger found, return searchStart (one screenful back)
    return searchStart;
}

void TriggeredScope::render (juce::Graphics& g)
{
    if (beatSyncBeats > 0 && hostIsPlaying.load() && circularBuffer.getNumChannels() > 0)
        renderBeatSync (g);
    else
        renderTrigger (g);
}

void TriggeredScope::renderBeatSync (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    int cycleStartPos = beatSyncCycleStartPos.load();
    if (cycleStartPos < 0 || circularBuffer.getNumChannels() == 0)
        return;

    // Calculate how many raw samples in this beat cycle
    double bpm = currentBpm.load();
    double samplesPerBeat = (sampleRate * 60.0) / bpm;
    int totalSamplesInCycle = juce::roundToInt (double (beatSyncBeats) * samplesPerBeat);

    // Calculate the PPQ offset within the current cycle
    double ppq = currentPpq.load();
    double cycleStartPpq = beatSyncCycleStartPpq.load();
    double ppqInCycle = ppq - cycleStartPpq;

    // Convert PPQ offset to sample offset - this is where we are in the cycle
    int sampleOffsetInCycle = juce::roundToInt (ppqInCycle * samplesPerBeat);
    sampleOffsetInCycle = juce::jlimit (0, totalSamplesInCycle, sampleOffsetInCycle);

    if (sampleOffsetInCycle <= 0)
        return;

    // Samples per pixel for this beat sync view
    float spp = float (totalSamplesInCycle) / float (w);
    spp = std::max (1.0f, spp);

    for (int ch = 0; ch < circularBuffer.getNumChannels(); ch++)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);

        bool drawTrace = ! traceColour.isTransparent();
        bool drawEnvelope = ! envelopeColour.isTransparent();

        float chOffset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;

        const float* channelData = circularBuffer.getReadPointer (ch);

        juce::Path p;
        g.setColour (envelopeColour);

        int currentX = 0;
        int sampleIdx = 0;

        while (currentX < w && sampleIdx < sampleOffsetInCycle)
        {
            int samplesToRead = juce::jmax (1, juce::roundToInt (spp));
            samplesToRead = juce::jmin (samplesToRead, sampleOffsetInCycle - sampleIdx);

            if (samplesToRead <= 0)
                break;

            float minVal = 1.0f, maxVal = -1.0f, sum = 0.0f;
            int validSamples = 0;

            for (int i = 0; i < samplesToRead; i++)
            {
                int bufPos = (cycleStartPos + sampleIdx + i) % bufferSize;
                if (bufPos < 0) bufPos += bufferSize;
                float val = channelData[bufPos];

                if (! std::isnan (val))
                {
                    minVal = std::min (minVal, val);
                    maxVal = std::max (maxVal, val);
                    sum += val;
                    validSamples++;
                }
            }

            sampleIdx += samplesToRead;

            if (validSamples > 0)
            {
                const float top = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + maxVal)))) * float (h);
                const float bottom = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + minVal)))) * float (h);
                const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + sum / float (validSamples))))) * float (h);

                if (drawEnvelope && bottom - top > 2)
                    g.drawVerticalLine (currentX, top, bottom);

                if (drawTrace)
                {
                    if (currentX == 0)
                        p.startNewSubPath (float (currentX), mid);
                    else
                        p.lineTo (float (currentX), mid);
                }
            }

            currentX++;
        }

        if (drawTrace)
        {
            g.setColour (traceColour);
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }
    }

    // Draw beat lines
    float pixelsPerBeat = float (w) / float (beatSyncBeats);
    auto lineColour = findColour (lineColourId);

    for (int beat = 0; beat <= beatSyncBeats; beat++)
    {
        float x = float (beat) * pixelsPerBeat;
        bool isBarStart = (beat % 4) == 0;

        if (isBarStart)
        {
            g.setColour (lineColour);
            g.drawVerticalLine (juce::roundToInt (x), 0.0f, float (h));
        }
        else
        {
            g.setColour (lineColour.withMultipliedAlpha (0.5f));
            g.drawLine (x, 0.0f, x, float (h), 0.5f);
        }
    }
}

void TriggeredScope::renderTrigger (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    if (circularBuffer.getNumChannels() == 0)
        return;

    int triggerSamplePos = findTriggerPoint();

    // Offset by trigger position (where on screen the trigger point appears)
    // triggerPos=0 means trigger at left, triggerPos=1 means trigger at right
    int startSamplePos = triggerSamplePos - juce::roundToInt (float (w) * triggerPos * numSamplesPerPixel);

    for (int ch = 0; ch < circularBuffer.getNumChannels(); ch++)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);

        bool drawTrace = ! traceColour.isTransparent();
        bool drawEnvelope = ! envelopeColour.isTransparent();

        float chOffset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;

        const float* channelData = circularBuffer.getReadPointer (ch);

        juce::Path p;
        g.setColour (envelopeColour);

        if (numSamplesPerPixel < 1.0f)
        {
            // Zoomed in: each sample spans multiple pixels
            const float pixelsPerSample = 1.0f / numSamplesPerPixel;
            int samplePos = startSamplePos;
            float currentX = 0.0f;
            bool firstPoint = true;

            while (currentX <= float (w) + pixelsPerSample)
            {
                int bufPos = ((samplePos % bufferSize) + bufferSize) % bufferSize;
                float val = channelData[bufPos];

                const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + val)))) * float (h);

                if (drawTrace)
                {
                    if (firstPoint)
                    {
                        p.startNewSubPath (currentX, mid);
                        firstPoint = false;
                    }
                    else
                    {
                        p.lineTo (currentX, mid);
                    }
                }

                samplePos++;
                currentX += pixelsPerSample;
            }
        }
        else
        {
            // Zoomed out: multiple samples per pixel
            int samplesPerPixel = juce::roundToInt (numSamplesPerPixel);
            int samplePos = startSamplePos;
            int currentX = 0;

            while (currentX < w)
            {
                float minVal = 1.0f, maxVal = -1.0f, sum = 0.0f;
                int validSamples = 0;

                for (int i = 0; i < samplesPerPixel; i++)
                {
                    int bufPos = ((samplePos % bufferSize) + bufferSize) % bufferSize;
                    float val = channelData[bufPos];

                    if (! std::isnan (val))
                    {
                        minVal = std::min (minVal, val);
                        maxVal = std::max (maxVal, val);
                        sum += val;
                        validSamples++;
                    }

                    samplePos++;
                }

                if (validSamples > 0)
                {
                    const float top = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + maxVal)))) * float (h);
                    const float bottom = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + minVal)))) * float (h);
                    const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + sum / float (validSamples))))) * float (h);

                    if (drawEnvelope && bottom - top > 2)
                        g.drawVerticalLine (currentX, top, bottom);

                    if (drawTrace)
                    {
                        if (currentX == 0)
                            p.startNewSubPath (float (currentX), mid);
                        else
                            p.lineTo (float (currentX), mid);
                    }
                }

                currentX++;
            }
        }

        if (drawTrace)
        {
            g.setColour (traceColour);
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }
    }
}

void TriggeredScope::resetTrigger()
{
    setPaused (false);
}

void TriggeredScope::updateAutoTrigger()
{
    if (circularBuffer.getNumChannels() == 0)
        return;

    const int windowSamples = juce::roundToInt (float (getWidth()) * numSamplesPerPixel);
    if (windowSamples <= 0)
        return;

    // Search window is 2x the display window
    const int searchSize = windowSamples;

    auto peekSample = [&] (int offset) -> float
    {
        int pos = writePos - windowSamples * 2 + offset;
        if (triggerChannel == -1)
            return getAverageSample (pos);
        return getSample (triggerChannel, pos);
    };

    auto peekMask = [&] (int offset) -> uint8_t
    {
        return peekSample (offset) > 0 ? 1 : 0;
    };

    // Ensure lastMatchMask is sized correctly
    if (lastMatchMask.size() != size_t (windowSamples))
    {
        lastMatchMask.resize (size_t (windowSamples));
        for (int i = 0; i < windowSamples; i++)
            lastMatchMask[size_t (i)] = peekMask (i);
    }

    // Collect all positive zero-crossings in the search window
    autoCandidates.clear();

    for (int i = 1; i < searchSize; i++)
        if (peekSample (i - 1) <= 0 && peekSample (i) > 0)
            autoCandidates.push_back (i);

    // Calculate correlations with last match
    autoRatings.clear();

    for (auto start : autoCandidates)
    {
        int rating = 0;

        for (int offset = 0; offset < windowSamples; offset++)
        {
            uint8_t value1 = lastMatchMask[size_t (offset)];
            uint8_t value2 = peekMask (start + offset);
            rating += (value1 & value2);
        }

        autoRatings.push_back (rating);
    }

    // No zero-crossings? Special case for low frequencies
    if (autoCandidates.empty())
    {
        if (lowFreqDelay > 0)
        {
            lowFreqDelay--;
        }
        else
        {
            autoCandidates.push_back (0);
            autoRatings.push_back (0);
        }
    }
    else if (int (autoCandidates.size()) < s_lowFreqThreshold)
    {
        lowFreqDelay = s_maxLowFreqDelay;
    }

    // Pick the best match and store trigger position
    if (! autoCandidates.empty())
    {
        auto it = std::max_element (autoRatings.begin(), autoRatings.end());
        auto index = int (it - autoRatings.begin());
        int bestStart = autoCandidates[size_t (index)];

        for (int offset = 0; offset < windowSamples; offset++)
            lastMatchMask[size_t (offset)] = peekMask (bestStart + offset);

        // Store the absolute buffer position for this trigger point
        int newTriggerPos = writePos - windowSamples * 2 + bestStart;
        autoTriggerPos = ((newTriggerPos % bufferSize) + bufferSize) % bufferSize;
    }
}

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

    for (auto c : channels)
    {
        c->posBuffer.clear ((size_t) c->bufferSize);
        c->minBuffer.clear ((size_t) c->bufferSize);
        c->maxBuffer.clear ((size_t) c->bufferSize);
    }

    startTimerHz (60);
}

TriggeredScope::~TriggeredScope()
{
    stopTimer();
}

void TriggeredScope::setNumChannels (int num)
{
    channels.clear();

    while (channels.size() < num)
        channels.add (new Channel());

    for (auto c : channels)
    {
        c->posBuffer.clear ((size_t) c->bufferSize);
        c->minBuffer.clear ((size_t) c->bufferSize);
        c->maxBuffer.clear ((size_t) c->bufferSize);
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

void TriggeredScope::addSamples (const juce::AudioSampleBuffer& buffer)
{
    jassert (buffer.getNumChannels() == channels.size());

    for (int i = 0; i < std::fmin (buffer.getNumChannels(), channels.size()); i++)
    {
        const float* samples = buffer.getReadPointer (i);
        const int numSamples = buffer.getNumSamples();

        // if we don't have enough space in the fifo, bail out, scope might be frozen
        const int numFreeInBuffer = channels[i]->samplesToProcess.getFreeSpace();
        if (numFreeInBuffer >= numSamples)
            channels[i]->samplesToProcess.writeMono (samples, numSamples);
    }
    needToUpdate = true;
}

//==============================================================================

void TriggeredScope::paint (juce::Graphics& g)
{
    if (needToUpdate)
    {
        needToUpdate = false;
        processPendingSamples();
    }

    render (g);

    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());

    g.setColour (findColour (lineColourId).withMultipliedAlpha (0.5f));
    if (triggerMode != None && drawTriggerPos)
    {
        const int w = getWidth();
        const int h = getHeight();

        int ch = std::max (0, triggerChannel);
        const float y = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + triggerLevel)))) * float ( h );

        g.drawHorizontalLine (juce::roundToInt (y), 0.0f, float (w));
        g.drawVerticalLine (juce::roundToInt (float ( w ) * triggerPos), 0.0f, float (h));
    }

    // Draw cursor info
    if (drawCursorInfo && mousePos.has_value() && channels.size() > 0)
    {
        const int w = getWidth();
        const int h = getHeight();

        auto* refChannel = channels.getFirst();

        // Lambda to calculate cursor info for a given screen position
        auto getCursorInfo = [&] (juce::Point<int> screenPos) -> std::tuple<bool, int, float, double, int>
        {
            // Returns: valid, bufferPos, dotX, timeInSamples, closestChannel
            int pos = 0;
            float dotX = float (screenPos.x);
            double timeInSamples = 0.0;
            bool validPosition = true;

            if (beatSyncBeats > 0 && hostIsPlaying.load())
            {
                int cycleStartPos = beatSyncCycleStartPos.load();
                int totalBufferSamples = beatSyncTotalSamples.load();

                if (cycleStartPos < 0 || totalBufferSamples <= 0)
                {
                    validPosition = false;
                }
                else
                {
                    int currentWritePos = refChannel->bufferWritePos;
                    int samplesWritten = currentWritePos - cycleStartPos;
                    if (samplesWritten < 0)
                        samplesWritten += refChannel->bufferSize;
                    if (samplesWritten > totalBufferSamples)
                        samplesWritten = totalBufferSamples;

                    float pixelsPerBufferSample = float (w) / float (totalBufferSamples);
                    int sampleOffset = juce::roundToInt (float (screenPos.x) / pixelsPerBufferSample);

                    if (sampleOffset >= samplesWritten)
                    {
                        validPosition = false;
                    }
                    else
                    {
                        pos = (cycleStartPos + sampleOffset) % refChannel->bufferSize;
                        if (pos < 0) pos += refChannel->bufferSize;

                        double beatsPerPixel = double (beatSyncBeats) / double (w);
                        double beatPosition = double (screenPos.x) * beatsPerPixel;
                        double bpm = currentBpm.load();
                        timeInSamples = (beatPosition / bpm) * 60.0 * sampleRate;
                    }
                }
            }
            else
            {
                int bufferReadPos = getTriggerPos().first;
                bufferReadPos -= juce::roundToInt (float (w) * triggerPos);
                if (bufferReadPos < 0)
                    bufferReadPos += refChannel->bufferSize;

                int sampleOffset;
                if (numSamplesPerPixel < 1.0f)
                    sampleOffset = juce::roundToInt (float (screenPos.x) * numSamplesPerPixel);
                else
                    sampleOffset = screenPos.x;

                pos = (bufferReadPos + sampleOffset + 1) % refChannel->bufferSize;

                if (numSamplesPerPixel < 1.0f)
                    dotX = float (sampleOffset) / numSamplesPerPixel;

                timeInSamples = double (sampleOffset) * std::max (1.0f, numSamplesPerPixel);
            }

            // Find closest channel
            int closestChannel = 0;
            if (validPosition)
            {
                float closestDistance = std::numeric_limits<float>::max();
                for (int chIdx = 0; chIdx < channels.size(); chIdx++)
                {
                    float chOffset = chIdx < verticalZoomOffset.size() ? verticalZoomOffset[chIdx] : 0.0f;
                    float chValue = channels[chIdx]->posBuffer[pos];
                    float chY = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (chOffset + chValue)))) * float (h);
                    float distance = std::abs (chY - float (screenPos.y));

                    if (distance < closestDistance)
                    {
                        closestDistance = distance;
                        closestChannel = chIdx;
                    }
                }
            }

            return { validPosition, pos, dotX, timeInSamples, closestChannel };
        };

        // Lambda to draw a cursor dot
        auto drawCursor = [&] (int bufferPos, float dotX, int channel, juce::Colour colour)
        {
            float sampleValue = channels[channel]->posBuffer[bufferPos];
            float offset = channel < verticalZoomOffset.size() ? verticalZoomOffset[channel] : 0.0f;
            float dotY = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset + sampleValue)))) * float (h);

            g.setColour (juce::Colours::lightgrey.withAlpha (0.5f));
            g.drawVerticalLine (juce::roundToInt (dotX), 0.0f, float (h));
            g.drawHorizontalLine (juce::roundToInt (dotY), 0.0f, float (w));

            g.setColour (colour);
            g.fillEllipse (dotX - 4.0f, dotY - 4.0f, 8.0f, 8.0f);
        };

        // Get current cursor info
        auto [valid1, pos1, dotX1, time1, ch1] = getCursorInfo (*mousePos);

        if (valid1)
        {
            float sampleValue1 = channels[ch1]->posBuffer[pos1];
            float absValue1 = std::abs (sampleValue1);
            float dB1 = absValue1 > 0.0f ? 20.0f * std::log10 (absValue1) : -100.0f;
            double timeInMs1 = (time1 / sampleRate) * 1000.0;

            // Check if we're in drag mode with an anchor
            if (isDragging && anchorPos.has_value())
            {
                auto [valid2, pos2, dotX2, time2, ch2] = getCursorInfo (*anchorPos);

                if (valid2)
                {
                    float sampleValue2 = channels[ch2]->posBuffer[pos2];
                    float absValue2 = std::abs (sampleValue2);
                    float dB2 = absValue2 > 0.0f ? 20.0f * std::log10 (absValue2) : -100.0f;
                    double timeInMs2 = (time2 / sampleRate) * 1000.0;

                    // Draw anchor cursor (dimmer)
                    drawCursor (pos2, dotX2, ch2, findColour (traceColourId + ch2).withAlpha (0.6f));

                    // Draw current cursor
                    drawCursor (pos1, dotX1, ch1, findColour (traceColourId + ch1));

                    // Draw line between dots
                    float offset1 = ch1 < verticalZoomOffset.size() ? verticalZoomOffset[ch1] : 0.0f;
                    float dotY1 = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset1 + sampleValue1)))) * float (h);
                    float offset2 = ch2 < verticalZoomOffset.size() ? verticalZoomOffset[ch2] : 0.0f;
                    float dotY2 = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (offset2 + sampleValue2)))) * float (h);

                    g.setColour (juce::Colours::white.withAlpha (0.5f));
                    g.drawLine (dotX1, dotY1, dotX2, dotY2, 1.0f);

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

                            // Calculate MIDI note and cents
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
            }
            else
            {
                // Single cursor mode
                drawCursor (pos1, dotX1, ch1, findColour (traceColourId + ch1));

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

void TriggeredScope::timerCallback()
{
    if (fifo.getNumChannels() != channels.size())
        setNumChannels (fifo.getNumChannels());

    while (fifo.getNumReady() > 0)
    {
        ScratchBuffer buffer (fifo.getNumChannels(), std::min (512, fifo.getNumReady()));

        fifo.read (buffer);

        // When paused, still read from fifo to prevent overflow, but don't process
        if (! paused)
        {
            addSamples (buffer);
            repaint();
        }
    }

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

    bool wasPlaying = hostIsPlaying.exchange (isPlaying);

    if (beatSyncBeats > 0 && isPlaying && channels.size() > 0)
    {
        int displayWidth = getWidth();
        if (displayWidth <= 0)
            displayWidth = 800; // fallback

        // Calculate raw samples for the beat cycle
        double rawSamplesPerBeat = (sampleRate * 60.0) / bpm;
        double totalRawSamples = double (beatSyncBeats) * rawSamplesPerBeat;

        // Calculate SPP so that the beat cycle maps to the display width
        // We want: totalRawSamples / spp == displayWidth
        float beatSpp = float (totalRawSamples / double (displayWidth));
        beatSpp = std::max (1.0f, beatSpp);

        beatSyncSamplesPerPixel.store (beatSpp);

        // The number of buffer samples for the full cycle equals display width
        beatSyncTotalSamples.store (displayWidth);

        // Detect beat cycle boundaries
        int currentCycle = int (std::floor (ppqPosition / double (beatSyncBeats)));

        if (currentCycle != lastBeatCycle)
        {
            lastBeatCycle = currentCycle;
            // Record the buffer position where this cycle starts
            if (channels.size() > 0)
                beatSyncCycleStartPos.store (channels[0]->bufferWritePos);
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
void TriggeredScope::processPendingSamples()
{
    bool triggered = false;
    int maxProcess = std::numeric_limits<int>::max();
    if (singleTrigger && channels.size() > 0)
    {
        if (triggerPoint >= 0)
        {
            triggered = true;
        }
        else if (triggerMode == Auto)
        {
            // For Auto mode, check if we have actual signal (zero crossings above trigger level)
            auto* refChannel = channels.getFirst();
            if (refChannel != nullptr)
            {
                const int windowSize = getWidth();
                const int bufferSize = refChannel->bufferSize;
                const int bufferWritePos = refChannel->bufferWritePos;

                // Check for zero crossings that exceed trigger level
                bool hasSignal = false;
                float effectiveTriggerLevel = std::max (std::abs (triggerLevel), 0.01f);

                for (int i = 1; i < windowSize && !hasSignal; i++)
                {
                    int idx = ((bufferWritePos - windowSize + i) % bufferSize + bufferSize) % bufferSize;
                    int prevIdx = ((bufferWritePos - windowSize + i - 1) % bufferSize + bufferSize) % bufferSize;

                    float val = refChannel->posBuffer[idx];
                    float prevVal = refChannel->posBuffer[prevIdx];

                    // Check for upward zero crossing with signal above trigger level
                    if (prevVal <= 0 && val > 0 && val > effectiveTriggerLevel)
                        hasSignal = true;
                }

                if (hasSignal)
                {
                    updateAutoTrigger();
                    triggerPoint = autoTriggerPos;
                    triggered = true;
                }
            }
        }
        else if (getTriggerPos().second)
        {
            triggerPoint = getTriggerPos().first;
            triggered = true;
        }

        if (triggered)
        {
            auto& c = *channels[0];
            maxProcess = c.bufferSize / 4 - samplesSinceTrigger;
        }
    }

    // In beat sync mode, use the auto-calculated SPP; otherwise use user setting
    float effectiveSpp = numSamplesPerPixel;
    if (beatSyncBeats > 0 && hostIsPlaying.load())
        effectiveSpp = beatSyncSamplesPerPixel.load();

    effectiveSpp = std::max (1.0f, effectiveSpp);

    for (auto c : channels)
    {
        int processed = 0;
        int numSamples = c->samplesToProcess.getNumReady();
        c->samplesToProcess.readMono (c->tempProcessingBlock, numSamples);
        float* samples = c->tempProcessingBlock.getData();

        while (--numSamples >= 0 && processed < maxProcess)
        {
            const float currentSample = *samples++;

            if (currentSample < c->currentMin)
                c->currentMin = currentSample;
            if (currentSample > c->currentMax)
                c->currentMax = currentSample;

            c->currentAve += currentSample;
            c->numAveraged++;

            if (--c->numLeftToAverage <= 0)
            {
                c->posBuffer[c->bufferWritePos] = c->currentAve / float ( c->numAveraged );
                c->minBuffer[c->bufferWritePos] = c->currentMin;
                c->maxBuffer[c->bufferWritePos] = c->currentMax;

                c->currentMax = -999999.0f;
                c->currentMin = 999999.0f;
                c->currentAve = 0.0;

                ++c->bufferWritePos %= c->bufferSize;
                c->numLeftToAverage += int (effectiveSpp);
                c->numAveraged = 0;

                if (triggered)
                    samplesSinceTrigger++;

                processed++;
            }
        }
        triggered = false;
    }
}

std::pair<int, bool> TriggeredScope::getTriggerPos()
{
    const int w = getWidth();

    if ( triggerPoint >= 0 )
    {
        return { triggerPoint, true };
    }

    bool found = false;
    int bufferReadPos = 0;

    auto minBuffer = [&] (int i) -> float
    {
        if (triggerChannel == -1)
        {
            float sum = 0;
            for (auto c : channels)
                sum += c->minBuffer[i];

            return sum / float ( channels.size() );
        }
        else
        {
            return channels[triggerChannel]->minBuffer[i];
        }
    };

    auto maxBuffer = [&] (int i) -> float
    {
        if (triggerChannel == -1)
        {
            float sum = 0;
            for (auto c : channels)
                sum += c->maxBuffer[i];

            return sum / float ( channels.size() );
        }
        else
        {
            return channels[triggerChannel]->maxBuffer[i];
        }
    };

    if (auto c = triggerChannel >= 0 ? channels[triggerChannel] : channels.getFirst())
    {
        bufferReadPos = c->bufferWritePos - w;
        if (bufferReadPos < 0 )
            bufferReadPos += c->bufferSize;

        if (triggerMode == Auto)
        {
            updateAutoTrigger();
            return { autoTriggerPos, true };
        }
        else if (triggerMode != None)
        {
            int posToTest = bufferReadPos;
            int numToSearch = c->bufferSize;
            while (--numToSearch >= 0)
            {
                int prevPosToTest = posToTest - 1;
                if (prevPosToTest < 0)
                    prevPosToTest += c->bufferSize;

                if (triggerMode == Up)
                {
                    if (minBuffer (prevPosToTest) <= triggerLevel
                        && maxBuffer (posToTest) > triggerLevel)
                    {
                        bufferReadPos = posToTest;
                        found = true;
                        break;
                    }
                }
                else
                {
                    if (minBuffer (prevPosToTest) > triggerLevel
                        && maxBuffer (posToTest) <= triggerLevel)
                    {
                        bufferReadPos = posToTest;
                        found = true;
                        break;
                    }
                }

                if (--posToTest < 0)
                    posToTest += c->bufferSize;
            }
        }
    }
    return { bufferReadPos, found };
}

void TriggeredScope::render (juce::Graphics& g)
{
    if (beatSyncBeats > 0 && hostIsPlaying.load() && channels.size() > 0)
        renderBeatSync (g);
    else
        renderTrigger (g);
}

void TriggeredScope::renderBeatSync (juce::Graphics& g)
{
    const int w = getWidth();
    const int h = getHeight();

    int cycleStartPos = beatSyncCycleStartPos.load();
    int totalBufferSamples = beatSyncTotalSamples.load();
    int bufferSize = channels[0]->bufferSize;

    if (cycleStartPos < 0 || totalBufferSamples <= 0)
        return;

    // Calculate how many samples have been written since cycle started
    int currentWritePos = channels[0]->bufferWritePos;
    int samplesWritten = currentWritePos - cycleStartPos;
    if (samplesWritten < 0)
        samplesWritten += bufferSize;

    // Clamp to cycle length
    if (samplesWritten > totalBufferSamples)
        samplesWritten = totalBufferSamples;

    if (samplesWritten <= 0)
        return;

    // Map buffer samples to screen width - beat cycle fills the display
    float pixelsPerBufferSample = float (w) / float (totalBufferSamples);

    // Start reading from the cycle start position
    int startPos = cycleStartPos;

    int ch = 0;
    for (auto c : channels)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);

        bool drawTrace = ! traceColour.isTransparent();
        bool drawEnvelope = ! envelopeColour.isTransparent();

        juce::Path p;
        g.setColour (envelopeColour);

        // Start from the fixed beat boundary position
        int pos = startPos;
        if (pos < 0) pos += c->bufferSize;

        float chOffset = ch < verticalZoomOffset.size() ? verticalZoomOffset[ch] : 0.0f;

        if (pixelsPerBufferSample >= 1.0f)
        {
            // Each buffer sample gets one or more pixels
            float currentX = 0.0f;
            bool firstPoint = true;

            for (int sampleIdx = 0; sampleIdx < samplesWritten; sampleIdx++)
            {
                float val = c->posBuffer[pos];
                if (std::isnan (val)) val = 0.0f;

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

                ++pos;
                if (pos >= c->bufferSize)
                    pos = 0;

                currentX += pixelsPerBufferSample;
            }
        }
        else
        {
            // Multiple buffer samples per pixel
            int samplesPerPixel = juce::roundToInt (1.0f / pixelsPerBufferSample);
            if (samplesPerPixel < 1) samplesPerPixel = 1;

            int currentX = 0;
            int maxX = juce::roundToInt (float (samplesWritten) * pixelsPerBufferSample);
            maxX = std::min (maxX, w);
            int samplesRemaining = samplesWritten;

            while (currentX < maxX && samplesRemaining > 0)
            {
                float minVal = 1.0f, maxVal = -1.0f, sum = 0.0f;
                int samplesToRead = std::min (samplesPerPixel, samplesRemaining);
                int validSamples = 0;

                for (int i = 0; i < samplesToRead; i++)
                {
                    float val = c->posBuffer[pos];
                    if (! std::isnan (val))
                    {
                        minVal = std::min (minVal, val);
                        maxVal = std::max (maxVal, val);
                        sum += val;
                        validSamples++;
                    }

                    ++pos;
                    if (pos >= c->bufferSize)
                        pos = 0;
                }

                samplesRemaining -= samplesToRead;

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

        ch++;
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

    int bufferReadPos = getTriggerPos().first;

    bufferReadPos -= juce::roundToInt (float (w) * triggerPos);
    if (bufferReadPos < 0)
        bufferReadPos += channels[0]->bufferSize;

    int ch = 0;
    for (auto c : channels)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);

        bool drawTrace = ! traceColour.isTransparent();
        bool drawEnvelope = ! envelopeColour.isTransparent();

        juce::Path p;

        g.setColour (envelopeColour);

        if (numSamplesPerPixel < 1.0f)
        {
            // Zoomed in: each sample spans multiple pixels
            const float pixelsPerSample = 1.0f / numSamplesPerPixel;
            int pos = bufferReadPos;
            float currentX = 0.0f;
            bool firstPoint = true;

            while (currentX <= float (w) + pixelsPerSample)
            {
                ++pos;
                if (pos >= c->bufferSize)
                    pos = 0;

                const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->posBuffer[pos])))) * float (h);

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

                currentX += pixelsPerSample;
            }
        }
        else
        {
            // Normal mode: one or more samples per pixel
            int pos = bufferReadPos;
            int currentX = 0;

            while (currentX < w)
            {
                ++pos;
                if (pos >= c->bufferSize)
                    pos = 0;

                const float top = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->maxBuffer[pos])))) * float (h);
                const float bottom = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->minBuffer[pos])))) * float (h);
                const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->posBuffer[pos])))) * float (h);

                if (drawEnvelope && bottom - top > 2)
                    g.drawVerticalLine (currentX, top, bottom);

                if (drawTrace)
                {
                    if (currentX == 0)
                        p.startNewSubPath (float (currentX), mid);
                    else
                        p.lineTo (float (currentX), mid);
                }

                currentX++;
            }
        }

        if (drawTrace)
        {
            g.setColour (traceColour);
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }

        ch++;
    }
}

void TriggeredScope::resetTrigger()
{
    triggerPoint = -1;
    samplesSinceTrigger = 0;
    singleTriggerWaitCount = 3; // Wait a few frames for Auto mode to stabilize

    for (auto c : channels)
    {
        c->posBuffer.clear ((size_t) c->bufferSize);
        c->minBuffer.clear ((size_t) c->bufferSize);
        c->maxBuffer.clear ((size_t) c->bufferSize);
    }
}

void TriggeredScope::updateAutoTrigger()
{
    if (channels.size() == 0)
        return;

    auto* refChannel = channels.getFirst();
    if (refChannel == nullptr)
        return;

    const int windowSize = getWidth();
    if (windowSize <= 0)
        return;

    // We look back 2 windows worth of data to find the best match
    const int searchSize = windowSize;
    const int bufferSize = refChannel->bufferSize;
    const int bufferWritePos = refChannel->bufferWritePos;

    auto peekBuffer = [&] (int idx) -> float
    {
        int wrappedIdx = ((bufferWritePos - windowSize * 2 + idx) % bufferSize + bufferSize) % bufferSize;
        if (triggerChannel == -1)
        {
            float sum = 0;
            for (auto ch : channels)
                sum += ch->posBuffer[wrappedIdx];
            return sum / float (channels.size());
        }
        return channels[triggerChannel]->posBuffer[wrappedIdx];
    };

    auto peekMask = [&] (int idx) -> uint8_t
    {
        return peekBuffer (idx) > 0 ? 1 : 0;
    };

    // Ensure lastMatchMask is sized correctly
    if (lastMatchMask.size() != size_t (windowSize))
    {
        lastMatchMask.resize (size_t (windowSize));
        for (int i = 0; i < windowSize; i++)
            lastMatchMask[size_t (i)] = peekMask (i);
    }

    // collect all positive zero-crossings in the search window
    autoCandidates.clear();

    for (int i = 1; i < searchSize; i++)
        if (peekBuffer (i - 1) <= 0 && peekBuffer (i) > 0)
            autoCandidates.push_back (i);

    // calculate correlations with last match
    autoRatings.clear();

    for (auto start : autoCandidates)
    {
        int rating = 0;

        for (int offset = 0; offset < windowSize; offset++)
        {
            uint8_t value1 = lastMatchMask[size_t (offset)];
            uint8_t value2 = peekMask (start + offset);
            rating += (value1 & value2);
        }

        autoRatings.push_back (rating);
    }

    // no zero-crossings at all? maybe special case for low frequencies
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

    // copy match with best correlation and store trigger position
    if (! autoCandidates.empty())
    {
        auto it = std::max_element (autoRatings.begin(), autoRatings.end());
        auto index = int (it - autoRatings.begin());
        int bestStart = autoCandidates[size_t (index)];

        for (int offset = 0; offset < windowSize; offset++)
            lastMatchMask[size_t (offset)] = peekMask (bestStart + offset);

        // Store the absolute buffer position for this trigger point
        autoTriggerPos = ((bufferWritePos - windowSize * 2 + bestStart) % bufferSize + bufferSize) % bufferSize;
    }
}

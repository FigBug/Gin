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
}

void TriggeredScope::setVerticalZoomFactor (const float newVerticalZoomFactor)
{
    verticalZoomFactor = newVerticalZoomFactor;
}

void TriggeredScope::setVerticalZoomOffset (float newVerticalZoomOffset, int ch)
{
    if (verticalZoomOffset.size() < ch + 1)
        verticalZoomOffset.resize (ch + 1);

    verticalZoomOffset.set (ch, newVerticalZoomOffset);
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
}

void TriggeredScope::timerCallback()
{
    while (fifo.getNumReady() > 0)
    {
        ScratchBuffer buffer (channels.size(), std::min (512, fifo.getNumReady()));

        fifo.read (buffer);
        addSamples (buffer);
        repaint();
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
                c->numLeftToAverage += int (std::max (1.0f, numSamplesPerPixel));
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

        if (triggerMode != None)
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
    const int w = getWidth();
    const int h = getHeight();

    int bufferReadPos = getTriggerPos().first;

    bufferReadPos -= juce::roundToInt ( float ( w ) * triggerPos);
    if (bufferReadPos < 0 )
        bufferReadPos += channels[0]->bufferSize;

    int ch = 0;
    for (auto c : channels)
    {
        auto traceColour = findColour (traceColourId + ch);
        auto envelopeColour = findColour (envelopeColourId + ch);

        bool drawTrace = ! traceColour.isTransparent ();
        bool drawEnvelope = ! envelopeColour.isTransparent ();

        juce::Path p;

        int pos = bufferReadPos;
        int currentX = 0;

        g.setColour (envelopeColour);

        while (currentX < w)
        {
            ++pos;
            if (pos == c->bufferSize)
                pos = 0;

            const float top = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->maxBuffer[pos])))) * float ( h );
            const float bottom = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->minBuffer[pos])))) * float ( h );
            const float mid = (1.0f - (0.5f + (0.5f * verticalZoomFactor * (verticalZoomOffset[ch] + c->posBuffer[pos])))) * float ( h );

            if (drawEnvelope && bottom - top > 2)
                g.drawVerticalLine (currentX, top, bottom);

            if (drawTrace)
            {
                if (currentX == 0)
                    p.startNewSubPath ( float ( currentX ), mid);
                else
                    p.lineTo ( float ( currentX ), mid);
            }

            currentX++;
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

    for (auto c : channels)
    {
        c->posBuffer.clear ((size_t) c->bufferSize);
        c->minBuffer.clear ((size_t) c->bufferSize);
        c->maxBuffer.clear ((size_t) c->bufferSize);
    }
}

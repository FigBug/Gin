XYScope::XYScope (AudioFifo& f) :
    fifo (f)
{
    channel.xBuffer.clear ((size_t) channel.bufferSize);
    channel.yBuffer.clear ((size_t) channel.bufferSize);

    history.resize ((size_t) historySize);
    for (auto& frame : history)
        frame.resize ((size_t) blockSize);

    startTimerHz (60);
}

XYScope::~XYScope()
{
    stopTimer();
}

void XYScope::setNumSamplesPerPixel (const float newNumSamplesPerPixel)
{
    numSamplesPerPixel = newNumSamplesPerPixel;
}

void XYScope::setZoomFactor (const float newZoomFactor)
{
    zoomFactor = newZoomFactor;
}

void XYScope::setHistorySize (int numFrames)
{
    historySize = std::max (1, numFrames);
    history.resize ((size_t) historySize);
    for (auto& frame : history)
        frame.resize ((size_t) blockSize);
    currentHistoryIndex = 0;
}

void XYScope::setBlockSize (int size)
{
    blockSize = std::max (1, size);
    for (auto& frame : history)
        frame.resize ((size_t) blockSize);
}

void XYScope::addSamples (const juce::AudioSampleBuffer& buffer)
{
    jassert (buffer.getNumChannels() >= 2);

    const int numSamples = buffer.getNumSamples();

    // if we don't have enough space in the fifo, clear out some old samples
    const int numFreeInBuffer = channel.samplesToProcess.getFreeSpace();
    if (numFreeInBuffer < numSamples)
        channel.samplesToProcess.ensureFreeSpace (numSamples);

    channel.samplesToProcess.write (buffer);

    needToUpdate = true;
}

//==============================================================================

void XYScope::paint (juce::Graphics& g)
{
    if (needToUpdate)
    {
        needToUpdate = false;
        processPendingSamples();
        buildCurrentFrame();
    }

    render (g);

    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());
}

void XYScope::timerCallback()
{
    while (fifo.getNumReady() > 0)
    {
        ScratchBuffer buffer (2, std::min (512, fifo.getNumReady()));

        fifo.read (buffer);
        addSamples (buffer);
        repaint();
    }
}

//==============================================================================
void XYScope::processPendingSamples()
{
    int numSamples = channel.samplesToProcess.getNumReady();

    ScratchBuffer tempProcessingBlock (2, numSamples);
    channel.samplesToProcess.read (tempProcessingBlock);

    auto samplesL = tempProcessingBlock.getReadPointer (0);
    auto samplesR = tempProcessingBlock.getReadPointer (1);

    while (--numSamples >= 0)
    {
        const float currentSampleL = *samplesL++;
        const float currentSampleR = *samplesR++;

        channel.currentX += currentSampleL;
        channel.currentY += currentSampleR;
        channel.numAveraged++;

        if (--channel.numLeftToAverage <= 0)
        {
            channel.xBuffer[channel.bufferWritePos] = channel.currentX / float (channel.numAveraged);
            channel.yBuffer[channel.bufferWritePos] = channel.currentY / float (channel.numAveraged);

            channel.numAveraged = 0;
            channel.currentX = 0.0;
            channel.currentY = 0.0;

            ++channel.bufferWritePos %= channel.bufferSize;
            channel.numLeftToAverage += std::max (1.0f, numSamplesPerPixel);
        }
    }
}

void XYScope::buildCurrentFrame()
{
    const float w = static_cast<float> (getWidth());
    const float h = static_cast<float> (getHeight());

    if (w <= 0 || h <= 0)
        return;

    // Build current frame from buffer
    int bufferReadPos = channel.bufferWritePos - blockSize;
    if (bufferReadPos < 0)
        bufferReadPos += channel.bufferSize;

    auto& currentFrame = history[(size_t) currentHistoryIndex];

    for (int i = 0; i < blockSize; ++i)
    {
        int pos = (bufferReadPos + i) % channel.bufferSize;

        const float x = (0.5f + 0.5f * zoomFactor * channel.xBuffer[pos]) * w;
        const float y = (0.5f - 0.5f * zoomFactor * channel.yBuffer[pos]) * h;

        currentFrame[(size_t) i] = { x, y };
    }

    // Advance to next history slot
    currentHistoryIndex = (currentHistoryIndex + 1) % historySize;
}

void XYScope::render (juce::Graphics& g)
{
    g.fillAll (findColour (backgroundColourId));

    auto rc = getLocalBounds().toFloat();
    auto lineColour = findColour (lineColourId);
    auto traceColour = findColour (traceColourId);

    // Draw crosshairs
    g.setColour (lineColour.withAlpha (0.3f));
    g.drawLine (rc.getX(), rc.getCentreY(), rc.getRight(), rc.getCentreY(), 1.0f);
    g.drawLine (rc.getCentreX(), rc.getY(), rc.getCentreX(), rc.getBottom(), 1.0f);

    // Draw history frames with fading effect
    for (int i = 0; i < historySize; ++i)
    {
        // Calculate which frame to draw (oldest first)
        int frameIndex = (currentHistoryIndex + i) % historySize;
        const auto& frame = history[(size_t) frameIndex];

        if (frame.empty())
            continue;

        // Calculate alpha and line width based on age (using squared falloff for faster fade)
        float age = (i + 1.0f) / static_cast<float> (historySize);
        float lineWidth = age * 1.7f;
        float alpha = age * age * 0.7f;

        g.setColour (traceColour.withMultipliedAlpha (alpha));

        juce::Path p;

        for (int j = 0; j < blockSize; ++j)
        {
            if (j == 0)
                p.startNewSubPath (frame[(size_t) j]);
            else
                p.lineTo (frame[(size_t) j]);
        }

        g.strokePath (p, juce::PathStrokeType (lineWidth));
    }
}

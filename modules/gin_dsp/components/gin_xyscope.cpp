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
        channel.xBuffer[channel.bufferWritePos] = *samplesL++;
        channel.yBuffer[channel.bufferWritePos] = *samplesR++;

        ++channel.bufferWritePos %= channel.bufferSize;
        ++samplessinceLastFrame;

        // Build a new frame each time we have blockSize new samples
        if (samplessinceLastFrame >= blockSize)
        {
            samplessinceLastFrame = 0;
            buildCurrentFrame();
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

    // 45 degree rotation constants (sin and cos of 45°)
    constexpr float sn = 0.7071067811865476f;
    constexpr float cs = 0.7071067811865476f;

    const float cx = w * 0.5f;
    const float cy = h * 0.5f;
    const float scale = std::min (cx, cy) * zoomFactor;

    for (int i = 0; i < blockSize; ++i)
    {
        int pos = (bufferReadPos + i) % channel.bufferSize;

        const float l = channel.xBuffer[pos] * scale;
        const float r = channel.yBuffer[pos] * scale;

        // Apply 45° rotation: mono (L==R) becomes vertical, stereo width becomes horizontal
        const float x = (l * cs - r * sn) + cx;
        const float y = cy - (l * sn + r * cs);

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

        // Start from the last point of the previous frame to connect frames
        if (i > 0)
        {
            int prevFrameIndex = (currentHistoryIndex + i - 1) % historySize;
            const auto& prevFrame = history[(size_t) prevFrameIndex];
            if (! prevFrame.empty())
                p.startNewSubPath (prevFrame[(size_t) (blockSize - 1)]);
            else
                p.startNewSubPath (frame[0]);
        }
        else
        {
            p.startNewSubPath (frame[0]);
        }

        for (int j = 0; j < blockSize; ++j)
            p.lineTo (frame[(size_t) j]);

        g.strokePath (p.createPathWithRoundedCorners (3.0f), juce::PathStrokeType (lineWidth));
    }
}

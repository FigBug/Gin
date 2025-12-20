SpectrumAnalyzer::Channel::Channel()
{
    inputBuffer.resize (fftSize, 0.0f);
    fftData.resize (fftSize * 2, 0.0f);
    bins.resize (fftSize / 2, -100.0f);
    smoothedBins.resize (fftSize / 2, -100.0f);
}

SpectrumAnalyzer::SpectrumAnalyzer (AudioFifo& f)
    : fifo (f)
{
    setNumChannels (fifo.getNumChannels());

    for (int i = 0; i < 32; i++)
        setColour (traceColourId + i, juce::Colours::white);

    for (int i = 0; i < 32; i++)
        setColour (envelopeColourId + i, juce::Colours::white.withAlpha (0.5f));

    startTimerHz (30);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
    stopTimer();
}

void SpectrumAnalyzer::setNumChannels (int num)
{
    channels.clear();

    while (channels.size() < num)
        channels.add (new Channel());
}

void SpectrumAnalyzer::setFrequencyRange (float minFreq, float maxFreq)
{
    minFrequency = minFreq;
    maxFrequency = maxFreq;
}

void SpectrumAnalyzer::setDbRange (float minDb, float maxDb)
{
    minDecibels = minDb;
    maxDecibels = maxDb;
}

void SpectrumAnalyzer::setSampleRate (double sr)
{
    sampleRate = sr;
}

void SpectrumAnalyzer::setSmoothing (float smoothing)
{
    smoothingFactor = juce::jlimit (0.0f, 0.99f, smoothing);
}

void SpectrumAnalyzer::timerCallback()
{
    if (fifo.getNumChannels() != channels.size())
        setNumChannels (fifo.getNumChannels());

    bool hasNewData = false;

    while (fifo.getNumReady() > 0)
    {
        const int numToRead = std::min (512, fifo.getNumReady());
        ScratchBuffer buffer (fifo.getNumChannels(), numToRead);
        fifo.read (buffer);

        // Process each channel separately
        for (int ch = 0; ch < buffer.getNumChannels() && ch < channels.size(); ++ch)
        {
            auto* channel = channels[ch];

            for (int i = 0; i < numToRead; ++i)
            {
                channel->inputBuffer[channel->inputBufferPos] = buffer.getSample (ch, i);
                channel->inputBufferPos++;

                if (channel->inputBufferPos >= fftSize)
                {
                    channel->inputBufferPos = 0;
                    processFFT (channel);
                    hasNewData = true;
                }
            }
        }
    }

    if (hasNewData)
    {
        needToUpdate = true;
        repaint();
    }
}

void SpectrumAnalyzer::processFFT (Channel* channel)
{
    // Copy input to FFT buffer and apply window
    std::copy (channel->inputBuffer.begin(), channel->inputBuffer.end(), channel->fftData.begin());
    std::fill (channel->fftData.begin() + fftSize, channel->fftData.end(), 0.0f);

    window.multiplyWithWindowingTable (channel->fftData.data(), fftSize);

    // Perform FFT
    fft.performFrequencyOnlyForwardTransform (channel->fftData.data());

    // Convert to dB and store in bins
    // Normalize by FFT size to get proper magnitude values
    const int numBins = fftSize / 2;
    const float normFactor = 1.0f / static_cast<float> (fftSize);

    for (int i = 0; i < numBins; ++i)
    {
        const float magnitude = channel->fftData[i] * normFactor;
        const float db = juce::Decibels::gainToDecibels (magnitude, minDecibels);
        channel->bins[i] = db;

        // Apply smoothing
        channel->smoothedBins[i] = channel->smoothedBins[i] * smoothingFactor
                                 + channel->bins[i] * (1.0f - smoothingFactor);
    }
}

void SpectrumAnalyzer::updatePaths()
{
    const float w = static_cast<float> (getWidth());
    const float h = static_cast<float> (getHeight());

    if (w <= 0 || h <= 0)
        return;

    const int numBins = fftSize / 2;
    const float binWidth = static_cast<float> (sampleRate) / static_cast<float> (fftSize);

    // Use logarithmic frequency scale
    const float logMinFreq = std::log10 (minFrequency);
    const float logMaxFreq = std::log10 (maxFrequency);
    const float logFreqRange = logMaxFreq - logMinFreq;

    constexpr float stepSize = 2.0f;

    for (auto* channel : channels)
    {
        channel->path.clear();

        float lastY = 5000.0f;
        float lastX = -1000.0f;

        for (int bin = 1; bin < numBins; ++bin)
        {
            // Map bin to frequency
            const float freq = static_cast<float> (bin) * binWidth;

            if (freq < minFrequency || freq > maxFrequency)
                continue;

            // Map frequency to x position (logarithmic)
            const float logFreq = std::log10 (freq);
            const float normX = (logFreq - logMinFreq) / logFreqRange;
            const float x = normX * w;

            // Map dB to y position
            const float db = channel->smoothedBins[bin];
            const float clampedDb = juce::jlimit (minDecibels, maxDecibels, db);
            const float normDb = (clampedDb - minDecibels) / (maxDecibels - minDecibels);
            const float y = h - (normDb * h);

            lastY = std::min (lastY, y);

            if ((x - lastX) >= stepSize || bin == numBins - 1)
            {
                if (lastX < 0.0f)
                    channel->path.startNewSubPath (-2.0f, std::min (lastY, h - 0.5f) + 1.5f);
                else
                    channel->path.lineTo (x, std::min (lastY, h - 0.5f) + 1.5f);

                lastX = x;
                lastY = 5000.0f;
            }
        }

        // Smooth the path with rounded corners
        channel->path = channel->path.createPathWithRoundedCorners (3.0f);
    }
}

void SpectrumAnalyzer::paint (juce::Graphics& g)
{
    g.fillAll (findColour (backgroundColourId));

    if (needToUpdate)
    {
        needToUpdate = false;
        updatePaths();
    }

    int ch = 0;
    for (auto* channel : channels)
    {
        auto traceColour = findColour (traceColourId + ch);

        if (! traceColour.isTransparent())
        {
            g.setColour (traceColour);
            g.strokePath (channel->path, juce::PathStrokeType (1.5f));
        }

        ch++;
    }

    g.setColour (findColour (lineColourId));
    g.drawRect (getLocalBounds());
}

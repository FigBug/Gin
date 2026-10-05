/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
KWeightingFilter::Coefficients KWeightingFilter::stage1Coefficients (double sampleRate)
{
    // High shelf, +4 dB above ~1.5 kHz. Design values from the reference
    // implementation so the 48 kHz result matches BS.1770 Table 1 exactly.
    const double f0 = 1681.974450955533;
    const double gainDb = 3.999843853973347;
    const double q  = 0.7071752369554196;

    const double k  = std::tan (juce::MathConstants<double>::pi * f0 / sampleRate);
    const double vh = std::pow (10.0, gainDb / 20.0);
    const double vb = std::pow (vh, 0.4996667741545416);
    const double a0 = 1.0 + k / q + k * k;

    Coefficients c;
    c.b0 = (vh + vb * k / q + k * k) / a0;
    c.b1 = 2.0 * (k * k - vh) / a0;
    c.b2 = (vh - vb * k / q + k * k) / a0;
    c.a1 = 2.0 * (k * k - 1.0) / a0;
    c.a2 = (1.0 - k / q + k * k) / a0;
    return c;
}

KWeightingFilter::Coefficients KWeightingFilter::stage2Coefficients (double sampleRate)
{
    // High pass at ~38 Hz. As in the standard the numerator is left as
    // {1, -2, 1} rather than normalised.
    const double f0 = 38.13547087602444;
    const double q  = 0.5003270373238773;

    const double k  = std::tan (juce::MathConstants<double>::pi * f0 / sampleRate);
    const double a0 = 1.0 + k / q + k * k;

    Coefficients c;
    c.b0 = 1.0;
    c.b1 = -2.0;
    c.b2 = 1.0;
    c.a1 = 2.0 * (k * k - 1.0) / a0;
    c.a2 = (1.0 - k / q + k * k) / a0;
    return c;
}

void KWeightingFilter::prepare (double sampleRate, int numChannels)
{
    stage1.assign (size_t (numChannels), {});
    stage2.assign (size_t (numChannels), {});

    const auto c1 = stage1Coefficients (sampleRate);
    const auto c2 = stage2Coefficients (sampleRate);

    for (auto& b : stage1) b.c = c1;
    for (auto& b : stage2) b.c = c2;
}

void KWeightingFilter::reset()
{
    for (auto& b : stage1) b.z1 = b.z2 = 0.0;
    for (auto& b : stage2) b.z1 = b.z2 = 0.0;
}

void KWeightingFilter::process (juce::AudioBuffer<float>& buffer)
{
    const int n = juce::jmin (buffer.getNumChannels(), getNumChannels());
    for (int ch = 0; ch < n; ch++)
        process (buffer.getWritePointer (ch), buffer.getNumSamples(), ch);
}

void KWeightingFilter::process (float* samples, int numSamples, int channel)
{
    jassert (juce::isPositiveAndBelow (channel, getNumChannels()));

    auto& s1 = stage1[size_t (channel)];
    auto& s2 = stage2[size_t (channel)];

    for (int i = 0; i < numSamples; i++)
        samples[i] = s2.process (s1.process (samples[i]));
}

//==============================================================================
float LoudnessMeter::powerToLoudness (double power)
{
    if (power <= 1.0e-10)
        return silence;

    return juce::jmax (silence, float (-0.691 + 10.0 * std::log10 (power)));
}

double LoudnessMeter::loudnessToPower (float loudness)
{
    return std::pow (10.0, (double (loudness) + 0.691) / 10.0);
}

void LoudnessMeter::prepare (double sr, int channels, double maxHistorySeconds)
{
    sampleRate  = sr;
    numChannels = channels;

    filter.prepare (sr, channels);
    weighted.setSize (channels, 512);

    channelWeights.assign (size_t (channels), 1.0f);
    for (int ch = 3; ch < juce::jmin (channels, 5); ch++)
        channelWeights[size_t (ch)] = 1.41f;

    samplesPerBlock = juce::jmax (1, juce::roundToInt (sr / 10.0));
    blockSumSquares.assign (size_t (channels), 0.0);

    momentaryHistory.assign (size_t (juce::jmax (1, int (maxHistorySeconds * 10.0))), 0.0f);
    shortTermHistory.assign (size_t (juce::jmax (1, int (maxHistorySeconds))), 0.0f);

    applyReset();
}

void LoudnessMeter::setChannelWeight (int channel, float weight)
{
    if (juce::isPositiveAndBelow (channel, int (channelWeights.size())))
        channelWeights[size_t (channel)] = weight;
}

void LoudnessMeter::reset()
{
    resetPending.store (true, std::memory_order_release);
}

void LoudnessMeter::applyReset()
{
    filter.reset();
    std::fill (blockSumSquares.begin(), blockSumSquares.end(), 0.0);
    samplesInBlock = 0;

    std::fill (std::begin (ring), std::end (ring), 0.0);
    ringPos = ringCount = 0;

    momentaryCount.store (0, std::memory_order_release);
    shortTermCount.store (0, std::memory_order_release);
    blocksProcessed.store (0, std::memory_order_relaxed);

    momentary.store (silence, std::memory_order_relaxed);
    shortTerm.store (silence, std::memory_order_relaxed);
    maxMomentary.store (silence, std::memory_order_relaxed);
    maxShortTerm.store (silence, std::memory_order_relaxed);
}

void LoudnessMeter::process (const juce::AudioBuffer<float>& buffer)
{
    if (numChannels == 0)
        return;

    if (resetPending.exchange (false, std::memory_order_acq_rel))
        applyReset();

    const int channels = juce::jmin (numChannels, buffer.getNumChannels());
    const int total    = buffer.getNumSamples();

    int pos = 0;
    while (pos < total)
    {
        const int n = juce::jmin (total - pos, samplesPerBlock - samplesInBlock, weighted.getNumSamples());

        for (int ch = 0; ch < channels; ch++)
        {
            auto* w = weighted.getWritePointer (ch);
            juce::FloatVectorOperations::copy (w, buffer.getReadPointer (ch, pos), n);
            filter.process (w, n, ch);

            double sum = 0.0;
            for (int i = 0; i < n; i++)
                sum += double (w[i]) * double (w[i]);

            blockSumSquares[size_t (ch)] += sum;
        }

        samplesInBlock += n;
        pos += n;

        if (samplesInBlock >= samplesPerBlock)
            finishBlock();
    }
}

void LoudnessMeter::finishBlock()
{
    // Weighted mean square for this 100 ms block
    double power = 0.0;
    for (size_t ch = 0; ch < blockSumSquares.size(); ch++)
    {
        power += double (channelWeights[ch]) * blockSumSquares[ch] / double (samplesInBlock);
        blockSumSquares[ch] = 0.0;
    }
    samplesInBlock = 0;

    ring[ringPos] = power;
    ringPos = (ringPos + 1) % ringSize;
    ringCount = juce::jmin (ringCount + 1, ringSize);

    auto meanOfLast = [this] (int blocks)
    {
        blocks = juce::jmin (blocks, ringCount);
        double sum = 0.0;
        for (int i = 1; i <= blocks; i++)
            sum += ring[(ringPos - i + ringSize) % ringSize];
        return blocks > 0 ? sum / blocks : 0.0;
    };

    const double mPower = meanOfLast (4);
    const double sPower = meanOfLast (ringSize);

    const float m = powerToLoudness (mPower);
    const float s = powerToLoudness (sPower);

    momentary.store (m, std::memory_order_relaxed);
    shortTerm.store (s, std::memory_order_relaxed);

    if (m > maxMomentary.load (std::memory_order_relaxed)) maxMomentary.store (m, std::memory_order_relaxed);
    if (s > maxShortTerm.load (std::memory_order_relaxed)) maxShortTerm.store (s, std::memory_order_relaxed);

    const int blocks = blocksProcessed.load (std::memory_order_relaxed) + 1;
    blocksProcessed.store (blocks, std::memory_order_relaxed);

    // Momentary blocks need a full 400 ms behind them before they count
    if (ringCount >= 4)
    {
        const int count = momentaryCount.load (std::memory_order_relaxed);
        if (count < int (momentaryHistory.size()))
        {
            momentaryHistory[size_t (count)] = float (mPower);
            momentaryCount.store (count + 1, std::memory_order_release);
        }
    }

    // Short-term once a second, after the first full 3 s window
    if (ringCount >= ringSize && blocks % 10 == 0)
    {
        const int count = shortTermCount.load (std::memory_order_relaxed);
        if (count < int (shortTermHistory.size()))
        {
            shortTermHistory[size_t (count)] = float (sPower);
            shortTermCount.store (count + 1, std::memory_order_release);
        }
    }
}

float LoudnessMeter::getIntegrated() const
{
    const int count = momentaryCount.load (std::memory_order_acquire);
    if (count == 0)
        return silence;

    const double absoluteGate = loudnessToPower (-70.0f);

    // First pass: mean of everything above the absolute gate
    double sum = 0.0;
    int n = 0;
    for (int i = 0; i < count; i++)
    {
        const double p = momentaryHistory[size_t (i)];
        if (p > absoluteGate)
        {
            sum += p;
            n++;
        }
    }

    if (n == 0)
        return silence;

    // Second pass: mean of everything within 10 LU of that
    const double relativeGate = loudnessToPower (powerToLoudness (sum / n) - 10.0f);

    sum = 0.0;
    n = 0;
    for (int i = 0; i < count; i++)
    {
        const double p = momentaryHistory[size_t (i)];
        if (p > relativeGate)
        {
            sum += p;
            n++;
        }
    }

    return n > 0 ? powerToLoudness (sum / n) : silence;
}

float LoudnessMeter::getLoudnessRange() const
{
    const int count = shortTermCount.load (std::memory_order_acquire);
    if (count < 2)
        return 0.0f;

    const double absoluteGate = loudnessToPower (-70.0f);

    std::vector<float> values;
    values.reserve (size_t (count));

    double sum = 0.0;
    for (int i = 0; i < count; i++)
    {
        const float p = shortTermHistory[size_t (i)];
        if (p > absoluteGate)
        {
            values.push_back (p);
            sum += p;
        }
    }

    if (values.size() < 2)
        return 0.0f;

    const float relativeGate = float (loudnessToPower (powerToLoudness (sum / double (values.size())) - 20.0f));
    values.erase (std::remove_if (values.begin(), values.end(), [relativeGate] (float p) { return p <= relativeGate; }), values.end());

    if (values.size() < 2)
        return 0.0f;

    std::sort (values.begin(), values.end());

    auto percentile = [&values] (double pct)
    {
        const double idx = pct * double (values.size() - 1);
        const size_t lo = size_t (std::floor (idx));
        const size_t hi = juce::jmin (lo + 1, values.size() - 1);
        const double frac = idx - double (lo);
        return double (values[lo]) * (1.0 - frac) + double (values[hi]) * frac;
    };

    return juce::jmax (0.0f, powerToLoudness (percentile (0.95)) - powerToLoudness (percentile (0.10)));
}

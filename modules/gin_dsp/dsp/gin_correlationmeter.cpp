/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void CorrelationMeter::prepare (double sr, double window)
{
    sampleRate = sr;
    setWindow (window);
    ll = rr = lr = 0.0;
    correlation.store (0.0f, std::memory_order_relaxed);
    balance.store (0.0f, std::memory_order_relaxed);
}

void CorrelationMeter::setWindow (double window)
{
    windowSeconds = juce::jmax (0.001, window);
    coefficient.store (float (1.0 - std::exp (-1.0 / (sampleRate * windowSeconds))), std::memory_order_relaxed);
}

void CorrelationMeter::reset()
{
    resetPending.store (true, std::memory_order_release);
}

void CorrelationMeter::process (const juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() >= 2)
        process (buffer.getReadPointer (0), buffer.getReadPointer (1), buffer.getNumSamples());
    else if (buffer.getNumChannels() == 1)
        process (buffer.getReadPointer (0), buffer.getReadPointer (0), buffer.getNumSamples());
}

void CorrelationMeter::process (const float* left, const float* right, int numSamples)
{
    if (resetPending.exchange (false, std::memory_order_acq_rel))
        ll = rr = lr = 0.0;

    const double a = coefficient.load (std::memory_order_relaxed);
    const double b = 1.0 - a;

    for (int i = 0; i < numSamples; i++)
    {
        const double l = left[i];
        const double r = right[i];

        ll = b * ll + a * l * l;
        rr = b * rr + a * r * r;
        lr = b * lr + a * l * r;
    }

    const double denom = std::sqrt (ll * rr);
    const double c = denom > 1.0e-12 ? juce::jlimit (-1.0, 1.0, lr / denom) : 0.0;
    correlation.store (float (c), std::memory_order_relaxed);

    const double rl = std::sqrt (juce::jmax (0.0, ll));
    const double rrt = std::sqrt (juce::jmax (0.0, rr));
    const double sum = rl + rrt;
    balance.store (sum > 1.0e-6 ? float ((rrt - rl) / sum) : 0.0f, std::memory_order_relaxed);
}

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

//==============================================================================
void RMSMeter::prepare (double sampleRate, int channelCount, double window)
{
    numChannels   = channelCount;
    windowSeconds = window;

    const int length = juce::jmax (1, juce::roundToInt (sampleRate * window));

    channels.clear();
    for (int ch = 0; ch < channelCount; ch++)
    {
        auto c = std::make_unique<Channel>();
        c->squares.assign (size_t (length), 0.0f);
        channels.push_back (std::move (c));
    }
}

void RMSMeter::reset()
{
    resetPending.store (true, std::memory_order_release);
}

void RMSMeter::process (const juce::AudioBuffer<float>& buffer)
{
    if (resetPending.exchange (false, std::memory_order_acq_rel))
    {
        for (auto& c : channels)
        {
            std::fill (c->squares.begin(), c->squares.end(), 0.0f);
            c->sum = 0.0;
            c->pos = c->filled = 0;
            c->rms.store (0.0f, std::memory_order_relaxed);
        }
    }

    const int chans = juce::jmin (numChannels, buffer.getNumChannels());
    const int n     = buffer.getNumSamples();

    for (int ch = 0; ch < chans; ch++)
    {
        auto& c = *channels[size_t (ch)];
        const int length = int (c.squares.size());
        const float* src = buffer.getReadPointer (ch);

        for (int i = 0; i < n; i++)
        {
            const float sq = src[i] * src[i];
            c.sum += double (sq) - double (c.squares[size_t (c.pos)]);
            c.squares[size_t (c.pos)] = sq;

            if (++c.pos >= length)
            {
                c.pos = 0;

                // Resum once per wrap so float rounding can't accumulate
                double exact = 0.0;
                for (auto v : c.squares)
                    exact += v;
                c.sum = exact;
            }
        }

        c.filled = juce::jmin (c.filled + n, length);

        const double mean = c.filled > 0 ? juce::jmax (0.0, c.sum) / double (c.filled) : 0.0;
        c.rms.store (float (std::sqrt (mean)), std::memory_order_relaxed);
    }
}

float RMSMeter::getRMSLinear (int channel) const
{
    if (! juce::isPositiveAndBelow (channel, numChannels))
        return 0.0f;

    return channels[size_t (channel)]->rms.load (std::memory_order_relaxed);
}

float RMSMeter::getRMS (int channel) const
{
    return juce::Decibels::gainToDecibels (getRMSLinear (channel), silence);
}

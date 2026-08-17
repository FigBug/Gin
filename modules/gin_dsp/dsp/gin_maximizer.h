/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 Algorithm ported from 0xMaximizer.
 Copyright (c) 2006-2008 and 2012, Michael "LOSER" Gruhn. Used under the terms
 of its permission notice, which allows use, modification and distribution.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Zero crossing maximizer.

    Not a lookahead limiter. Audio is buffered a half cycle at a time - from
    one zero crossing to the next - and the peak of that half cycle is
    measured as it goes by. At the crossing, if the peak overshot the
    threshold, the whole half cycle that was just buffered is scaled back so
    its peak lands exactly on the threshold, and the output is caught up from
    the buffer.

    The point is where the gain changes happen: only ever at a zero crossing,
    where the signal is silent, so there is no step in the middle of a
    waveform to hear. A conventional limiter has to move its gain while the
    waveform is doing something, and spends attack and release time trying to
    make that inaudible.

    Latency is one buffer - see setMaxCycleLength(), which sets how long a
    half cycle can be before the maximizer gives up and restarts on it.

    Key Features:
    - Gain changes confined to zero crossings
    - Threshold and ceiling, with automatic makeup between them
    - Optional release, so gain recovers gradually across cycles
    - Per channel detection, so a peak in one does not duck the other

    Usage:
    @code
    Maximizer max;
    max.setSampleRate (44100.0);
    max.setNumChannels (2);
    max.setThreshold (-6.0f);       // scale half cycles back to -6 dB
    max.setCeiling (-0.3f);         // then bring the whole thing up to -0.3
    max.setRelease (0.25f);

    processor.setLatencySamples (max.getLatencySamples());

    max.process (buffer);
    @endcode

    @see Dynamics, TransientShaper
*/
class Maximizer
{
public:
    Maximizer() = default;
    ~Maximizer() = default;

    void setSampleRate (double sr)
    {
        sampleRate = sr;
        allocate();
        setRelease (releaseSeconds);
    }

    void setNumChannels (int ch)
    {
        channels = ch;
        allocate();
    }

    /** Level the half cycle peaks are scaled back to, in dB. */
    void setThreshold (float db)
    {
        threshold = juce::Decibels::decibelsToGain (db);
        recalcMakeup();
    }

    /** Output ceiling in dB. Everything between threshold and ceiling is made
        up automatically, which is where the loudness comes from. */
    void setCeiling (float db)
    {
        ceiling = juce::Decibels::decibelsToGain (db);
        recalcMakeup();
    }

    /** How long the gain takes to recover 6 dB, in seconds. 0 disables the
        release entirely, so every half cycle is scaled on its own merits. */
    void setRelease (float seconds)
    {
        releaseSeconds = seconds;
        releaseActive = seconds > 0.0f;
        release = releaseActive ? juce::Decibels::decibelsToGain (float (-6.0 / (seconds * sampleRate)))
                                : 1.0f;
    }

    /** The longest half cycle the maximizer will hold, in seconds, which is
        also its latency. The default 50 ms covers everything down to 10 Hz.
        Anything longer is restarted mid cycle, which is audible, so raise
        this rather than lower it if the source has real subsonic content. */
    void setMaxCycleLength (float seconds)
    {
        maxCycleLength = seconds;
        allocate();
    }

    /** Latency in samples. One slot short of the buffer, because the sample
        being written is the one read out a whole buffer later. */
    int getLatencySamples() const           { return std::max (0, bufferSize - 1); }

    void reset()
    {
        for (auto& c : chans)
        {
            std::fill (c.buffer.begin(), c.buffer.end(), 0.0f);
            c.length = 0;
            c.maxSample = 0.0f;
            c.gain = 1.0f;
            c.lastPositive = false;
        }
        bufferPos = 0;
    }

    void process (juce::AudioSampleBuffer& buffer)
    {
        juce::ScopedNoDenormals noDenormals;

        const int numSamples = buffer.getNumSamples();
        const int numChannels = std::min (channels, buffer.getNumChannels());

        if (bufferSize <= 0)
            return;

        for (int i = 0; i < numSamples; i++)
        {
            for (int ch = 0; ch < numChannels; ch++)
            {
                auto& c = chans[size_t (ch)];
                auto* data = buffer.getWritePointer (ch);

                auto in = data[i];
                c.buffer[size_t (bufferPos)] = in;

                const bool positive = in > 0.0f;
                c.maxSample = std::max (c.maxSample, std::abs (in));

                if (positive != c.lastPositive)
                {
                    // End of a half cycle: work out what it needed and apply
                    // that to every sample of it, retroactively.
                    auto needed = c.maxSample > threshold ? threshold / c.maxSample : 1.0f;
                    c.gain = std::min (needed, c.gain);

                    for (int n = c.length; n >= 0; n--)
                    {
                        auto idx = bufferPos - n;
                        if (idx < 0) idx += bufferSize;
                        c.buffer[size_t (idx)] *= c.gain;
                    }

                    c.length = -1;
                    c.maxSample = 0.0f;
                }

                if (++c.length >= bufferSize)
                {
                    // Longer than we can hold - start again rather than run off
                    // the end of the buffer
                    c.length = 0;
                    c.maxSample = 0.0f;
                }

                c.lastPositive = positive;
            }

            if (++bufferPos >= bufferSize)
                bufferPos = 0;

            for (int ch = 0; ch < numChannels; ch++)
            {
                auto& c = chans[size_t (ch)];
                auto* data = buffer.getWritePointer (ch);

                c.gain = releaseActive ? std::min (c.gain / release, 1.0f) : 1.0f;

                data[i] = juce::jlimit (-safetyLimit, safetyLimit, c.buffer[size_t (bufferPos)] * makeup);
            }
        }
    }

private:
    void allocate()
    {
        bufferSize = std::max (1, int (maxCycleLength * sampleRate));

        chans.resize (size_t (std::max (0, channels)));
        for (auto& c : chans)
            c.buffer.assign (size_t (bufferSize), 0.0f);

        reset();
    }

    void recalcMakeup()
    {
        makeup = threshold > 0.0f ? ceiling / threshold : 1.0f;
    }

    struct Channel
    {
        std::vector<float> buffer;
        int length = 0;
        float maxSample = 0.0f;
        float gain = 1.0f;
        bool lastPositive = false;
    };

    static constexpr float safetyLimit = 10.0f;

    double sampleRate = 44100.0;
    int channels = 0;
    int bufferSize = 0, bufferPos = 0;

    float maxCycleLength = 0.05f;
    float threshold = 1.0f, ceiling = 1.0f, makeup = 1.0f;
    float releaseSeconds = 0.0f, release = 1.0f;
    bool releaseActive = false;

    std::vector<Channel> chans;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Maximizer)
};

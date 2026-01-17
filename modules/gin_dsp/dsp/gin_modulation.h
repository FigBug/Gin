/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */

#pragma once

//==============================================================================
/**
    Modulated delay effect for chorus, flanger, and phaser effects.

    Modulation uses an LFO to modulate a delay line, creating time-varying delay
    effects. By adjusting delay time, modulation rate, depth, and stereo width,
    you can create chorus, flanger, and phaser-like effects.

    Key Features:
    - Sinusoidal LFO modulation
    - Stereo width control (independent L/R modulation)
    - Configurable delay time, rate, and depth
    - Wet/dry mix control
    - High-quality Lagrange interpolation

    Parameters:
    - delay: Base delay time in seconds (typically 10-30ms)
    - rate: LFO frequency in Hz (typically 0.1-5 Hz)
    - depth: Modulation depth in seconds (how much delay varies)
    - width: Stereo width (0.0 = mono, 1.0 = full stereo L/R phase offset)
    - mix: Wet/dry mix (0.0 = dry, 1.0 = wet)

    Effect Types:
    - Chorus: delay ~20ms, rate ~0.5Hz, depth ~10ms, mix ~0.5
    - Flanger: delay ~3ms, rate ~0.2Hz, depth ~5ms, mix ~0.5
    - Vibrato: delay ~5ms, rate ~5Hz, depth ~2ms, mix ~1.0

    Usage:
    @code
    Modulation chorus(0.03f); // 30ms max delay
    chorus.setSampleRate(44100.0);

    // Create a chorus effect
    chorus.setParams(
        0.020f,  // 20ms base delay
        0.5f,    // 0.5 Hz modulation
        0.010f,  // 10ms depth
        0.5f,    // 50% stereo width
        0.5f     // 50% wet/dry mix
    );

    chorus.process(audioBuffer);
    @endcode

    @see DelayLine
*/
class Modulation
{
public:
    Modulation (float maxDelay_ = 30.0f / 1000.0f)
        : maxDelay (maxDelay_)
    {
    }

    void setSampleRate (double sr)
    {
        sampleRate = sr;

        delayLine.setSize (2, (maxDelay + depth) + 0.1, sampleRate);
        delayLine.clear();
        phase = 0.0f;
    }

    void reset()
    {
        phase = 0;
        delayLine.clear();
    }

    void setParams (float delay_, float rate_, float depth_, float width_, float mix_)
    {
        delay = delay_;
        rate  = rate_;
        depth = depth_;
        width = width_;
        mix   = mix_;
    }
    
    void process (juce::AudioSampleBuffer& buffer)
    {
        WetDryMix wetDry (mix);

        float delta = float (rate / sampleRate);

        int numSamples = buffer.getNumSamples ();
        auto l = buffer.getWritePointer (0);
        auto r = buffer.getWritePointer (1);

        for (int i = 0; i < numSamples; ++i)
        {
            float totalDelayL = delay + depth / 2.0f + depth * std::sin (juce::MathConstants<float>::pi * 2 * (phase + width / 2.0f)) / 2.0f;
            float totalDelayR = delay + depth / 2.0f + depth * std::sin (juce::MathConstants<float>::pi * 2 * (phase - width / 2.0f)) / 2.0f;

            delayLine.write (0, l[i]);
            delayLine.write (1, r[i]);
            delayLine.writeFinished();

            l[i] = delayLine.readLagrange (0, totalDelayL) * wetDry.wetGain + l[i] * wetDry.dryGain;
            r[i] = delayLine.readLagrange (1, totalDelayR) * wetDry.wetGain + r[i] * wetDry.dryGain;

            phase += delta;
            if (phase >= 1.0f)
                phase = 0.0f;
        }
    }

private:
    const float maxDelay;

    DelayLine delayLine {2};

    double sampleRate = 44100;

    float phase = 0.0f;
    float delay = 0.02f, rate = 1.0f, depth = 0.03f, width = 0.5f, mix = 0.0f;
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Anti-aliased bitcrusher with independent bit depth and sample rate reduction.

    BitCrusher combines two classic lo-fi effects with optional anti-aliasing:

    Bit depth reduction quantizes the amplitude of the signal to a reduced
    number of bits (1 to 16). Quantization is a static nonlinearity that
    generates harmonics which alias back into the audible range, so the
    quantizer runs inside a 4x juce::dsp::Oversampling stage - the signal is
    upsampled, quantized at the oversampled rate, then filtered and decimated
    back down.

    Sample rate reduction resamples the signal to a lower rate (e.g. 500Hz to
    48kHz) with a sample & hold driven by a phase accumulator, like a sync'd
    oscillator. Each hold transition is a step discontinuity which would
    normally cause fold-back aliasing. Instead of naively holding values, a
    polyBLEP correction is injected around each discontinuity (across the
    samples before and after the step, via a one sample delay line), giving a
    band-limited step that keeps the audible stepped character without the
    harsh fold-back. The held value is sampled at the exact step instant by
    linear interpolation of the input.

    Signal chain: oversample -> quantize -> decimate -> BLEP sample & hold.
    The sample & hold runs last, at the project rate, since the BLEP correction
    band-limits the steps relative to the output Nyquist, and holding after
    quantization keeps the steps exactly on the bit grid.

    Dirty mode bypasses all anti-aliasing (naive quantization and naive hold,
    zero latency) for the raw aliased sound of a classic bitcrusher.

    Key Features:
    - Bit depth reduction (1 to 16 bits), anti-aliased via 4x oversampling
    - Sample rate reduction via phase accumulator sample & hold
    - polyBLEP band-limited steps to suppress fold-back aliasing
    - Dirty mode toggle for the classic raw aliased character
    - Smoothed parameter changes
    - Latency reporting for host compensation

    Usage:
    @code
    BitCrusher crusher;
    crusher.setSampleRate (44100.0);

    // 8 bits, hold at 6kHz, anti-aliased
    crusher.setParams (8.0f, 6000.0f, false);
    crusher.process (buffer);

    // raw lo-fi: naive quantization and hold
    crusher.setParams (8.0f, 6000.0f, true);
    crusher.process (buffer);

    // report crusher.getLatencySamples() to the host
    @endcode

    @see ValueSmoother, AirWindowsDistortion
*/
class BitCrusher
{
public:
    BitCrusher (int maxChannels_ = 2, int maxBlockSize_ = 4096)
        : maxChannels (maxChannels_), maxBlockSize (maxBlockSize_),
          oversampling (size_t (maxChannels_), overOrder,
                        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,
                        true, true)
    {
        oversampling.initProcessing (size_t (maxBlockSize));

        heldValue.resize (size_t (maxChannels));
        prevInput.resize (size_t (maxChannels));
        delayed.resize (size_t (maxChannels));

        bitsSmoother.setTime (0.02);
        bitsSmootherOS.setTime (0.02);
        rateSmoother.setTime (0.02);

        setSampleRate (44100.0);
    }

    /** Sets the sample rate. Resets the effect. */
    void setSampleRate (double sr)
    {
        sampleRate = sr;
        bitsSmoother.setSampleRate (sr);
        bitsSmootherOS.setSampleRate (sr * (1 << overOrder));
        rateSmoother.setSampleRate (sr);
        setParams (targetBits, targetRate, dirty);
        reset();
    }

    /** Sets the parameters.
        @param bitDepth   Bits of amplitude resolution (1 to 16)
        @param rateHz     Target sample & hold rate in Hz. At or above the
                          project rate the signal passes through unheld.
        @param dirtyMode  true bypasses all anti-aliasing for the classic
                          raw aliased sound
    */
    void setParams (float bitDepth, float rateHz, bool dirtyMode)
    {
        targetBits = bitDepth;
        targetRate = rateHz;
        dirty = dirtyMode;

        // smoothers work on normalized 0-1 values
        bitsSmoother.setValue (bitDepth / maxBits);
        bitsSmootherOS.setValue (bitDepth / maxBits);
        rateSmoother.setValue (float (rateHz / sampleRate));
    }

    /** Resets all internal state. */
    void reset()
    {
        oversampling.reset();

        phase = 1.0f;

        std::fill (heldValue.begin(), heldValue.end(), 0.0f);
        std::fill (prevInput.begin(), prevInput.end(), 0.0f);
        std::fill (delayed.begin(), delayed.end(), 0.0f);

        bitsSmoother.snapToValue();
        bitsSmootherOS.snapToValue();
        rateSmoother.snapToValue();
    }

    /** Latency in samples for the current mode. Zero in dirty mode, otherwise
        the oversampling filter latency plus one sample for the BLEP delay.
        Changes when dirty mode is toggled. */
    int getLatencySamples() const
    {
        return dirty ? 0 : juce::roundToInt (oversampling.getLatencyInSamples()) + 1;
    }

    /** Processes a buffer of audio. Channels beyond maxChannels are left
        untouched. Buffers larger than maxBlockSize are processed in chunks. */
    void process (juce::AudioSampleBuffer& buffer)
    {
        if (dirty != lastDirty)
        {
            lastDirty = dirty;
            oversampling.reset();
            delayed = heldValue;
            prevInput = heldValue;
        }

        int pos  = 0;
        int todo = buffer.getNumSamples();

        while (todo > 0)
        {
            int n = std::min (todo, maxBlockSize);

            juce::AudioSampleBuffer slice (buffer.getArrayOfWritePointers(),
                                           buffer.getNumChannels(), pos, n);
            if (dirty)
                processNaive (slice);
            else
                processAntiAliased (slice);

            pos  += n;
            todo -= n;
        }
    }

private:
    static float quantise (float v, float bits)
    {
        auto scale = std::exp2 (bits - 1.0f);
        return std::round (v * scale) / scale;
    }

    void processNaive (juce::AudioSampleBuffer& buffer)
    {
        int numSamples  = buffer.getNumSamples();
        int numChannels = std::min (buffer.getNumChannels(), maxChannels);

        // keep the oversampled smoother in step so mode switches don't jump
        bitsSmootherOS.process (numSamples << overOrder);

        for (int i = 0; i < numSamples; i++)
        {
            auto bits = bitsSmoother.getNextValue() * maxBits;
            auto inc  = rateSmoother.getNextValue();

            phase += inc;

            bool step = phase >= 1.0f;
            if (step)
                phase -= std::floor (phase);

            for (int ch = 0; ch < numChannels; ch++)
            {
                auto* d = buffer.getWritePointer (ch);

                if (step)
                    heldValue[size_t (ch)] = quantise (d[i], bits);

                d[i] = heldValue[size_t (ch)];
            }
        }
    }

    void processAntiAliased (juce::AudioSampleBuffer& buffer)
    {
        int numSamples  = buffer.getNumSamples();
        int numChannels = std::min (buffer.getNumChannels(), maxChannels);

        // quantize at the oversampled rate, then filter / decimate back down
        {
            juce::dsp::AudioBlock<float> block (buffer.getArrayOfWritePointers(),
                                                size_t (numChannels), size_t (numSamples));

            auto os = oversampling.processSamplesUp (block);

            for (size_t i = 0; i < os.getNumSamples(); i++)
            {
                auto bits = bitsSmootherOS.getNextValue() * maxBits;

                for (size_t ch = 0; ch < os.getNumChannels(); ch++)
                    os.setSample (int (ch), int (i), quantise (os.getSample (int (ch), int (i)), bits));
            }

            oversampling.processSamplesDown (block);

            bitsSmoother.process (numSamples);
        }

        // BLEP-corrected sample & hold at the project rate
        for (int i = 0; i < numSamples; i++)
        {
            auto inc = rateSmoother.getNextValue();

            if (inc >= 1.0f)
            {
                // hold rate at or above the project rate: pass through,
                // keeping the one sample delay so latency stays constant
                phase = 1.0f;

                for (int ch = 0; ch < numChannels; ch++)
                {
                    auto* d  = buffer.getWritePointer (ch);
                    auto in  = d[i];

                    d[i] = delayed[size_t (ch)];

                    delayed[size_t (ch)]   = in;
                    heldValue[size_t (ch)] = in;
                    prevInput[size_t (ch)] = in;
                }
                continue;
            }

            phase += inc;

            bool step = phase >= 1.0f;
            float t = 0.0f;

            if (step)
            {
                phase -= 1.0f;
                // fraction of a sample since the step occurred, 0 to 1
                t = juce::jlimit (0.0f, 1.0f, phase / inc);
            }

            for (int ch = 0; ch < numChannels; ch++)
            {
                auto* d  = buffer.getWritePointer (ch);
                auto in  = d[i];
                auto out = heldValue[size_t (ch)];

                if (step)
                {
                    // input value at the exact step instant
                    auto newHeld = prevInput[size_t (ch)] + (1.0f - t) * (in - prevInput[size_t (ch)]);
                    auto amt     = newHeld - heldValue[size_t (ch)];

                    // polyBLEP residual: the sample before the step is still
                    // in the delay line, the sample after is this one
                    delayed[size_t (ch)] += 0.5f * amt * t * t;
                    out = newHeld - 0.5f * amt * (1.0f - t) * (1.0f - t);

                    heldValue[size_t (ch)] = newHeld;
                }

                d[i] = delayed[size_t (ch)];
                delayed[size_t (ch)]   = out;
                prevInput[size_t (ch)] = in;
            }
        }
    }

    static constexpr int overOrder = 2; // 2^2 = 4x oversampling
    static constexpr float maxBits = 16.0f;

    int maxChannels = 2;
    int maxBlockSize = 4096;

    double sampleRate = 44100.0;
    float targetBits = 16.0f, targetRate = 44100.0f;
    bool dirty = false, lastDirty = false;

    juce::dsp::Oversampling<float> oversampling;

    ValueSmoother<float> bitsSmoother, bitsSmootherOS, rateSmoother;

    float phase = 1.0f;
    std::vector<float> heldValue, prevInput, delayed;
};

/*
==============================================================================

This file is part of the GIN library.
Copyright (c) 2018 - 2026 by Roland Rabien.

==============================================================================
*/

#pragma once
//================================================================================
/**
    Envelope follower for dynamics processing.

    EnvelopeDetector tracks the amplitude envelope of an audio signal using
    configurable attack, hold, and release times. It supports multiple detection
    modes (peak, mean-square, RMS) and can use analog-style or digital time
    constants for natural or precise envelope following.

    Key Features:
    - Three detection modes: peak, MS, RMS
    - Attack, hold, and release time controls
    - Analog or digital time constants
    - Optional logarithmic detector for dB domain
    - Sample-by-sample processing
    - Used by compressors, gates, expanders, limiters

    The envelope detector is the core component for gain reduction calculations
    in dynamics processors, providing a smooth representation of signal level
    over time.

    @see Dynamics, LevelTracker
*/
class EnvelopeDetector
{
public:
    EnvelopeDetector() = default;
    ~EnvelopeDetector() = default;

    /**
        Envelope detection modes.

        Determines how the input signal is converted to an envelope value:
        - peak: Track the absolute peak value
        - ms: Mean square (squared values, more sensitive to transients)
        - rms: Root mean square (true RMS level detection)
    */
    enum Mode
    {
        peak,  ///< Peak detection (absolute value)
        ms,    ///< Mean square detection
        rms    ///< RMS (root mean square) detection
    };

    void setSampleRate (double f)           { sampleRate = f; setRMSWindow (rmsWindow); }

    void setParams (float attackS, float holdS, float releaseS, bool analogTC, Mode detect, bool logDetector);

    /** Averaging window for the ms and rms modes, in seconds.

        This is what makes RMS detection mean anything: the squared input is
        averaged over the window before the attack and release stages see it,
        so the detector responds to how much energy is in the signal rather
        than to individual samples. Short windows (1 ms) approach peak
        detection, long ones (50 ms+) ignore transients almost entirely, which
        is the loudness-following behaviour a bus compressor wants.

        Has no effect in peak mode.
    */
    void setRMSWindow (float seconds);

    void reset();
    float process (float input);

protected:
    void setAttackTime (float attackS);
    void setHoldTime (float holdS);
    void setReleaseTime (float releaseS);

    double sampleRate = 44100.0;
    Mode mode = peak;
    float attackTime = 0.0f, releaseTime = 0.0f, envelope = 0.0;
    float holdTime = 0.0f, holdRemaining = 0.0f;
    bool analogTC = false, logDetector = false;
    float rmsWindow = 0.010f, rmsA = 1.0f, rmsB = 0.0f, rmsState = 0.0f;
};

//================================================================================
/**
    Dynamics processor supporting compression, limiting, expansion, and gating.

    Dynamics provides a versatile dynamics processing engine that can function as
    a compressor, limiter, expander, or gate. Based on algorithms from "Designing
    Audio Effect Plug-ins in C++" by Will Pirkle, it features attack/hold/release
    envelope detection, adjustable ratio and threshold, soft knee, and optional
    channel linking for stereo operation.

    The compexp mode combines downward compression (above threshold) with upward
    compression (below the inverted threshold), using the same ratio for both.

    Key Features:
    - Five processing types: compressor, limiter, expander, gate, compexp
    - Configurable attack, hold, release times
    - Adjustable threshold and ratio
    - Soft knee for smooth compression
    - Independent input/output gain
    - Stereo linking (process based on max of both channels)
    - Side chain input for ducking, de-essing and source keyed gating
    - Optional envelope output for visualization
    - Level tracking for input/output metering

    Usage:
    @code
    Dynamics comp;
    comp.setSampleRate(44100.0);
    comp.setNumChannels(2);
    comp.setMode(Dynamics::compressor);
    comp.setLinked(true); // Link stereo channels

    // Set compression parameters
    comp.setParams(0.01f,   // 10ms attack
                   0.0f,     // no hold
                   0.1f,     // 100ms release
                   -20.0f,   // -20dB threshold
                   4.0f,     // 4:1 ratio
                   6.0f);    // 6dB soft knee

    comp.setInputGain(0.0f);   // 0dB input
    comp.setOutputGain(3.0f);  // +3dB makeup gain

    comp.process(audioBuffer);

    // keyed off something else: duck the music under the voiceover
    comp.process(musicBuffer, voiceBuffer);

    // Get metering
    float inputLevel = comp.getInputTracker().getLevel();
    float outputLevel = comp.getOutputTracker().getLevel();
    @endcode

    @see EnvelopeDetector, LevelTracker
*/
class Dynamics
{
public:
    Dynamics() = default;
    ~Dynamics() = default;

    /**
        Dynamics processing types.

        Determines the type of dynamics processing applied:
        - compressor: Reduces gain above threshold (ratio > 1:1)
        - limiter: Hard limiting (very high ratio, fast attack)
        - expander: Increases dynamic range (reduces gain below threshold)
        - gate: Attenuates signals below threshold (extreme expansion)
        - compexp: Combined compressor + expander (expander threshold is inverse of compressor threshold)
    */
    enum Type
    {
        compressor,  ///< Compression (reduce loud signals)
        limiter,     ///< Limiting (prevent peaks above threshold)
        expander,    ///< Expansion (increase dynamic range)
        gate,        ///< Gate (cut quiet signals)
        compexp,     ///< Combined compressor and expander (expander uses inverse threshold)
    };

    void setSampleRate (double sampleRate);
    void setNumChannels (int ch);
    void setMode (Type t)                   { type = t;             }
    void setLinked (bool l)                 { channelsLinked = l;   }

    void setParams (float attackS, float holdS, float releaseS, float thresh, float ratio, float kneeWidth);

    void setInputGain (float g)             { inputGain = g;    }
    void setOutputGain (float g)            { outputGain = g;   }

    /** How the detector measures level: peak, mean square or RMS.
        @see EnvelopeDetector::Mode, setRMSWindow
    */
    void setDetectorMode (EnvelopeDetector::Mode m);

    /** Averaging window for the ms and rms detector modes, in seconds.
        @see EnvelopeDetector::setRMSWindow
    */
    void setRMSWindow (float seconds);

    /** Analog rather than digital time constants.

        Digital constants reach 99% of the target in the time you asked for,
        analog ones 63% - the same curve a real capacitor charging through a
        resistor follows. Analog is slower to arrive and sounds gentler on the
        attack; digital does what the number says.
    */
    void setAnalogTC (bool analog);

    /** Delays the audio so the detector sees each transient before the gain
        does, in seconds. Without it, an attack fast enough to catch a peak
        has already let the front of that peak through.

        Costs latency, which the host has to be told about - see
        getLatencySamples(). 5 ms is plenty for most material.
    */
    void setLookahead (float seconds);

    /** Lookahead delay in samples, for AudioProcessor::setLatencySamples(). */
    int getLatencySamples() const           { return lookaheadSamples; }

    /** Filters the detector's view of the signal without touching what is
        heard. A high pass stops kick drums pumping the whole mix; a low pass
        stops cymbals doing the same.

        @param highpassHz   detector high pass, 0 for none
        @param lowpassHz    detector low pass, 0 for none
    */
    void setSidechainFilter (float highpassHz, float lowpassHz);

    /** A peak in the detector path, again heard only through what it makes
        the gain do. Boost around 6 kHz and the compressor turns into a
        de-esser; boost the low mids and it leans on boxiness.

        @param freqHz   centre frequency
        @param q        width
        @param gainDb   boost or cut, 0 disables the filter
    */
    void setDetectorPeak (float freqHz, float q, float gainDb);

    /** Enables automatic makeup gain for compressor/limiter modes.
        When enabled, gain is automatically applied to compensate for
        the gain reduction, bringing peaks back to 0dB.
        @param enabled  True to enable auto makeup gain
    */
    void setAutoMakeupGain (bool enabled)   { autoMakeupGain = enabled; }

    void reset();

    /** Processes a buffer, with the detector following that same buffer.

        @param buffer       audio to process, in place
        @param envelopeOut  optional detector envelope, as a linear level.
                            Written to channel 0 when linked and to every
                            channel otherwise, so it needs as many channels
                            as the processor has.
    */
    void process (juce::AudioSampleBuffer& buffer, juce::AudioSampleBuffer* envelopeOut = nullptr);

    /** Processes a buffer, with the detector following a separate side chain.

        The gain is worked out from `sidechain` and applied to `buffer`:
        ducking, de-essing, and gating something on a source other than
        itself. A gated reverb is the case that shows why it matters - keyed
        off its own output, a gate can only chase an envelope the reverb has
        already smeared, so it opens softly instead of on the transient and
        its timing shifts whenever the decay does.

        @param buffer       audio to process, in place
        @param sidechain    what the detector follows. Needs at least as many
                            samples as `buffer`; fewer channels is fine, the
                            last one is reused, so a mono key drives them all.
                            Input gain scales what the detector sees here just
                            as it does without a side chain.
        @param envelopeOut  optional detector envelope, as above
    */
    void process (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer& sidechain, juce::AudioSampleBuffer* envelopeOut = nullptr);

    const LevelTracker& getInputTracker()       { return inputTracker;      }
    const LevelTracker& getOutputTracker()      { return outputTracker;     }
    const LevelTracker& getReductionTracker()   { return reductionTracker;  }

    float calcCurve (float detectorValue);

private:
    void processInternal (juce::AudioSampleBuffer& buffer, const juce::AudioSampleBuffer* sidechain, juce::AudioSampleBuffer* envelopeOut);

    void updateDetectorParams();
    void updateDetectorFilters (int numChannels);
    void updateLookahead();

    /** The detector's filters, one set per channel it has to key from. */
    struct DetectorFilters
    {
        Biquad highpass, lowpass, peak;
    };

    juce::OwnedArray<EnvelopeDetector> envelopes;
    LevelTracker inputTracker, outputTracker, reductionTracker {-30.0f};

    double sampleRate = 44100.0;
    Type type = compressor;

    int channels = 0;
    bool channelsLinked = true;
    bool autoMakeupGain = false;
    float inputGain = 1.0f, outputGain = 1.0f;
    float threshold = 0.0f, ratio = 0.0f, kneeWidth = 0.0f;

    float attackSeconds = 0.0f, holdSeconds = 0.0f, releaseSeconds = 0.0f;
    EnvelopeDetector::Mode detectorMode = EnvelopeDetector::peak;
    float rmsWindow = 0.010f;
    bool analogTC = false;

    float lookaheadSeconds = 0.0f;
    int lookaheadSamples = 0, lookaheadPos = 0;
    std::vector<std::vector<float>> lookaheadBuffers;

    float sidechainHighpass = 0.0f, sidechainLowpass = 0.0f;
    float detectorPeakFreq = 1000.0f, detectorPeakQ = 1.0f, detectorPeakGain = 0.0f;
    bool detectorFilterActive = false;
    std::vector<DetectorFilters> detectorFilters;
};

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

    void setSampleRate (double f)           { sampleRate = f;       }

    void setParams (float attackS, float holdS, float releaseS, bool analogTC, Mode detect, bool logDetector);

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

    /** Enables automatic makeup gain for compressor/limiter modes.
        When enabled, gain is automatically applied to compensate for
        the gain reduction, bringing peaks back to 0dB.
        @param enabled  True to enable auto makeup gain
    */
    void setAutoMakeupGain (bool enabled)   { autoMakeupGain = enabled; }

    void reset();
    void process (juce::AudioSampleBuffer& buffer, juce::AudioSampleBuffer* envelopeOut = nullptr);

    const LevelTracker& getInputTracker()       { return inputTracker;      }
    const LevelTracker& getOutputTracker()      { return outputTracker;     }
    const LevelTracker& getReductionTracker()   { return reductionTracker;  }

    float calcCurve (float detectorValue);

private:
    juce::OwnedArray<EnvelopeDetector> envelopes;
    LevelTracker inputTracker, outputTracker, reductionTracker {-30.0f};

    double sampleRate = 44100.0;
    Type type = compressor;

    int channels = 0;
    bool channelsLinked = true;
    bool autoMakeupGain = false;
    float inputGain = 1.0f, outputGain = 1.0f;
    float threshold = 0.0f, ratio = 0.0f, kneeWidth = 0.0f;
};

/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/**
    Equation parser with audio-specific functions for DSP processing.

    AudioEquationParser extends EquationParser with audio and synthesis functions,
    allowing users to create custom audio processing algorithms using mathematical
    expressions. It provides oscillators, filters, utilities, and audio constants
    that can be evaluated in real-time for creative sound design.

    Key Features:
    - Standard oscillators (sine, saw, square, triangle)
    - Synthesis filters (lowpass, highpass, bandpass, notch)
    - Effect filters with resonance control
    - Audio constants (sample rate, pi, e, etc.)
    - Utility functions (clamp, wrap, fold, etc.)
    - Real-time expression evaluation
    - Stateful audio functions (filters maintain state)

    The parser is particularly useful for:
    - Custom waveshaping algorithms
    - User-programmable oscillators
    - Dynamic filter equations
    - Mathematical modulation sources
    - Educational audio programming tools

    Usage:
    @code
    AudioEquationParser parser;
    parser.addConstants();
    parser.addUtilities();
    parser.addOscillatorFunctions();
    parser.addSynthFilterFunctions();

    // Parse a custom oscillator equation
    parser.setEquation("sin(phase * 2 * pi) * 0.5 + saw(phase) * 0.5");

    // Evaluate for each sample
    for (int i = 0; i < numSamples; i++)
    {
        float phase = (float)i / sampleRate;
        parser.setVariable("phase", phase);
        float output = parser.evaluate();
        buffer[i] = output;
    }

    // Create a filtered oscillator
    parser.setEquation("lp24(saw(phase), cutoff, resonance)");
    parser.setVariable("cutoff", 1000.0f);
    parser.setVariable("resonance", 0.7f);
    @endcode

    @see EquationParser, AudioFunctionHost
*/
class AudioEquationParser : public EquationParser,
                            public AudioFunctionHost
{
public:
    AudioEquationParser() = default;

    /** Adds audio constants like pi, sampleRate, e, etc. */
    void addConstants();

    /** Adds utility functions like clamp, wrap, fold, abs, etc. */
    void addUtilities();

    /** Adds oscillator functions: sine, saw, square, triangle, noise, etc. */
    void addOscillatorFunctions();

    /** Adds synthesis filters: lp12, lp24, hp12, hp24, bp12, bp24, notch12, notch24. */
    void addSynthFilterFunctions();

    /** Adds effect-style filters with resonance control for creative processing. */
    void addEffectFilterFunctions();
};

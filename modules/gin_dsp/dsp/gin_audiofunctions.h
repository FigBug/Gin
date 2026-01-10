/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2018 - 2026 by Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
    Base class for storing state in audio functions.

    FuncState provides a base for stateful audio functions used in AudioEquationParser.
    While pure functions don't have state, audio filters and oscillators need to
    maintain internal state between samples (filter histories, oscillator phases, etc.).
    This class manages that state and provides sample rate change notification.

    All audio function state classes (filter states, oscillator states) derive from
    this base to provide consistent state management and sample rate handling.

    @see AudioFunctionHost, AudioEquationParser
*/
struct FuncState
{
    FuncState (double sr) : sampleRate (sr) {}
    virtual ~FuncState() = default;
    virtual void setSampleRate (double sr) { sampleRate = sr; }
    virtual void reset() {}
    double sampleRate = 44100.0;
};

//==============================================================================
/**
    State storage for oscillator functions in AudioEquationParser.

    OscState maintains the phase accumulator and frequency conversion for
    oscillator functions. It automatically converts MIDI note numbers to Hz
    and calculates phase increment for accurate oscillation at the current
    sample rate.

    @see AudioFunctionHost, AudioEquationParser
*/
struct OscState : public FuncState
{
    OscState (double sr) : FuncState (sr) {}

    float phase { juce::Random::getSystemRandom().nextFloat() };

    inline void incPhase (float note)
    {
        if (! juce::approximatelyEqual (note, lastNote))
        {
            lastNote = note;

            frequency = gin::getMidiNoteInHertz (note);
            const float period = 1.0f / frequency;
            const float periodInSamples = float (period * sampleRate);
            delta = 1.0f / periodInSamples;

            jassert (delta > 0);
        }
        phase += delta;
        if (phase > 1.0f)
            phase -= 1.0f;

        jassert (! std::isinf (phase));
    }

    void reset() override
    {
        phase = juce::Random::getSystemRandom().nextFloat();
    }

    float lastNote = -1.0f, frequency = -1.0f, delta = -1.0f;
};

//==============================================================================
/**
    State storage for noise generator in AudioEquationParser.

    NoiseState wraps a WhiteNoise generator for use in equation-based audio
    processing, providing random values for noise functions.

    @see WhiteNoise, AudioFunctionHost
*/
struct NoiseState : public FuncState
{
    NoiseState (double sr) : FuncState (sr) {}

    double process()
    {
        return noise.nextSample();
    }

    WhiteNoise noise;
};

//==============================================================================
/**
    State storage for 12dB/octave highpass filter in AudioEquationParser.

    HP12State implements a one-pole IIR highpass filter for use in audio equations.
    The filter processes audio with a 12dB/octave slope, removing frequencies below
    the cutoff with adjustable Q for resonance control.

    @see HP24State, AudioFunctionHost
*/
struct HP12State : public FuncState
{
    HP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c = juce::IIRCoefficients::makeHighPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
/**
    State storage for 24dB/octave highpass filter in AudioEquationParser.

    HP24State implements a two-pole IIR highpass filter (cascaded 12dB stages)
    for use in audio equations. The filter provides a steeper 24dB/octave slope
    for more aggressive highpass filtering.

    @see HP12State, AudioFunctionHost
*/
struct HP24State : public FuncState
{
    HP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c1 = juce::IIRCoefficients::makeHighPass (sampleRate, freq, q);
        auto c2 = juce::IIRCoefficients::makeHighPass (sampleRate, freq, Q);

        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
/**
    State storage for 12dB/octave bandpass filter in AudioEquationParser.

    BP12State implements a one-pole IIR bandpass filter for use in audio equations.
    The filter passes frequencies near the center frequency while attenuating
    frequencies above and below with adjustable Q for bandwidth control.

    @see BP24State, AudioFunctionHost
*/
struct BP12State : public FuncState
{
    BP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c = juce::IIRCoefficients::makeBandPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
/**
    State storage for 24dB/octave bandpass filter in AudioEquationParser.

    BP24State implements a two-pole IIR bandpass filter (cascaded 12dB stages)
    for use in audio equations. The filter provides a steeper slope for more
    selective bandpass filtering with narrower bandwidth.

    @see BP12State, AudioFunctionHost
*/
struct BP24State : public FuncState
{
    BP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c1 = juce::IIRCoefficients::makeBandPass (sampleRate, freq, q);
        auto c2 = juce::IIRCoefficients::makeBandPass (sampleRate, freq, Q);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
/**
    State storage for 12dB/octave lowpass filter in AudioEquationParser.

    LP12State implements a one-pole IIR lowpass filter for use in audio equations.
    The filter removes frequencies above the cutoff with a 12dB/octave slope,
    with adjustable Q for resonance control.

    @see LP24State, AudioFunctionHost
*/
struct LP12State : public FuncState
{
    LP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c = juce::IIRCoefficients::makeLowPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
/**
    State storage for 24dB/octave lowpass filter in AudioEquationParser.

    LP24State implements a two-pole IIR lowpass filter (cascaded 12dB stages)
    for use in audio equations. The filter provides a steeper 24dB/octave slope
    for more aggressive lowpass filtering.

    @see LP12State, AudioFunctionHost
*/
struct LP24State : public FuncState
{
    LP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c1 = juce::IIRCoefficients::makeLowPass (sampleRate, freq, q);
        auto c2 = juce::IIRCoefficients::makeLowPass (sampleRate, freq, Q);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
/**
    State storage for 12dB/octave notch filter in AudioEquationParser.

    Notch12State implements a one-pole IIR notch (band-reject) filter for use
    in audio equations. The filter removes frequencies near the center frequency
    while passing frequencies above and below with adjustable Q for notch width.

    @see Notch24State, AudioFunctionHost
*/
struct Notch12State : public FuncState
{
    Notch12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c = juce::IIRCoefficients::makeNotchFilter (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
/**
    State storage for 24dB/octave notch filter in AudioEquationParser.

    Notch24State implements a two-pole IIR notch (band-reject) filter for use
    in audio equations. The filter provides a deeper notch with steeper slopes
    for more aggressive frequency removal.

    @see Notch12State, AudioFunctionHost
*/
struct Notch24State : public FuncState
{
    Notch24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = juce::jlimit (8.0, juce::jmin (20000.0, sampleRate / 2.0), freq);
        q = juce::jmax (0.0000001, q);

        auto c1 = juce::IIRCoefficients::makeNotchFilter (sampleRate, freq, q);
        auto c2 = juce::IIRCoefficients::makeNotchFilter (sampleRate, freq, Q);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
/**
    State management for audio functions in AudioEquationParser.

    AudioFunctionHost manages the state objects for all stateful audio functions
    (filters, oscillators, noise generators) used in audio equation parsing. It
    provides factory methods for creating and retrieving state objects, ensuring
    each function instance maintains its own state across evaluations.

    Key Features:
    - Automatic state creation and management
    - Sample rate propagation to all states
    - State reset for all functions
    - Integration with EquationParser
    - Band-limited oscillator table support

    The host maintains a map of state objects, creating them on-demand when
    functions are first called and reusing them for subsequent calls to maintain
    continuity (e.g., filter history, oscillator phase).

    @see FuncState, AudioEquationParser, EquationParser
*/
class AudioFunctionHost
{
public:
    // You must provide a set of lookup tables at the correct sample rate
    // if you add the oscillator functions
    gin::BandLimitedLookupTables* lookupTables = nullptr;

    void setSampleRate (double sr);
    double getSampleRate()  { return sampleRate; }
    void reset();

    void addConstants (gin::EquationParser&);
    void addUtilities (gin::EquationParser&);
    void addOscillatorFunctions (gin::EquationParser&);
    void addSynthFilterFunctions (gin::EquationParser&);
    void addEffectFilterFunctions (gin::EquationParser&);

    template <class T>
    T* getFuncParams (int i, double sr)
    {
        auto itr = funcStates.find (i);
        if (itr != funcStates.end())
        {
            auto p = dynamic_cast<T*> (itr->second.get());
            jassert (p != nullptr);
            return p;
        }

        auto p = new T (sr);
        funcStates[i].reset (p);
        return p;
    }

protected:
    std::map<int, std::unique_ptr<FuncState>> funcStates;
    double sampleRate = 44100.0;
};

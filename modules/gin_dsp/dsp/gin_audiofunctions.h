/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
 Base class to store an audio functions state
 But you say "functions don't have state!"
 Well, that's true, but to make things like filters act as functions,
 I need to hide a bit of state in them.
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
/** State for an oscillator
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
/** State for noise
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
/** State for high pass filter
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
/** State for high pass filter
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
/** State for band pass filter
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
/** State for band pass filter
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
/** State for low pass filter
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
/** State for low pass filter
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
/** State for notch filter
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
/** State for notch filter
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
/** State managment for audio functions
*/
class AudioFunctionHost
{
public:
    // You must provide a set of lookup tables at the correct sample rate
    // if you add the oscillator functions
    gin::BandLimitedLookupTables* lookupTables = nullptr;

    void setSampleRate (double sr);
    
    void addConstants (gin::EquationParser&);
    void addUtilities (gin::EquationParser&);
    void addOscillatorFunctions (gin::EquationParser&);
    void addSynthFilterFunctions (gin::EquationParser&);
    void addEffectFilterFunctions (gin::EquationParser&);

protected:
    std::map<int, std::unique_ptr<FuncState>> funcStates;
    double sampleRate = 44100.0;

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
};

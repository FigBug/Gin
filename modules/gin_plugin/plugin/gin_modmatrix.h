#pragma once

//==============================================================================
class ModMatrix;

/** Make your synth voice inherit from this if it supports modulation
*/
class ModVoice
{
public:
    ModVoice() = default;
    virtual ~ModVoice() = default;

    /* Gets value of a parameter with modulation applied */
    inline float getValue (gin::Parameter* p);
    inline float getValueUnsmoothed (gin::Parameter* p);

    void finishBlock (int numSamples)
    {
        for (auto& s : smoothers)
            s.process (numSamples);
    }

    void snapParams()
    {
        for (auto& s : smoothers)
            s.snapToValue();
    }

    void startVoice ();
    void stopVoice();

    int getAge()    { return age; }

    virtual bool isVoiceActive() = 0;

protected:
    bool disableSmoothing = false;

private:
    friend ModMatrix;

    ModMatrix* owner = nullptr;
    juce::Array<float> values;
    juce::Array<ValueSmoother<float>> smoothers;
    int age = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModVoice)
};

//==============================================================================
/**
    Modulation matrix system for routing multiple modulation sources to parameters.

    The ModMatrix provides a flexible modulation routing system commonly used in
    synthesizers and audio effects. It allows multiple modulation sources (LFOs,
    envelopes, velocity, etc.) to control multiple destination parameters with
    configurable depth, enable/disable, and curve shaping.

    Key Features:
    - Monophonic and polyphonic modulation sources
    - Multiple sources can modulate a single parameter
    - Configurable modulation curves (linear, exponential, sine, etc.)
    - Parameter smoothing to avoid discontinuities
    - Learning mode for quick modulation assignment
    - State persistence via ValueTree
    - Voice management for polyphonic synthesizers

    Usage:
    1. Create a ModMatrix instance in your synthesizer
    2. Add all parameters using addParameter()
    3. Add modulation sources using addMonoModSource() or addPolyModSource()
    4. Update mod source values from your processing loop
    5. Always retrieve parameter values through the ModMatrix using getValue()

    Example:
    @code
    ModMatrix modMatrix;

    // Setup
    modMatrix.addParameter(filterCutoff, false);
    auto lfo1 = modMatrix.addMonoModSource("lfo1", "LFO 1", true);
    auto env1 = modMatrix.addPolyModSource("env1", "Envelope 1", false);

    // In processing loop
    modMatrix.setMonoValue(lfo1, lfoOutput);
    modMatrix.setPolyValue(voice, env1, envelopeOutput);
    float cutoff = modMatrix.getValue(filterCutoff);
    @endcode

    @see ModVoice, ModSrcId, ModDstId, IModMatrix
*/
class ModMatrix : public IModMatrix
{
public:
    ModMatrix() = default;
    ~ModMatrix() override = default;

    /**
        Modulation polarity modes for source-to-destination mapping.

        PolarityMode determines how modulation values are interpreted when
        applied to parameters:
        - unipolar: Modulation ranges from 0 to 1 (always positive)
        - bipolar: Modulation ranges from -1 to 1 (positive and negative)
        - sameAsSource: Use the polarity of the modulation source

        This affects how modulation depth is applied and visualized in the UI.

        @see ModMatrix, ModSrcId, ModDstId
    */
    enum PolarityMode
    {
        unipolar,      ///< 0 to 1 (positive only)
        bipolar,       ///< -1 to 1 (positive and negative)
        sameAsSource,  ///< Match source polarity
    };
    
    void setDefaultPolarityMode (PolarityMode m) { defaultPolarityMode = m; }

    //==============================================================================
    void stateUpdated (const juce::ValueTree& vt);
    void updateState (juce::ValueTree& vt);
    
    //==============================================================================
    /** Backwards-compatible alias for ModFunction */
    using Function = ModFunction;

    // Static constants for backwards compatibility with ModMatrix::Function::xxx usage
    static constexpr ModFunction linear = ModFunction::linear;
    static constexpr ModFunction quadraticIn = ModFunction::quadraticIn;
    static constexpr ModFunction quadraticInOut = ModFunction::quadraticInOut;
    static constexpr ModFunction quadraticOut = ModFunction::quadraticOut;
    static constexpr ModFunction sineIn = ModFunction::sineIn;
    static constexpr ModFunction sineInOut = ModFunction::sineInOut;
    static constexpr ModFunction sineOut = ModFunction::sineOut;
    static constexpr ModFunction exponentialIn = ModFunction::exponentialIn;
    static constexpr ModFunction exponentialInOut = ModFunction::exponentialInOut;
    static constexpr ModFunction exponentialOut = ModFunction::exponentialOut;
    static constexpr ModFunction invLinear = ModFunction::invLinear;
    static constexpr ModFunction invQuadraticIn = ModFunction::invQuadraticIn;
    static constexpr ModFunction invQuadraticInOut = ModFunction::invQuadraticInOut;
    static constexpr ModFunction invQuadraticOut = ModFunction::invQuadraticOut;
    static constexpr ModFunction invSineIn = ModFunction::invSineIn;
    static constexpr ModFunction invSineInOut = ModFunction::invSineInOut;
    static constexpr ModFunction invSineOut = ModFunction::invSineOut;
    static constexpr ModFunction invExponentialIn = ModFunction::invExponentialIn;
    static constexpr ModFunction invExponentialInOut = ModFunction::invExponentialInOut;
    static constexpr ModFunction invExponentialOut = ModFunction::invExponentialOut;

    static float shape (float v, ModFunction f, bool biPolarSrc, bool biPolarDst)
    {
        if (biPolarSrc)
            v = juce::jmap (v, -1.0f, 1.0f, 0.0f, 1.0f);

        switch (f)
        {
            case ModFunction::linear:
            case ModFunction::quadraticIn:
            case ModFunction::quadraticInOut:
            case ModFunction::quadraticOut:
            case ModFunction::sineIn:
            case ModFunction::sineInOut:
            case ModFunction::sineOut:
            case ModFunction::exponentialIn:
            case ModFunction::exponentialInOut:
            case ModFunction::exponentialOut:
                break;
            case ModFunction::invLinear:
            case ModFunction::invQuadraticIn:
            case ModFunction::invQuadraticInOut:
            case ModFunction::invQuadraticOut:
            case ModFunction::invSineIn:
            case ModFunction::invSineInOut:
            case ModFunction::invSineOut:
            case ModFunction::invExponentialIn:
            case ModFunction::invExponentialInOut:
            case ModFunction::invExponentialOut:
                v = 1.0f - v;
                break;
        }

        switch (f)
        {
            case ModFunction::linear:
            case ModFunction::invLinear:
                break;
            case ModFunction::quadraticIn:
            case ModFunction::invQuadraticIn:
                v = easeQuadraticIn (v);
                break;
            case ModFunction::quadraticInOut:
            case ModFunction::invQuadraticInOut:
                v = easeQuadraticInOut (v);
                break;
            case ModFunction::quadraticOut:
            case ModFunction::invQuadraticOut:
                v = easeQuadraticOut (v);
                break;
            case ModFunction::sineIn:
            case ModFunction::invSineIn:
                v = easeSineIn (v);
                break;
            case ModFunction::sineInOut:
            case ModFunction::invSineInOut:
                v = easeSineInOut (v);
                break;
            case ModFunction::sineOut:
            case ModFunction::invSineOut:
                v = easeSineOut (v);
                break;
            case ModFunction::exponentialIn:
            case ModFunction::invExponentialIn:
                v = easeExponentialIn (v);
                break;
            case ModFunction::exponentialInOut:
            case ModFunction::invExponentialInOut:
                v = easeExponentialInOut (v);
                break;
            case ModFunction::exponentialOut:
            case ModFunction::invExponentialOut:
                v = easeExponentialOut (v);
                break;
        }

        if (biPolarDst)
            v = juce::jmap (v, 0.0f, 1.0f, -1.0f, 1.0f);

        return v;
    }

    //==============================================================================
    float getValue (gin::Parameter* p, bool smoothed = true)
    {
        const int paramId = p->getModIndex();

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.enabled)
            {
                if (src.poly && activeVoice != nullptr)
                    base += shape (activeVoice->values[src.id.id], src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                else if (! src.poly)
                    base += shape (sources[src.id.id].monoValue, src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
            }
        }

        base = juce::jlimit (0.0f, 1.0f, base);
        auto& smoother = smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = smoothed ? smoother.getCurrentValue() : base;

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    float getValue (ModVoice& voice, gin::Parameter* p, bool smoothed = true)
    {
        const int paramId = p->getModIndex();
        jassert (paramId >= 0);

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.enabled)
            {
                if (src.poly)
                    base += shape (voice.values[src.id.id], src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                else
                    base += shape (sources[src.id.id].monoValue, src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
            }
        }

        base = juce::jlimit (0.0f, 1.0f, base);
        auto& smoother = voice.smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = (voice.disableSmoothing || ! smoothed) ? base : smoother.getCurrentValue();

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    void setMonoValue (ModSrcId id, float value)
    {
        auto& info = sources.getReference (id.id);
        jassert (! info.poly);

        info.monoValue = value;
    }

    void setPolyValue (ModVoice& voice, ModSrcId id, float value)
    {
        jassert (sources.getReference (id.id).poly);
        voice.values.setUnchecked (id.id, value);
    }

    void finishBlock (int numSamples)
    {
        for (auto& s : smoothers)
            s.process (numSamples);
    }

    //==============================================================================
    void addVoice (ModVoice* v);
    ModSrcId addMonoModSource (const juce::String& id, const juce::String& name, bool bipolar);
    ModSrcId addPolyModSource (const juce::String& id, const juce::String& name, bool bipolar);
    void addParameter (gin::Parameter* p, bool poly, float smoothingTime = 0.02f);

    void setSampleRate (double sampleRate);
    void build();

    //==============================================================================
    // IModMatrix interface implementation
    void enableLearn (ModSrcId source) override;
    void disableLearn() override;
    ModSrcId getLearn() override                        { return learnSource;               }

    //==============================================================================
    int getNumModSources() override                     { return sources.size();            }
    juce::String getModSrcName (ModSrcId src) override  { return sources[src.id].name;      }
    bool getModSrcPoly (ModSrcId src) override          { return sources[src.id].poly;      }
    bool getModSrcBipolar (ModSrcId src) override       { return sources[src.id].bipolar;   }

    juce::String getModDstName (ModDstId dst) override;

    juce::Array<ModSrcId> getModSources (gin::Parameter*) override;

    Parameter* getParameter (ModDstId d) override;

    bool isModulated (ModDstId param) override;

    float getModDepth (ModSrcId src, ModDstId param) override;
    std::vector<std::pair<ModDstId, float>> getModDepths (ModSrcId src) override;
    std::vector<std::pair<ModSrcId, float>> getModDepths (ModDstId param) override;
    void setModDepth (ModSrcId src, ModDstId param, float f) override;
    void clearModDepth (ModSrcId src, ModDstId param) override;

    ModFunction getModFunction (ModSrcId src, ModDstId param) override;
    void setModFunction (ModSrcId src, ModDstId param, ModFunction f) override;

    bool getModEnable (ModSrcId src, ModDstId param) override;
    void setModEnable (ModSrcId src, ModDstId param, bool b) override;

    bool getModBipolarMapping (ModSrcId src, ModDstId param) override;
    void setModBipolarMapping (ModSrcId src, ModDstId param, bool b) override;

    //==============================================================================
    bool shouldShowLiveModValues() override
    {
        if (onlyShowModWhenVoiceActive)
        {
            for (auto v : voices)
                if (v->isVoiceActive())
                    return true;

            return false;
        }

        return true;
    }

    juce::Array<float> getLiveValues (gin::Parameter* p) override;

    void setOnlyShowModWhenVoiceActive (bool b)
    {
        onlyShowModWhenVoiceActive = b;
    }

    //==============================================================================
    void addListener (Listener* l) override     { listeners.add (l);            }
    void removeListener (Listener* l) override  { listeners.remove (l);         }

private:
    friend ModVoice;

    //==============================================================================
    int voiceStarted (ModVoice* v);
    void voiceStopped (ModVoice* v);

    //==============================================================================
    struct SourceInfo
    {
        juce::String id;
        juce::String name;
        bool poly = false;
        bool bipolar = false;
        ModSrcId index = {};
        float monoValue = 0.0f;
    };

    struct Source
    {
        ModSrcId id = {};
        bool poly = false;
        bool enabled = true;
        float depth = 0.0f;
        bool biPolarMapping = false;
        ModFunction function = ModFunction::linear;
    };

    struct ParamInfo
    {
        gin::Parameter* parameter = nullptr;
        bool poly = false;
        float smoothingTime = 0.02f;
        juce::Array<Source> sources;
    };

    //==============================================================================
    juce::Array<SourceInfo> sources;
    juce::Array<ParamInfo> parameters;
    juce::Array<ModVoice*> voices;
    juce::Array<ValueSmoother<float>> smoothers;
    ModVoice* activeVoice = nullptr;
    bool onlyShowModWhenVoiceActive = false;
    PolarityMode defaultPolarityMode = sameAsSource;

    double sampleRate = 44100.0;

    juce::ListenerList<Listener> listeners;

    ModSrcId learnSource;
    int nextAge = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModMatrix)
};

inline float ModVoice::getValue (gin::Parameter* p)
{
    return owner->getValue (*this, p);
}

inline float ModVoice::getValueUnsmoothed (gin::Parameter* p)
{
    return owner->getValue (*this, p, false);
}

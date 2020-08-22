#pragma once

//==============================================================================
struct ModSrcId
{
    ModSrcId () = default;
    explicit ModSrcId (int id_) : id (id_) {}
    ModSrcId (const ModSrcId& other) { id = other.id; }
    ModSrcId& operator= (const ModSrcId& other) { id = other.id; return *this; }
    bool operator== (const ModSrcId& other) const { return other.id == id; }
    bool isValid() const { return id > 0; }
    
    int id = -1;
};

//==============================================================================
struct ModDstId
{
    ModDstId () = default;
    explicit ModDstId (int id_) : id (id_)  {}
    ModDstId (const ModDstId& other) { id = other.id; }
    ModDstId& operator= (const ModDstId& other) { id = other.id; return *this; }
    bool operator== (const ModDstId& other) const { return other.id == id; }
    bool isValid() const { return id > 0; }
 
    int id = -1;
};

//==============================================================================
class ModMatrix;

/** Make your voice inherit from this if it supports modulation
*/
class ModVoice
{
public:
    ModVoice() = default;
    virtual ~ModVoice() = default;

    float getValue (gin::Parameter* p);

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
/** Add one of these to you Synth if you want to support modulation

    Then add all your parameters
    Then add all your mod source. Update your mod sources from your processing
    loop.
    Always get your parameter values from the mod matrix
*/
class ModMatrix
{
public:
    ModMatrix() = default;

    //==============================================================================
    void stateUpdated (const juce::ValueTree& vt);
    void updateState (juce::ValueTree& vt);

    //==============================================================================
    float getValue (gin::Parameter* p)
    {
        const int paramId = p->getModIndex();

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.poly && activeVoice != nullptr)
                base += activeVoice->values[src.id.id] * src.depth;
            else if (! src.poly)
                base += sources[src.id.id].monoValue * src.depth;
        }

        base = juce::jlimit (0.0f, 1.0f, base);
        auto& smoother = smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = smoother.getCurrentValue();

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    float getValue (ModVoice& voice, gin::Parameter* p)
    {
        const int paramId = p->getModIndex();

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.poly)
                base += voice.values[src.id.id] * src.depth;
            else
                base += sources[src.id.id].monoValue * src.depth;
        }

        base = juce::jlimit (0.0f, 1.0f, base);
        auto& smoother = voice.smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = voice.disableSmoothing ? base : smoother.getCurrentValue();

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    juce::Array<float> getLiveValues (gin::Parameter* p)
    {
        juce::Array<float> liveValues;

        const int paramId = p->getModIndex();
        auto& pi = parameters.getReference (paramId);

        if (pi.poly)
        {
            for (auto v : voices)
            {
                if (v->isVoiceActive())
                {
                    float base = p->getValue();
                    auto& info = parameters.getReference (paramId);

                    for (auto& src : info.sources)
                    {
                        if (src.poly)
                            base += v->values[src.id.id] * src.depth;
                        else
                            base += sources[src.id.id].monoValue * src.depth;
                    }

                    base = juce::jlimit (0.0f, 1.0f, base);

                    liveValues.add (base);
                }
            }
        }
        else
        {
            bool ok = false;
            auto v = activeVoice;

            float base = p->getValue();
            auto& info = parameters.getReference (paramId);

            for (auto& src : info.sources)
            {
                if (src.poly && v != nullptr)
                {
                    ok = true;
                    base += v->values[src.id.id] * src.depth;
                }
                else if (! src.poly)
                {
                    ok = true;
                    base += sources[src.id.id].monoValue * src.depth;
                }
            }

            if (ok)
            {
                base = juce::jlimit (0.0f, 1.0f, base);
                liveValues.add (base);
            }
        }

        return liveValues;
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
    void addParameter (gin::Parameter* p, bool poly);

    void setSampleRate (double sampleRate);
    void build();

    //==============================================================================
    void enableLearn (ModSrcId source);
    void disableLearn();
    ModSrcId getLearn()                         { return learnSource;               }

    //==============================================================================
    int getNumModSources()                      { return sources.size();            }
    juce::String getModSrcName (ModSrcId src)   { return sources[src.id].name;      }
    bool getModSrcPoly (ModSrcId src)           { return sources[src.id].poly;      }
    bool getModSrcBipolar (ModSrcId src)        { return sources[src.id].bipolar;   }

    juce::Array<ModSrcId> getModSources (gin::Parameter*);

    bool isModulated (ModDstId param);

    float getModDepth (ModSrcId src, ModDstId param);
    void setModDepth (ModSrcId src, ModDstId param, float f);
    void clearModDepth (ModSrcId src, ModDstId param);

    //==============================================================================
    class Listener
    {
    public:
        virtual ~Listener() = default;
        virtual void modMatrixChanged()             {}
        virtual void learnSourceChanged (ModSrcId)  {}
    };

    void addListener (Listener* l)      { listeners.add (l);            }
    void removeListener (Listener* l)   { listeners.remove (l);         }

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
        float depth = 0.0f;
    };

    struct ParamInfo
    {
        gin::Parameter* parameter;
        bool poly = false;
        juce::Array<Source> sources;
    };

    //==============================================================================
    juce::Array<SourceInfo> sources;
    juce::Array<ParamInfo> parameters;
    juce::Array<ModVoice*> voices;
    juce::Array<ValueSmoother<float>> smoothers;
    ModVoice* activeVoice = nullptr;

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

#pragma once

//==============================================================================
class ModMatrix;
class ModVoice
{
public:
    virtual ~ModVoice() = default;
    
    float getValue (Parameter* p);

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
    
private:
    friend ModMatrix;

    ModMatrix* owner = nullptr;
    Array<float> values;
    Array<ValueSmoother<float>> smoothers;
    int age = 0;
};

//==============================================================================
class ModMatrix
{
public:

    //==============================================================================
    void stateUpdated (const ValueTree& vt);
    void updateState (ValueTree& vt);

    //==============================================================================
    float getValue (Parameter* p)
    {
        const int paramId = p->getModIndex();

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.poly && activeVoice != nullptr)
                base += activeVoice->values[src.id] * src.depth;
            else if (! src.poly)
                base += sources[src.id].monoValue * src.depth;
        }

        base = jlimit (0.0f, 1.0f, base);
        auto& smoother = smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = smoother.getCurrentValue();

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    float getValue (ModVoice& voice, Parameter* p)
    {
        const int paramId = p->getModIndex();

        float base = p->getValue();
        auto& info = parameters.getReference (paramId);

        for (auto& src : info.sources)
        {
            if (src.poly)
                base += voice.values[src.id] * src.depth;
            else
                base += sources[src.id].monoValue * src.depth;
        }

        base = jlimit (0.0f, 1.0f, base);
        auto& smoother = voice.smoothers.getReference (paramId);

        smoother.setValue (base);
        auto v = smoother.getCurrentValue();

        v = p->getUserRange().convertFrom0to1 (v);

        if (p->conversionFunction)
            v = p->conversionFunction (v);

        return v;
    }

    Array<float> getLiveValues (Parameter* p)
    {
        Array<float> liveValues;

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
                            base += v->values[src.id] * src.depth;
                        else
                            base += sources[src.id].monoValue * src.depth;
                    }

                    base = jlimit (0.0f, 1.0f, base);

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
                    base += v->values[src.id] * src.depth;
                }
                else if (! src.poly)
                {
                    ok = true;
                    base += sources[src.id].monoValue * src.depth;
                }
            }

            if (ok)
            {
                base = jlimit (0.0f, 1.0f, base);
                liveValues.add (base);
            }
        }

        return liveValues;
    }

    void setMonoValue (int id, float value)
    {
        auto& info = sources.getReference (id);
        jassert (! info.poly);

        info.monoValue = value;
    }

    void setPolyValue (ModVoice& voice, int id, float value)
    {
        auto& info = sources.getReference (id);
        jassert (info.poly);

        voice.values.setUnchecked (id, value);
    }
    
    void finishBlock (int numSamples)
    {
        for (auto& s : smoothers)
            s.process (numSamples);
    }

    //==============================================================================
    void addVoice (ModVoice* v);
    int addMonoModSource (const String& name, bool bipolar);
    int addPolyModSource (const String& name, bool bipolar);
    void addParameter (Parameter* p, bool poly);

    void setSampleRate (double sampleRate);
    void build();

    //==============================================================================
    void enableLearn (int source);
    void disableLearn();
    int getLearn()                      { return learnSource;           }

    //==============================================================================
	int getNumModSources()				{ return sources.size();		}
    String getModSrcName (int src)      { return sources[src].name;     }
    bool getModSrcPoly (int src)        { return sources[src].poly;     }
    bool getModSrcBipolar (int src)     { return sources[src].bipolar;  }

    Array<int> getModSources (Parameter*);

    bool isModulated (int param);

    float getModDepth (int src, int param);
    void setModDepth (int src, int param, float f);
    void clearModDepth (int src, int param);

    //==============================================================================
    class Listener
    {
    public:
        virtual ~Listener() = default;
		virtual void modMatrixChanged()			{}
		virtual void learnSourceChanged (int) 	{}
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
        String name;
        bool poly = false;
        bool bipolar = false;
        int index = 0;
        float monoValue = 0.0f;
    };

    struct Source
    {
        int id = 0;
        bool poly = false;
        float depth = 0.0f;
    };

    struct ParamInfo
    {
        Parameter* parameter;
        bool poly = false;
        Array<Source> sources;
    };

    //==============================================================================
    Array<SourceInfo> sources;
    Array<ParamInfo> parameters;
    Array<ModVoice*> voices;
    Array<ValueSmoother<float>> smoothers;
    ModVoice* activeVoice = nullptr;

    double sampleRate = 44100.0;

    ListenerList<Listener> listeners;

    int learnSource = -1, nextAge = 0;
};

inline float ModVoice::getValue (Parameter* p)
{
    return owner->getValue (*this, p);
}

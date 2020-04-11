#pragma once

//==============================================================================
class ModMatrix;
class ModVoice
{
public:
    float getValue (Parameter* p);

private:
    friend ModMatrix;

    ModMatrix* owner = nullptr;
    Array<float> values;
    Array<ValueSmoother<float>> smoothers;
};

//==============================================================================
class ModMatrix
{
public:
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

    void addVoice (ModVoice* v);
    int addMonoModSource (const String& name);
    int addPolyModSource (const String& name);
    void addParameter (Parameter* p);

    void setSampleRate (double sampleRate);
    void build();

private:
    struct SourceInfo
    {
        String name;
        bool poly;
        int index;
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
        Array<Source> sources;
    };

    Array<SourceInfo> sources;
    Array<ParamInfo> parameters;
    Array<ModVoice*> voices;
    Array<ValueSmoother<float>> smoothers;
    ModVoice* activeVoice = nullptr;

    double sampleRate = 44100.0;
};

inline float ModVoice::getValue (Parameter* p)
{
    return owner->getValue (*this, p);
}

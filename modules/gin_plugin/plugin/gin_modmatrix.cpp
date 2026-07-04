//==============================================================================
static ModFunction strToFunc (const juce::String& str)
{
    if (str == "linear") return ModFunction::linear;
    if (str == "quadraticIn") return ModFunction::quadraticIn;
    if (str == "quadraticInOut") return ModFunction::quadraticInOut;
    if (str == "quadraticOut") return ModFunction::quadraticOut;
    if (str == "sineIn") return ModFunction::sineIn;
    if (str == "sineInOut") return ModFunction::sineInOut;
    if (str == "sineOut") return ModFunction::sineOut;
    if (str == "exponentialIn") return ModFunction::exponentialIn;
    if (str == "exponentialInOut") return ModFunction::exponentialInOut;
    if (str == "exponentialOut") return ModFunction::exponentialOut;
    if (str == "invLinear") return ModFunction::invLinear;
    if (str == "invQuadraticIn") return ModFunction::invQuadraticIn;
    if (str == "invQuadraticInOut") return ModFunction::invQuadraticInOut;
    if (str == "invQuadraticOut") return ModFunction::invQuadraticOut;
    if (str == "invSineIn") return ModFunction::invSineIn;
    if (str == "invSineInOut") return ModFunction::invSineInOut;
    if (str == "invSineOut") return ModFunction::invSineOut;
    if (str == "invExponentialIn") return ModFunction::invExponentialIn;
    if (str == "invExponentialInOut") return ModFunction::invExponentialInOut;
    if (str == "invExponentialOut") return ModFunction::invExponentialOut;

    return ModFunction::linear;
}

static juce::String funcToStr (ModFunction f)
{
    if (f == ModFunction::linear) return "linear";
    if (f == ModFunction::quadraticIn) return "quadraticIn";
    if (f == ModFunction::quadraticInOut) return "quadraticInOut";
    if (f == ModFunction::quadraticOut) return "quadraticOut";
    if (f == ModFunction::sineIn) return "sineIn";
    if (f == ModFunction::sineInOut) return "sineInOut";
    if (f == ModFunction::sineOut) return "sineOut";
    if (f == ModFunction::exponentialIn) return "exponentialIn";
    if (f == ModFunction::exponentialInOut) return "exponentialInOut";
    if (f == ModFunction::exponentialOut) return "exponentialOut";
    if (f == ModFunction::invLinear) return "invLinear";
    if (f == ModFunction::invQuadraticIn) return "invQuadraticIn";
    if (f == ModFunction::invQuadraticInOut) return "invQuadraticInOut";
    if (f == ModFunction::invQuadraticOut) return "invQuadraticOut";
    if (f == ModFunction::invSineIn) return "invSineIn";
    if (f == ModFunction::invSineInOut) return "invSineInOut";
    if (f == ModFunction::invSineOut) return "invSineOut";
    if (f == ModFunction::invExponentialIn) return "invExponentialIn";
    if (f == ModFunction::invExponentialInOut) return "invExponentialInOut";
    if (f == ModFunction::invExponentialOut) return "invExponentialOut";

    return "linear";
}

//==============================================================================
void ModVoice::startVoice ()
{
    age = owner->voiceStarted (this);
}

void ModVoice::stopVoice()
{
    owner->voiceStopped (this);
}

//==============================================================================
void ModMatrix::stateUpdated (const juce::ValueTree& vt)
{
    auto lookupSrc = [&] (const juce::String& str)
    {
        int idx = 0;
        for (auto& s : sources)
        {
            if (s.id == str)
                return ModSrcId (idx);
            idx++;
        }
        //jassertfalse;
        return ModSrcId();
    };

    for (auto& pi : parameters)
        pi.sources.clear();

    auto mm = vt.getChildWithName ("MODMATRIX");
    if (mm.isValid())
    {
        for (auto c : mm)
        {
            if (! c.hasType ("MODITEM")) continue;

            auto src = c.getProperty ("srcId").toString();
            auto dst = c.getProperty ("dstId").toString();

            auto f = float (c.getProperty ("depth", 0.0f));
            auto e = bool (c.getProperty ("enabled", true));
            auto z = strToFunc (c.getProperty ("function", "linear"));
            auto b = bool (c.getProperty ("biPolarMapping", false));

            if (src.isNotEmpty() && dst.isNotEmpty())
            {
                Source s;
                s.id = lookupSrc (src);
                s.poly = getModSrcPoly (s.id);
                s.depth = f;
                s.enabled = e;
                s.function = z;
                s.biPolarMapping = b;
                
                if (! c.hasProperty ("biPolarMapping"))
                {
                    if (defaultPolarityMode == bipolar)
                        s.biPolarMapping = true;
                    else if (defaultPolarityMode == unipolar)
                        s.biPolarMapping = false;
                    else if (defaultPolarityMode == sameAsSource)
                        s.biPolarMapping = sources[s.id.id].bipolar;
                }

                auto foundParam = false;
                for (auto& pi : parameters)
                {
                    if (pi.parameter->getUid() == dst)
                    {
                        pi.sources.add (s);
                        foundParam = true;
                        break;
                    }
                }

                if (! foundParam)
                {
                    DBG("Parameter not found: " + dst);
                    jassertfalse;
                }
            }
        }
    }
    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

void ModMatrix::updateState (juce::ValueTree& vt)
{
    auto mm = vt.getOrCreateChildWithName ("MODMATRIX", nullptr);
    mm.removeAllChildren (nullptr);

    for (int i = 0; i < parameters.size(); i++)
    {
        auto& pi = parameters.getReference (i);
        for (auto src : pi.sources)
        {
            auto c = juce::ValueTree ("MODITEM");
            c.setProperty ("srcId", sources[src.id.id].id, nullptr);
            c.setProperty ("depth", src.depth, nullptr);
            c.setProperty ("enabled", src.enabled, nullptr);
            c.setProperty ("dstId", pi.parameter->getUid(), nullptr);
            c.setProperty ("function", funcToStr (src.function), nullptr);
            c.setProperty ("biPolarMapping", src.biPolarMapping, nullptr);

            mm.addChild (c, -1, nullptr);
        }
    }
}

void ModMatrix::addVoice (ModVoice* v)
{
    voices.add (v);

    v->owner = this;
}

ModSrcId ModMatrix::addMonoModSource (const juce::String& id, const juce::String& name, bool bipolar)
{
    SourceInfo si;
    si.id      = id;
    si.name    = name;
    si.poly    = false;
    si.bipolar = bipolar;
    si.index   = ModSrcId (sources.size());

    sources.add (si);
    return ModSrcId (si.index);
}

ModSrcId ModMatrix::addPolyModSource (const juce::String& id, const juce::String& name, bool bipolar)
{
    SourceInfo si;
    si.id      = id;
    si.name    = name;
    si.poly    = true;
    si.bipolar = bipolar;
    si.index   = ModSrcId (sources.size());

    sources.add (si);
    return ModSrcId (si.index);
}

void ModMatrix::addParameter (Parameter* p, bool poly, float smoothingTime)
{
    p->setModMatrix (this);
    p->setModIndex (parameters.size());

    ParamInfo pi;
    pi.poly = poly;
    pi.parameter = p;
    pi.smoothingTime = smoothingTime;

    parameters.add (pi);
}

void ModMatrix::setSampleRate (double sr)
{
    sampleRate = sr;

    for (auto idx = 0; auto& s : smoothers)
    {
        s.setSampleRate (sr);
        s.setTime (parameters[idx].smoothingTime);
        idx++;
    }

    for (auto& v : voices)
    {
        for (auto idx = 0; auto& s : v->smoothers)
        {
            s.setSampleRate (sr);
            s.setTime (parameters[idx].smoothingTime);
            idx++;
        }
    }
}

void ModMatrix::build()
{
    for (auto& v : voices)
    {
        v->values.resize (sources.size());
        v->smoothers.resize (parameters.size());
    }

    smoothers.resize (parameters.size());
}

void ModMatrix::enableLearn (ModSrcId src)
{
    learnSource = src;

    listeners.call ([&] (Listener& l) { l.learnSourceChanged (learnSource); });
}

void ModMatrix::disableLearn()
{
    learnSource.id = -1;

    listeners.call ([&] (Listener& l) { l.learnSourceChanged (learnSource); });
}

bool ModMatrix::isModulated (ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    if (pi.sources.size() > 0)
        return true;
    return false;
}

bool ModMatrix::getModEnable (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.enabled;

    return false;
}

void ModMatrix::setModEnable (ModSrcId src, ModDstId param, bool b)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            si.enabled = b;

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

bool ModMatrix::getModBipolarMapping (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.biPolarMapping;

    return false;
}

void ModMatrix::setModBipolarMapping (ModSrcId src, ModDstId param, bool b)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            si.biPolarMapping = b;

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

float ModMatrix::getModDepth (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.depth;

    return 0;
}

ModFunction ModMatrix::getModFunction (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.function;

    return ModFunction::linear;
}

std::vector<std::pair<ModSrcId, float>> ModMatrix::getModDepths (ModDstId param)
{
    std::vector<std::pair<ModSrcId, float>> res;

    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        res.push_back ({si.id, si.depth});

    return res;
}

std::vector<std::pair<ModDstId, float>> ModMatrix::getModDepths (ModSrcId param)
{
    std::vector<std::pair<ModDstId, float>> res;

    for (auto [idx, pi] : juce::enumerate (parameters))
    {
        for (auto& si : pi.sources)
            if (si.id == param)
                res.push_back ({ModDstId (int (idx)), si.depth});
    }

    return res;
}

void ModMatrix::setModDepth (ModSrcId src, ModDstId param, float f)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
    {
        if (si.id == src)
        {
            si.depth = f;

            listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });

            return;
        }
    }

    Source s;
    s.id       = src;
    s.poly     = getModSrcPoly (src);
    s.depth    = f;
    s.function = linear;
    
    if (defaultPolarityMode == bipolar)
        s.biPolarMapping = true;
    else if (defaultPolarityMode == unipolar)
        s.biPolarMapping = false;
    else if (defaultPolarityMode == sameAsSource)
        s.biPolarMapping = sources[src.id].bipolar;

    pi.sources.add (s);

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

void ModMatrix::setModFunction (ModSrcId src, ModDstId param, ModFunction f)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
    {
        if (si.id == src)
        {
            si.function = f;

            listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });

            return;
        }
    }

    Source s;
    s.id = src;
    s.poly = getModSrcPoly (src);
    s.depth = 1.0f;
    s.function = f;
    
    if (defaultPolarityMode == bipolar)
        s.biPolarMapping = true;
    else if (defaultPolarityMode == unipolar)
        s.biPolarMapping = false;
    else if (defaultPolarityMode == sameAsSource)
        s.biPolarMapping = sources[src.id].bipolar;

    pi.sources.add (s);

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

void ModMatrix::clearModDepth (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (int i = pi.sources.size(); --i >= 0;)
    {
        auto si = pi.sources[i];
        if (si.id == src)
            pi.sources.remove (i);
    }

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

juce::String ModMatrix::getModDstName (ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    return pi.parameter->getName (1024);
}

juce::Array<ModSrcId> ModMatrix::getModSources (gin::Parameter* param)
{
    juce::Array<ModSrcId> srcs;

    auto idx = param->getModIndex();
    if (idx >= 0)
    {
        auto& pi = parameters.getReference (idx);
        for (auto& si : pi.sources)
            srcs.add (si.id);
    }

    return srcs;
}

int ModMatrix::voiceStarted (ModVoice* v)
{
    activeVoice = v;

    return ++nextAge;
}

void ModMatrix::voiceStopped (ModVoice* v)
{
    if (v == activeVoice)
    {
        int youngestVoice = 0;
        activeVoice = nullptr;
        for (auto possibleVoice : voices)
        {
            if (possibleVoice == v) continue;

            if (possibleVoice->isVoiceActive() && possibleVoice->getAge() > youngestVoice)
            {
                youngestVoice = possibleVoice->getAge();
                activeVoice = possibleVoice;
            }
        }
    }
}

Parameter* ModMatrix::getParameter (ModDstId d)
{
    for (const auto& info : parameters)
        if (info.parameter->getModIndex() == d.id)
            return info.parameter;

    return nullptr;
}

juce::Array<float> ModMatrix::getLiveValues (gin::Parameter* p)
{
    juce::Array<float> liveValues;

    const int paramId = p->getModIndex();
    auto& pi = parameters.getReference (paramId);

    auto& info = parameters.getReference (paramId);

    if (pi.poly)
    {
        for (auto v : voices)
        {
            if (v->isVoiceActive())
            {
                bool ok = false;
                float base = p->getValue();

                for (auto& src : info.sources)
                {
                    if (src.enabled)
                    {
                        if (src.poly)
                            base += shape (v->values[src.id.id], src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                        else
                            base += shape (sources[src.id.id].monoValue, src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                        ok = true;
                    }
                }

                if (ok)
                {
                    base = juce::jlimit (0.0f, 1.0f, base);
                    liveValues.add (base);
                }
            }
        }

        if (liveValues.size() == 0)
        {
            float base = p->getValue();
            bool ok = false;

            for (auto& src : info.sources)
            {
                if (src.enabled && ! src.poly)
                {
                    base += shape (sources[src.id.id].monoValue, src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                    ok = true;
                }
            }

            if (ok)
            {
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

        for (auto& src : info.sources)
        {
            if (src.enabled)
            {
                if (src.poly && v != nullptr)
                {
                    ok = true;
                    base += shape (v->values[src.id.id], src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                }
                else if (! src.poly)
                {
                    ok = true;
                    base += shape (sources[src.id.id].monoValue, src.function, sources[src.id.id].bipolar, src.biPolarMapping) * src.depth;
                }
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

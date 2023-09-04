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
        jassertfalse;
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

            juce::String src = c.getProperty ("srcId");
            float f    = c.getProperty ("depth");
            juce::String dst = c.getProperty ("dstId");

            if (src.isNotEmpty() && dst.isNotEmpty())
            {
                Source s;
                s.id = lookupSrc (src);
                s.poly = getModSrcPoly (s.id);
                s.depth = f;

                for (auto& pi : parameters)
                {
                    if (pi.parameter->getUid() == dst)
                    {
                        pi.sources.add (s);
                        break;
                    }
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
            c.setProperty ("dstId", pi.parameter->getUid(), nullptr);

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

void ModMatrix::addParameter (Parameter* p, bool poly)
{
    p->setModMatrix (this);
    p->setModIndex (parameters.size());

    ParamInfo pi;
    pi.poly = poly;
    pi.parameter = p;

    parameters.add (pi);
}

void ModMatrix::setSampleRate (double sr)
{
    sampleRate = sr;

    for (auto& s : smoothers)
    {
        s.setSampleRate (sr);
        s.setTime (0.02);
    }

    for (auto& v : voices)
    {
        for (auto& s : v->smoothers)
        {
            s.setSampleRate (sr);
            s.setTime (0.02);
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

float ModMatrix::getModDepth (ModSrcId src, ModDstId param)
{
    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.depth;

    return 0;
}

std::vector<std::pair<ModSrcId, float>> ModMatrix::getModDepths (ModDstId param)
{
    std::vector<std::pair<ModSrcId, float>> res;

    auto& pi = parameters.getReference (param.id);
    for (auto& si : pi.sources)
        res.push_back ({si.id, si.depth});

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
    s.id = src;
    s.poly = getModSrcPoly (src);
    s.depth = f;

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
    if (activeVoice == nullptr)
        activeVoice = v;

    return ++nextAge;
}

void ModMatrix::voiceStopped (ModVoice* v)
{
    if (v == activeVoice)
    {
        int youngestVoice = std::numeric_limits<int>::max();
        activeVoice = nullptr;
        for (auto possibleVoice : voices)
        {
            if (possibleVoice == v) continue;

            if (possibleVoice->isVoiceActive() && possibleVoice->getAge() < youngestVoice)
            {
                youngestVoice = possibleVoice->getAge();
                activeVoice = possibleVoice;
            }
        }
    }
}

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
void ModMatrix::stateUpdated (const ValueTree& vt)
{
    for (auto& pi : parameters)
        pi.sources.clear();

    auto mm = vt.getChildWithName ("MODMATRIX");
    if (mm.isValid())
    {
        for (auto c : mm)
        {
            if (! c.hasType ("MODITEM")) continue;

            int src   = c.getProperty ("src");
            float f   = c.getProperty ("depth");
            int param = c.getProperty ("param");

            Source s;
            s.id = src;
            s.poly = getModSrcPoly (src);
            s.depth = f;

            auto& pi = parameters.getReference (param);
            pi.sources.add (s);
        }
    }
    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

void ModMatrix::updateState (ValueTree& vt)
{
    auto mm = vt.getOrCreateChildWithName ("MODMATRIX", nullptr);
    mm.removeAllChildren (nullptr);

    for (int i = 0; i < parameters.size(); i++)
    {
        auto& pi = parameters.getReference (i);
        for (auto src : pi.sources)
        {
            auto c = ValueTree ("MODITEM");
            c.setProperty ("src", src.id, nullptr);
            c.setProperty ("depth", src.depth, nullptr);
            c.setProperty ("param", i, nullptr);

            mm.addChild (c, -1, nullptr);
        }
    }
}

void ModMatrix::addVoice (ModVoice* v)
{
    voices.add (v);

    v->owner = this;
}

int ModMatrix::addMonoModSource (const String& name, bool bipolar)
{
    SourceInfo si;
    si.name    = name;
    si.poly    = false;
    si.bipolar = bipolar;
    si.index   = sources.size();

    sources.add (si);
    return si.index;
}

int ModMatrix::addPolyModSource (const String& name, bool bipolar)
{
    SourceInfo si;
    si.name    = name;
    si.poly    = true;
    si.bipolar = bipolar;
    si.index   = sources.size();

    sources.add (si);
    return si.index;
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

void ModMatrix::enableLearn (int src)
{
    learnSource = src;

    listeners.call ([&] (Listener& l) { l.learnSourceChanged (learnSource); });
}

void ModMatrix::disableLearn()
{
    learnSource = -1;

    listeners.call ([&] (Listener& l) { l.learnSourceChanged (learnSource); });
}

bool ModMatrix::isModulated (int param)
{
    auto& pi = parameters.getReference (param);
    if (pi.sources.size() > 0)
        return true;
    return false;
}

float ModMatrix::getModDepth (int src, int param)
{
    auto& pi = parameters.getReference (param);
    for (auto& si : pi.sources)
        if (si.id == src)
            return si.depth;

    return 0;
}

void ModMatrix::setModDepth (int src, int param, float f)
{
    auto& pi = parameters.getReference (param);
    for (auto& si : pi.sources)
    {
        if (si.id == src)
        {
            si.depth = f;
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

void ModMatrix::clearModDepth (int src, int param)
{
    auto& pi = parameters.getReference (param);
    for (int i = pi.sources.size(); --i >= 0;)
    {
        auto si = pi.sources[i];
        if (si.id == src)
            pi.sources.remove (i);
    }

    listeners.call ([&] (Listener& l) { l.modMatrixChanged(); });
}

Array<int> ModMatrix::getModSources (Parameter* param)
{
    Array<int> srcs;

    auto& pi = parameters.getReference (param->getModIndex());
    for (auto& si : pi.sources)
        srcs.add (si.id);

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

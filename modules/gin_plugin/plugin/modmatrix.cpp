
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

void ModMatrix::addParameter (Parameter* p)
{
    p->setModMatrix (this);
    p->setModIndex (parameters.size());

    ParamInfo pi;
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

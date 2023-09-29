
StepLFOComponent::StepLFOComponent (int maxSteps_)
    : maxSteps (maxSteps_)
{
    setName ("step");
    level.resize (size_t (maxSteps));
}

void StepLFOComponent::resized()
{
    dirty = true;
}

void StepLFOComponent::setParams (Parameter::Ptr beat_, Parameter::Ptr length_, Parameter::Ptr* level_, Parameter::Ptr enable_)
{
    unwatchParams();

    watchParam (beat   = beat_);
    watchParam (length = length_);
    watchParam (enable = enable_);
    
    for (int i = 0; i < maxSteps; i++)
        watchParam (level[size_t (i)] = level_[i]);

    startTimerHz (30);
}

void StepLFOComponent::paramChanged ()
{
    MultiParamComponent::paramChanged();
    dirty = true;
}

void StepLFOComponent::createPath (juce::Rectangle<int> area)
{
    lfo.setSampleRate ((double) area.getWidth());

    lfo.setFreq (1.0f * getNumSteps());
    lfo.setNumPoints (getNumSteps());
    for (int i = 0; i < getNumSteps(); i++)
        lfo.setPoint (i, level[size_t (i)]->getProcValue());

    lfo.reset();

    auto vToY = [&] (float v)
    {
        return area.getBottom() - (v + 1.0f) / 2.0f * area.getHeight();
    };

    curve.clear();
    path.clear();

    for (int x = area.getX(); x <= area.getRight(); x++)
    {
        auto v = lfo.process (1);

        if (x == area.getX())
            path.startNewSubPath ({float (area.getX()), vToY (v)});
        else
            path.lineTo ({float (x), vToY (v)});

        curve[x - area.getX()] = vToY (v);
    }
}

void StepLFOComponent::mouseDrag (const juce::MouseEvent& e)
{
    int step = int (e.x / float (getWidth()) * getNumSteps());
    if (step < 0 || step >= getNumSteps()) return;
        
    float l = -(e.y / float (getHeight()) * 2 - 1);
    l = juce::jlimit (-1.0f, 1.0f, l);
    
    level[size_t (step)]->setUserValue (l);
}

void StepLFOComponent::paint (juce::Graphics& g)
{
    auto rc = getLocalBounds().reduced (2);

    if (dirty)
    {
        dirty = false;
        createPath (rc);
    }

    g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.3f)));
    g.fillRect (rc.getX(), rc.getCentreY(), rc.getWidth(), 1);

    g.setColour (dimIfNeeded (findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f)));
    g.strokePath (path, juce::PathStrokeType (1.5f));

    if (isEnabled())
    {
        auto lerp = [] (float t, float a, float b)  { return a + t * (b - a); };

        for (auto curPhase : curPhases)
        {
            float x = curPhase * rc.getWidth();
            float t = x - int (x);
            float y = lerp (t, curve[int(x)], curve[int(x) + 1]);

            g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.9f)));
            g.fillEllipse (rc.getX() + x - 2, y - 2, 4, 4);
        }
    }
}

void StepLFOComponent::timerCallback()
{
    if (isEnabled() && phaseCallback)
    {
        auto newPhases = phaseCallback();
        if (newPhases != curPhases)
        {
            curPhases = newPhases;
            repaint();
        }
    }
}

int StepLFOComponent::getNumSteps()
{
    return int (length->getProcValue());
}

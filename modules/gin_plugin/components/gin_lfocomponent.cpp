
void LFOComponent::resized()
{
    dirty = true;
}

void LFOComponent::setParams (Parameter::Ptr wave_, Parameter::Ptr sync_, Parameter::Ptr rate_,
                              Parameter::Ptr beat_, Parameter::Ptr depth_, Parameter::Ptr offset_,
                              Parameter::Ptr phase_, Parameter::Ptr enable_)
{
    unwatchParams();

    watchParam (wave   = wave_);
    watchParam (sync   = sync_);
    watchParam (rate   = rate_);
    watchParam (beat   = beat_);
    watchParam (depth  = depth_);
    watchParam (offset = offset_);
    watchParam (phase  = phase_);
    watchParam (enable = enable_);

    startTimerHz (30);
}

void LFOComponent::paramChanged ()
{
    MultiParamComponent::paramChanged();
    dirty = true;
}

void LFOComponent::setBPM (float bpm_)
{
    bpm = bpm_;
}

void LFOComponent::createPath (juce::Rectangle<int> area)
{
    lfo.setSampleRate ((double) area.getWidth());

    LFO::Parameters p;
    p.waveShape = (LFO::WaveShape) int (wave->getProcValue());
    p.frequency = 1.0f * getNumSteps();
    p.phase     = phase->getProcValue();
    p.offset    = offset->getProcValue();
    p.depth     = depth->getProcValue();

    lfo.setParameters (p);
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

void LFOComponent::paint (juce::Graphics& g)
{
    auto rc = getLocalBounds().reduced (2);

    if (dirty)
    {
        dirty = false;
        createPath (rc);
    }

    auto c = findColour (isEnabled() ? GinLookAndFeel::colourId5 : GinLookAndFeel::colourId2);

    g.setColour (c.withMultipliedAlpha (0.35f));
    g.fillRect (rc.getX(), rc.getCentreY(), rc.getWidth(), 1);

    g.setColour (c.withMultipliedAlpha (0.5f));
    g.strokePath (path, juce::PathStrokeType (1.5f));

    if (isEnabled())
    {
        auto lerp = [] (float t, float a, float b)  { return a + t * (b - a); };

        float x = curPhase * rc.getWidth();
        float t = x - int (x);
        float y = lerp (t, curve[int(x)], curve[int(x) + 1]);

        g.setColour (c);
        g.fillEllipse (rc.getX() + x - 2, y - 2, 4, 4);
    }
}

void LFOComponent::timerCallback()
{
    if (lastUpdate == -1)
    {
        lastUpdate = juce::Time::getMillisecondCounter() / 1000.0;
    }
    else
    {
        double now = juce::Time::getMillisecondCounter() / 1000.0;
        double delta = now - lastUpdate;
        lastUpdate = now;

        if (sync->isOn())
        {
            auto duration = NoteDuration::getNoteDurations()[size_t (beat->getUserValue())];
            const float hz = 1.0f / duration.toSeconds (bpm);
            curPhase += float (hz * delta) / getNumSteps();
        }
        else
        {
            curPhase += float (rate->getProcValue() * delta) / getNumSteps();
        }

        curPhase = std::fmod (curPhase, 1.0f);
        if (std::isnan (curPhase) || std::isinf (curPhase))
            curPhase = 0.0f;

        repaint();
    }
}

int LFOComponent::getNumSteps()
{
    LFO::WaveShape shape = (LFO::WaveShape) int (wave->getProcValue());

    if (shape == LFO::WaveShape::sampleAndHold) return 8;
    if (shape == LFO::WaveShape::noise)         return 8;

    return 1;
}

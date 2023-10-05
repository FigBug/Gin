GateEffectComponent::GateEffectComponent (int maxSteps_)
    : maxSteps (maxSteps_)
{
    setName ("pattern");
    l.resize (size_t (maxSteps));
    r.resize (size_t (maxSteps));
}

void GateEffectComponent::setParams (Parameter::Ptr length_, Parameter::Ptr* l_, Parameter::Ptr* r_, Parameter::Ptr enable_)
{
    unwatchParams();

    watchParam (length = length_);
    watchParam (enable = enable_);
    
    for (int i = 0; i < 16; i++)
    {
        watchParam (l[size_t (i)] = l_[i]);
        watchParam (r[size_t (i)] = r_[i]);
    }
}

void GateEffectComponent::paramChanged ()
{
    MultiParamComponent::paramChanged();
}

void GateEffectComponent::mouseDown (const juce::MouseEvent& e)
{
    mouseDrag (e);
}

void GateEffectComponent::mouseDrag (const juce::MouseEvent& e)
{
    auto rc = getLocalBounds();

    if (e.y < rc.getY() || e.y > rc.getBottom()) return;
    int ch = e.y < getHeight() / 2 ? 0 : 1;

    auto data = (ch == 0) ? l : r;

    int step = int ((e.x - rc.getX()) / float (rc.getWidth()) * getNumSteps());
    if (step < 0 || step >= getNumSteps()) return;

    if (! dragging)
    {
        setOn = ! data[size_t (step)]->isOn();
        dragging = true;
    }

    data[size_t (step)]->setUserValue (setOn ? 1.0f : 0.0f);

    repaint();
}

void GateEffectComponent::mouseUp (const juce::MouseEvent&)
{
    dragging = false;
}

void GateEffectComponent::paint (juce::Graphics& g)
{
    auto rc = getLocalBounds().toFloat();
    float w = rc.getWidth() / float (getNumSteps());

    g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.3f)));
    for (int i = 0; i <= getNumSteps(); i++)
    {
        auto x = juce::roundToInt (rc.getX() + i * w);
        g.drawLine (float (x), rc.getY(), float (x), rc.getBottom ());
    }
    g.drawLine (rc.getX(), rc.getCentreY(), rc.getRight(), rc.getCentreY());

    g.setColour (dimIfNeeded (findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f)));
    for (int i = 0; i < getNumSteps(); i++)
    {
        if (l[size_t (i)]->isOn())
        {
            auto rs = juce::Rectangle<float> (rc.getX() + i * w, rc.getY(), w, rc.getCentreY() - rc.getY());
            g.fillRect (rs.reduced (3));
        }
        if (r[size_t (i)]->isOn())
        {
            auto rs = juce::Rectangle<float> (rc.getX() + i * w, rc.getCentreY(), w, rc.getBottom() - rc.getCentreY());
            g.fillRect (rs.reduced (3));
        }
    }
}

int GateEffectComponent::getNumSteps()
{
    return int (length->getProcValue());
}

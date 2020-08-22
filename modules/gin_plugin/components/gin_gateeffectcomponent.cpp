
void GateEffectComponent::setParams (Parameter::Ptr length_, Parameter::Ptr* l_, Parameter::Ptr* r_, Parameter::Ptr enable_)
{
    unwatchParams();

    watchParam (length = length_);
    watchParam (enable = enable_);
    
    for (int i = 0; i < 32; i++)
    {
        watchParam (l[i] = l_[i]);
        watchParam (r[i] = r_[i]);
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
    auto rc = getLocalBounds().reduced (2, 20);

    if (e.y < rc.getY() || e.y > rc.getBottom()) return;
    int ch = e.y < getHeight() / 2 ? 0 : 1;

    auto data = (ch == 0) ? l : r;

    int step = int ((e.x - rc.getX()) / float (rc.getWidth()) * getNumSteps());
    if (step < 0 || step >= getNumSteps()) return;

    if (! dragging)
    {
        setOn = ! data[step]->isOn();
        dragging = true;
    }

    data[step]->setUserValue (setOn ? 1.0f : 0.0f);

    repaint();
}

void GateEffectComponent::mouseUp (const juce::MouseEvent&)
{
    dragging = false;
}

void GateEffectComponent::paint (juce::Graphics& g)
{
    auto rc = getLocalBounds().reduced (2, 20);
    float w = rc.getWidth() / float (getNumSteps());

    auto c = findColour (isEnabled() ? GinLookAndFeel::colourId5 : GinLookAndFeel::colourId2);

    g.setColour (c.withMultipliedAlpha (0.25f));
    for (int i = 0; i <= getNumSteps(); i++)
        g.drawVerticalLine (juce::roundToInt (rc.getX() + i * w), float (rc.getY ()), float (rc.getBottom ()));
    g.drawHorizontalLine (rc.getCentreY(), float (rc.getX()), float (rc.getRight()));

    g.setColour (c.withMultipliedAlpha (0.20f));
    for (int i = 0; i < getNumSteps(); i++)
    {
        if (l[i]->isOn())
        {
            auto rs = juce::Rectangle<int> (juce::roundToInt (rc.getX() + i * w),
                                      rc.getY(),
                                      juce::roundToInt (rc.getX() + (i + 1) * w) - juce::roundToInt (rc.getX() + i * w),
                                      rc.getCentreY() - rc.getY());

            g.fillRect (rs.reduced (3));
        }
        if (r[i]->isOn())
        {
            auto rs = juce::Rectangle<int> (juce::roundToInt (rc.getX() + i * w),
                                      rc.getCentreY(),
                                      juce::roundToInt (rc.getX() + (i + 1) * w) - juce::roundToInt (rc.getX() + i * w),
                                      rc.getBottom() - rc.getCentreY());

            g.fillRect (rs.reduced (3));
        }
    }
}

int GateEffectComponent::getNumSteps()
{
    return int (length->getProcValue());
}

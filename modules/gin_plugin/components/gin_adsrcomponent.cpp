#pragma once

ADSRComponent::ADSRComponent()
{
    setName ("adsr");
}

void ADSRComponent::setParams (Parameter::Ptr attack_,
                               Parameter::Ptr decay_,
                               Parameter::Ptr sustain_,
                               Parameter::Ptr release_)
{
    unwatchParams();
    watchParam (attack = attack_);
    watchParam (decay = decay_);
    watchParam (sustain = sustain_);
    watchParam (release = release_);

    repaint();
    startTimerHz (30);
}

juce::Rectangle<int> ADSRComponent::getArea()
{
    return getLocalBounds().reduced (handleSz);
}

juce::String ADSRComponent::getBubbleText()
{
    switch (handle)
    {
        case Handle::attack:        return attack->getCurrentValueAsText();
        case Handle::decaySustain:  return decay->getCurrentValueAsText() + "\n" + sustain->getCurrentValueAsText();
        case Handle::release:       return release->getCurrentValueAsText();
        case Handle::none:
        default:                    return {};
    }
}

void ADSRComponent::timerCallback()
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

void ADSRComponent::paint (juce::Graphics& g)
{
    auto c = findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f);

    auto a = getArea();
    auto p1 = juce::Point<float> (float (a.getX()), float (a.getBottom()));
    auto p2 = getHandlePos (Handle::attack).toFloat();
    auto p3 = getHandlePos (Handle::decaySustain).toFloat();
    auto p4 = getHandlePos (Handle::release).toFloat();

    // Draw curve
    {
        juce::Path p;

        p.startNewSubPath (p1);
        p.lineTo (p2);
        p.lineTo (p3);
        p.lineTo (p4);

        g.setColour (dimIfNeeded (c));
        g.strokePath (p, juce::PathStrokeType (2));
    }

    // Draw handles
    {
        juce::Colour back = juce::Colours::black;

        g.setColour (back);
        g.fillEllipse (getHandleRect (Handle::attack).toFloat());
        g.fillEllipse (getHandleRect (Handle::decaySustain).toFloat());
        g.fillEllipse (getHandleRect (Handle::release).toFloat());

        auto h = findColour (GinLookAndFeel::whiteColourId).withAlpha (0.9f);
        g.setColour (dimIfNeeded (h));

        g.drawEllipse (getHandleRect (Handle::attack).toFloat(), 0.75f);
        g.drawEllipse (getHandleRect (Handle::decaySustain).toFloat(), 0.75f);
        g.drawEllipse (getHandleRect (Handle::release).toFloat(), 0.75f);
    }

    // Draw dots
    {
        auto l1 = juce::Line<float> (p1, p2);
        auto l2 = juce::Line<float> (p2, p3);
        auto l3 = juce::Line<float> (p3, p4);

        for (auto dot : curPhases)
        {
            auto x = 0.0f;
            auto y = (a.getHeight() * (1.0f - dot.second)) + a.getY();

            if (dot.first == 0) // A
            {
                y = std::clamp (y, l1.getEndY(), l1.getStartY());
                x = getXForY (l1, y);
            }
            else if (dot.first == 1) // D
            {
                y = std::clamp (y, l2.getStartY(), l2.getEndY());
                x = getXForY (l2, y);
            }
            else if (dot.first == 2) // S
            {
                x = l3.getStartX();
            }
            else if (dot.first == 3) // R
            {
                y = std::clamp (y, l3.getStartY(), l3.getEndY());
                x = getXForY (l3, y);
            }

            g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.9f)));
            g.fillEllipse (x - 2, y - 2, 4, 4);
        }
    }
}

void ADSRComponent::mouseDown (const juce::MouseEvent& e)
{
    if (! isEnabled()) return;

    handle = getHandleAt (e.getPosition());
    switch (handle)
    {
        case Handle::attack:
            attack->beginUserAction();
            dx = getHandlePos (Handle::attack).getX() - e.x;
            break;
        case Handle::decaySustain:
            decay->beginUserAction();
            sustain->beginUserAction();
            dx = getHandlePos (Handle::decaySustain).getX() - e.x;
            dy = getHandlePos (Handle::decaySustain).getY() - e.y;
            break;
        case Handle::release:
            release->beginUserAction();
            dx = getHandlePos (Handle::release).getX() - e.x;
            break;
        case Handle::none:
        default:
            break;
    }
}

void ADSRComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (! isEnabled()) return;

    auto a = getArea();
    switch (handle)
    {
        case Handle::attack:
            attack->setValueNotifyingHost (xToParam (float (e.x - a.getX() + dx)));
            showBubble (getBubbleText(), getHandleRect (handle));
            break;
        case Handle::decaySustain:
            decay->setValueNotifyingHost (xToParam (float (e.x + dx - getHandlePos (Handle::attack).getX())));
            sustain->setValueNotifyingHost (1.0f - (e.y - a.getY() + dy) / float (a.getHeight()));
            showBubble (getBubbleText(), getHandleRect (handle));
            break;
        case Handle::release:
            release->setValueNotifyingHost (xToParam (float (e.x + dx - getHandlePos (Handle::decaySustain).getX())));
            showBubble (getBubbleText(), getHandleRect (handle));
            break;
        case Handle::none:
        default:
            break;
    }
}

void ADSRComponent::mouseUp (const juce::MouseEvent&)
{
    if (! isEnabled()) return;

    switch (handle)
    {
        case Handle::attack:
            attack->endUserAction();
            break;
        case Handle::decaySustain:
            decay->endUserAction();
            sustain->endUserAction();
            break;
        case Handle::release:
            release->endUserAction();
            break;
        case Handle::none:
        default:
            break;
    }
    handle = Handle::none;
    hideBubble();
}

juce::MouseCursor ADSRComponent::getMouseCursor()
{
    if (isEnabled())
    {
        auto h = handle;
        if (h == Handle::none)
            h = getHandleAt (getMouseXYRelative());

        if (h == Handle::attack)        return juce::MouseCursor::LeftRightResizeCursor;
        if (h == Handle::decaySustain)  return juce::MouseCursor::UpDownLeftRightResizeCursor;
        if (h == Handle::release)       return juce::MouseCursor::LeftRightResizeCursor;
    }
    return juce::MouseCursor::NormalCursor;
}

ADSRComponent::Handle ADSRComponent::getHandleAt (juce::Point<int> pt)
{
    if (getHandleRect (Handle::attack).contains (pt))          return Handle::attack;
    if (getHandleRect (Handle::decaySustain).contains (pt))    return Handle::decaySustain;
    if (getHandleRect (Handle::release).contains (pt))         return Handle::release;
    return Handle::none;
}

juce::Point<int> ADSRComponent::getHandlePos (Handle g)
{
    auto a = getArea();

    switch (g)
    {
        case Handle::attack:
            return { a.getX() + paramToX (attack->getValue()),
                     a.getY() };
        case Handle::decaySustain:
            return { a.getX() + paramToX (attack->getValue()) + paramToX (decay->getValue()),
                a.getY() + juce::roundToInt (a.getHeight() * (1.0f - sustain->getProcValue())) };
        case Handle::release:
            return { a.getX() + paramToX (attack->getValue()) + paramToX (decay->getValue()) + paramToX (release->getValue()),
                     a.getBottom() };
         case Handle::none:
        default:
            return {};
    }
}

juce::Rectangle<int> ADSRComponent::getHandleRect (Handle h)
{
    auto pt = getHandlePos (h);
    return { pt.getX() - handleSz / 2, pt.getY() - handleSz / 2, handleSz, handleSz };
}

int ADSRComponent::paramToX (float t)
{
    t = juce::jlimit (0.0f, 1.0f, t);
    auto a = getArea();
    return juce::roundToInt (a.getWidth() / 3.0f * t);
}

float ADSRComponent::xToParam (float x)
{
    auto a = getArea();
    return juce::jlimit (0.0f, 1.0f, x / float (a.getWidth()) * 3.0f);
}


#pragma once

void ADSRComponent::setParams (Parameter::Ptr attack_,
                               Parameter::Ptr decay_,
                               Parameter::Ptr sustain_,
                               Parameter::Ptr release_)
{
    clearParams();
    addParam (attack = attack_);
    addParam (decay = decay_);
    addParam (sustain = sustain_);
    addParam (release = release_);

    repaint();
}

juce::Rectangle<int> ADSRComponent::getArea()
{
    return getLocalBounds().reduced (handleSz);
}

String ADSRComponent::getBubbleText()
{
    switch (handle)
    {
        case Handle::attack:        return attack->getCurrentValueAsText();
        case Handle::decaySustain:  return decay->getCurrentValueAsText() + "\n" + sustain->getCurrentValueAsText();
        case Handle::release:       return release->getCurrentValueAsText();
        default:                    return {};
    }
}

void ADSRComponent::paint (Graphics& g)
{
    auto a = getArea();
    auto p1 = Point<float> (a.getX(), a.getBottom());
    auto p2 = getHandlePos (Handle::attack).toFloat();
    auto p3 = getHandlePos (Handle::decaySustain).toFloat();
    auto p4 = getHandlePos (Handle::release).toFloat();

    Path p;

    g.setColour (Colours::white.withMultipliedAlpha (0.5f));

    p.startNewSubPath (p1);
    p.lineTo (p2);
    p.lineTo (p3);
    p.lineTo (p4);

    g.strokePath (p, PathStrokeType (2));

    Colour back = Colours::black;

    g.setColour (back);
    g.fillRoundedRectangle (getHandleRect (Handle::attack).toFloat(), 3);
    g.fillRoundedRectangle (getHandleRect (Handle::decaySustain).toFloat(), 3);
    g.fillRoundedRectangle (getHandleRect (Handle::release).toFloat(), 3);

    g.setColour (Colours::white);

    g.drawRect (getHandleRect (Handle::attack).toFloat());
    g.drawRect (getHandleRect (Handle::decaySustain).toFloat());
    g.drawRect (getHandleRect (Handle::release).toFloat());
}

void ADSRComponent::mouseDown (const MouseEvent& e)
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
        default:
            break;
    }
}

void ADSRComponent::mouseDrag (const MouseEvent& e)
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
        default:
            break;
    }
}

void ADSRComponent::mouseUp (const MouseEvent&)
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
        default:
            break;
    }
    handle = Handle::none;
    hideBubble();
}

MouseCursor ADSRComponent::getMouseCursor()
{
    auto h = handle;
    if (h == Handle::none)
        h = getHandleAt (getMouseXYRelative());

    if (h == Handle::attack)        return MouseCursor::LeftRightResizeCursor;
    if (h == Handle::decaySustain)  return MouseCursor::UpDownLeftRightResizeCursor;
    if (h == Handle::release)       return MouseCursor::LeftRightResizeCursor;
    return MouseCursor::NormalCursor;
}

ADSRComponent::Handle ADSRComponent::getHandleAt (Point<int> pt)
{
    if (getHandleRect (Handle::attack).contains (pt))          return Handle::attack;
    if (getHandleRect (Handle::decaySustain).contains (pt))    return Handle::decaySustain;
    if (getHandleRect (Handle::release).contains (pt))         return Handle::release;
    return Handle::none;
}

Point<int> ADSRComponent::getHandlePos (Handle g)
{
    auto a = getArea();

    switch (g)
    {
        case Handle::attack:
            return { a.getX() + paramToX (attack->getValue()),
                     a.getY() };
        case Handle::decaySustain:
            return { a.getX() + paramToX (attack->getValue()) + paramToX (decay->getValue()),
                     a.getY() + roundToInt (a.getHeight() * (1.0f - sustain->getProcValue())) };
        case Handle::release:
            return { a.getX() + paramToX (attack->getValue()) + paramToX (decay->getValue()) + paramToX (release->getValue()),
                     a.getBottom() };
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
    t = jlimit (0.0f, 1.0f, t);
    auto a = getArea();
    return roundToInt (a.getWidth() / 3.0f * t);
}

float ADSRComponent::xToParam (float x)
{
    auto a = getArea();
    return jlimit (0.0f, 1.0f, x / float (a.getWidth()) * 3.0f);
}


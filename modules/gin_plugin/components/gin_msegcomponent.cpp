
MSEGComponent::MSEGComponent (MSEG::Data& d) : data (d)
{
    setName ("mseg");
}

void MSEGComponent::resized()
{
    dirty = true;
}

void MSEGComponent::setParams (Parameter::Ptr wave_, Parameter::Ptr sync_, Parameter::Ptr rate_,
                               Parameter::Ptr beat_, Parameter::Ptr depth_, Parameter::Ptr offset_,
                               Parameter::Ptr phase_, Parameter::Ptr enable_, Parameter::Ptr xgrid_,
                               Parameter::Ptr ygrid_, Parameter::Ptr loop_)
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
    watchParam (xgrid  = xgrid_);
    watchParam (ygrid  = ygrid_);
    watchParam (loop   = loop_);

    startTimerHz (30);
}

void MSEGComponent::paramChanged ()
{
    MultiParamComponent::paramChanged();
    dirty = true;
}

void MSEGComponent::createPath()
{
    mseg.setSampleRate (44100.0);

    MSEG::Parameters p;
    p.frequency = 1.0f;

    if (! editable)
    {
        p.phase     = phase->getProcValue();
        p.offset    = offset->getProcValue();
        p.depth     = depth->getProcValue();
    }
    else
    {
        p.phase     = 0.0f;
        p.offset    = 0.0f;
        p.depth     = 1.0f;
    }

    mseg.setParameters (p);
    mseg.reset();

    path.clear();

    path.startNewSubPath (timeToX (0), valueToY (data.points[0].value));

    for (auto i = 0; i < data.numPoints - 1; i++)
    {
        auto p1 = data.points[i + 0];
        auto p2 = data.points[i + 1];

        for (auto x = timeToX (p1.time); x < timeToX (p2.time); x++)
            path.lineTo ({x, valueToY (mseg.getValueAt (xToTime (x))) });

        path.lineTo ({timeToX (p2.time), valueToY (p2.value) });
    }
}

void MSEGComponent::paint (juce::Graphics& g)
{
    auto rc = getArea();

    if (dirty)
    {
        dirty = false;
        createPath();
    }

    g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.1f)));

    juce::RectangleList<float> rects;

    // Draw grid
    if (editable)
    {
        for (int i = 0; i <= ygrid->getUserValueInt(); i++)
            rects.add ({rc.getX(), rc.getY() + i * rc.getHeight() / ygrid->getUserValueInt(), rc.getWidth(), 1});
        
        for (int i = 0; i <= xgrid->getUserValueInt(); i++)
            rects.add ({rc.getX() + i * rc.getWidth() / xgrid->getUserValueInt(), rc.getY(), 1, rc.getHeight()});
    }
    else
    {
        rects.add ({rc.getX(), rc.getCentreY(), rc.getWidth(), 1});
    }

    g.fillRectList (rects);

    auto c = findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f);

    g.setColour (dimIfNeeded (c));
    g.strokePath (path, juce::PathStrokeType (1.5f));

    if (isEnabled())
    {
        for (auto curPhase : curPhases)
        {
            float x = timeToX (curPhase);
            float y = valueToY (mseg.getValueAt (curPhase));

            g.setColour (dimIfNeeded (findColour (GinLookAndFeel::whiteColourId).withAlpha (0.9f)));
            g.fillEllipse (x - 2, y - 2, 4, 4);
        }
    }

    if (editable)
    {
        for (auto i = 0; i < data.numPoints; i++)
        {
            auto r = juce::Rectangle (timeToX (data.points[i].time) - 2.5f, valueToY (data.points[i].value) - 2.5f, 5.0f, 5.0f);

            if (draggingPoint == i || getPointAt (getMouseXYRelative().toFloat()) == i)
            {
                g.setColour (dimIfNeeded (juce::Colours::white).withAlpha (0.3f));
                g.fillEllipse (r.expanded (4));
            }

            if (loop->getUserValueInt() == 1 && i == data.startIndex)
                g.setColour (dimIfNeeded (juce::Colours::green).withAlpha (1.0f));
            else if (loop->getUserValueInt() == 1 && i == data.endIndex)
                g.setColour (dimIfNeeded (juce::Colours::red).withAlpha (1.0f));
            else
                g.setColour (dimIfNeeded (findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f)));

            g.fillEllipse (r);
        }

        for (auto i = 0; i < data.numPoints - 1; i++)
        {
            auto t = (data.points[i].time + data.points[i + 1].time) / 2.0f;
            auto r = juce::Rectangle (timeToX (t) - 2.0f, valueToY (mseg.getValueAt (t)) - 2.0f, 4.0f, 4.0f);

            if (draggingCurve == i || getCurveAt (getMouseXYRelative().toFloat()) == i)
            {
                g.setColour (dimIfNeeded (juce::Colours::white).withAlpha (0.3f));
                g.fillEllipse (r.expanded (4));
            }

            g.setColour (dimIfNeeded (findColour (GinLookAndFeel::accentColourId).withAlpha (0.7f)));
            g.drawEllipse (r, 1.0f);
        }
    }
}

void MSEGComponent::timerCallback()
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

float MSEGComponent::valueToY (float v)
{
    auto area = getArea();
    return area.getBottom() - (v + 1.0f) / 2.0f * area.getHeight();
}

float MSEGComponent::timeToX (float t)
{
    auto area = getArea();
    return area.getX() + area.getWidth() * t;
}

float MSEGComponent::xToTime (float x)
{
    auto area = getArea();
    return (x - area.getX()) / area.getWidth();
}

float MSEGComponent::yToValue (float y)
{
    auto area = getArea();
    return 1.0f -((y - area.getY()) / getHeight()) * 2.0f;
}

int MSEGComponent::getPointAt (juce::Point<float> p)
{
    for (auto i = 0; i < data.numPoints; i++)
    {
        auto x = timeToX (data.points[i].time);
        auto y = valueToY (data.points[i].value);

        if (juce::Rectangle<float> (p.x - 3.0f, p.y - 3.0f, 6.0f, 6.0f).contains (x, y))
            return i;
    }
    return -1;
}

int MSEGComponent::getCurveAt (juce::Point<float> p)
{
    for (auto i = 0; i < data.numPoints - 1; i++)
    {
        auto t = (data.points[i].time + data.points[i + 1].time) / 2.0f;

        auto x = timeToX (t);
        auto y = valueToY (mseg.getValueAt (t));

        if (juce::Rectangle<float> (p.x - 3.0f, p.y - 3.0f, 6.0f, 6.0f).contains (x, y))
            return i;
    }
    return -1;
}

void MSEGComponent::mouseDown (const juce::MouseEvent& e)
{
    if (! editable)
        return;

    if ((draggingPoint = getPointAt (e.position)) >= 0)
        repaint ();
    else if ((draggingCurve = getCurveAt (e.position)) >= 0)
        repaint ();

    if (e.getNumberOfClicks() == 2)
    {
        if (draggingPoint >= 0)
        {
            // delete point
            if (draggingPoint == 0 || draggingPoint == data.numPoints - 1)
                return;

            for (int i = draggingPoint; i < data.numPoints - 1; i++)
                data.points.getReference (i) = data.points[i + 1];

            data.numPoints--;

            if (data.startIndex >= draggingPoint)
                data.startIndex = std::max (0, data.startIndex - 1);

            if (data.endIndex >= draggingPoint)
                data.endIndex = std::min (data.numPoints - 1, data.endIndex - 1);

            dirty = true;
            repaint();
        }
        else
        {
            // add point
            if (data.numPoints == data.maxPoints)
                return;

            auto t = snapT (xToTime (e.position.x));
            auto v = snapV (yToValue (e.position.y));

            for (auto i = 0; i < data.numPoints - 1; i++)
            {
                auto p1 = data.points[i + 0];
                auto p2 = data.points[i + 1];

                if (t > p1.time && t <= p2.time)
                {
                    for (auto j = data.numPoints; j >= i + 1; j--)
                        data.points.getReference (j) = data.points[j - 1];

                    data.points.getReference (i + 1) = { t, v, 0.0f };
                    data.numPoints++;

                    if (data.startIndex >= i + 1)
                        data.startIndex++;

                    if (data.endIndex >= i + 1)
                        data.endIndex++;

                    break;
                }
            }

            dirty = true;
            repaint();
        }
    }

    lastY = e.position.y;
}

void MSEGComponent::mouseMove (const juce::MouseEvent&)
{
    if (! editable)
        return;

    repaint();
}

void MSEGComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (! editable)
        return;

    if (draggingPoint >= 0)
    {
        auto& p = data.points.getReference (draggingPoint);

        auto minT = 0.0f;
        auto maxT = 0.0f;

        if (draggingPoint > 0)
            minT = data.points[draggingPoint - 1].time;

        if (draggingPoint < data.numPoints - 1)
            maxT = data.points[draggingPoint + 1].time;

        p.time  = snapT (std::clamp (xToTime (e.position.x), minT, maxT));
        p.value = snapV (std::clamp (yToValue (e.position.y), -1.0f, 1.0f));

        if (draggingPoint == 0)                  
        {
            p.time = 0.0f;
            data.points.getReference (data.numPoints - 1).value = p.value;
        }
        if (draggingPoint == data.numPoints - 1)
        {
            p.time = 1.0f;
            data.points.getReference (0).value = p.value;
        }

        dirty = true;
        repaint ();

        auto r = juce::Rectangle<int> (juce::roundToInt (timeToX (data.points[draggingPoint].time) - 2),
                                       juce::roundToInt (valueToY (data.points[draggingPoint].value) - 2),
                                       4, 4);

        juce::String ts;
        if (sync->getUserValueBool())
        {
            auto dur = gin::NoteDuration::getNoteDurations()[size_t (beat->getUserValueInt())];
            if (auto b = dur.getBars(); b > 0)
                ts = juce::String::formatted ("%.2f bars", b * p.time);
            else
                ts = juce::String::formatted ("%.2f beats", dur.getBeats() * p.time);
        }
        else
        {
            ts = juce::String::formatted ("%.2fs", p.time * 1.0f / rate->getUserValue());
        }

        showBubbleMessage (r.expanded (5), ts + juce::String::formatted (": %.1f%%", p.value * 100));
    }
    else if (draggingCurve >= 0)
    {
        auto& p = data.points.getReference (draggingCurve);

        auto factor = easeQuadraticOut (juce::jmap (std::abs (p.curve), 0.0f, 50.0f, 0.0f, 1.0f));
        auto speed  = juce::jmap (factor, 0.0f, 1.0f, 0.01f, 0.2f);
        auto dir    = data.points[draggingCurve].value > data.points[draggingCurve + 1].value ? -1.0f : 1.0f;

        p.curve = std::clamp (p.curve + ((e.y - lastY) * speed * dir), -50.0f, 50.0f);

        dirty = true;
        repaint ();

        auto t = (data.points[draggingCurve].time + data.points[draggingCurve + 1].time) / 2.0f;
        auto r = juce::Rectangle<int> (juce::roundToInt (timeToX (t) - 2),
                                       juce::roundToInt (valueToY (mseg.getValueAt (t)) - 2),
                                       4, 4);

        showBubbleMessage (r.expanded (5), juce::String (p.curve, 1));
    }
    lastY = e.position.y;
}

void MSEGComponent::mouseUp (const juce::MouseEvent& e)
{
    if (onClick && e.mouseWasClicked() && ! e.mods.isPopupMenu())
        onClick();

    if (e.mods.isPopupMenu() && (! editable || (draggingPoint == -1 && draggingCurve == -1)))
    {
        juce::PopupMenu m;
        if (onLoad) m.addItem ("Load...", [this] { onLoad(); });
        if (onSave) m.addItem ("Save...", [this] { onSave(); });
        m.addSeparator();
        m.addItem ("Clear", [this]
        {
            data.numPoints = 2;
            data.startIndex = 0;
            data.endIndex = 1;
            data.points.getReference (0) = { 0.0f, 0.0f, 0.0f };
            data.points.getReference (1) = { 1.0f, 0.0f, 0.0f };
            dirty = true;
            repaint();
        });
        m.showMenuAsync ({});

        return;
    }

    if (! editable)
        return;

    if (e.mods.isPopupMenu())
    {
        if (draggingPoint >= 0 && loop->getUserValueInt() == 1)
        {
            juce::PopupMenu m;
            m.addItem ("Set Loop Start", draggingPoint < data.numPoints - 1, false, [this, dp = draggingPoint]
            {
                data.startIndex = dp;

                if (data.endIndex <= dp)
                    data.endIndex = dp + 1;

                repaint();
            });
            m.addItem ("Set Loop End", draggingPoint > 0, false, [this, dp = draggingPoint]
            {
                data.endIndex = dp;

                if (data.startIndex >= dp)
                    data.startIndex = dp - 1;

                repaint();
            });

            m.showMenuAsync ({});
        }
    }

    draggingPoint = -1;
    draggingCurve = -1;
    repaint ();

    hideBubbleMessage();
}

void MSEGComponent::mouseEnter (const juce::MouseEvent&)
{
    if (! editable)
        return;

    repaint ();
}

void MSEGComponent::mouseExit (const juce::MouseEvent&)
{
    if (! editable)
        return;

    repaint ();
}

void MSEGComponent::showBubbleMessage (const juce::Rectangle<int>& rc, const juce::String& message)
{
    auto parent = findParentComponentOfClass<juce::AudioProcessorEditor>();
    if (! parent)
        return;

    if (bubbleMessage == nullptr)
    {
        bubbleMessage = std::make_unique<juce::BubbleMessageComponent> (-1);
        bubbleMessage->setAllowedPlacement (juce::BubbleComponent::above | juce::BubbleComponent::below);
        parent->addAndMakeVisible (*bubbleMessage);
    }

    juce::AttributedString attString;
    attString.append (message, juce::Font (13.0f));
    attString.setColour (juce::Colours::white);

    bubbleMessage->setAlwaysOnTop (true);
    bubbleMessage->setVisible (true);
    bubbleMessage->showAt (parent->getLocalArea (this, rc), attString, -1, false, false);
}

void MSEGComponent::hideBubbleMessage()
{
    bubbleMessage = nullptr;
}

float MSEGComponent::snapT (float t)
{
    if (juce::ModifierKeys::currentModifiers.isShiftDown())
        return t;
    
    auto d = 1.0f / getWidth() * 10.0f;
    
    for (int i = 0; i <= xgrid->getUserValueInt(); i++)
    {
        auto step = i * 1.0f / xgrid->getUserValueInt();
        if (std::abs (step - t) < d)
            return step;
    }
    
    return t;
}

float MSEGComponent::snapV (float v)
{
    if (juce::ModifierKeys::currentModifiers.isShiftDown())
        return v;
    
    auto d = 1.0f / getHeight() * 10.0f;
    
    for (int i = 0; i <= ygrid->getUserValueInt(); i++)
    {
        auto step = -1.0f + i * 2.0f / ygrid->getUserValueInt();
        if (std::abs (step - v) < d)
            return step;
    }

    return v;
}

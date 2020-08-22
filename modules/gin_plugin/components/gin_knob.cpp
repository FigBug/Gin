
//==============================================================================
Knob::Knob (Parameter* p, bool fromCentre)
  : ParamComponent (p),
    value (parameter),
    knob (parameter, juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (knob);
    addChildComponent (modButton);

    knob.setDoubleClickReturnValue (true, parameter->getUserDefaultValue());
    knob.setSkewFactor (parameter->getSkew(), parameter->isSkewSymmetric());
    if (fromCentre)
        knob.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), juce::dontSendNotification);
    value.setJustificationType (juce::Justification::centredTop);
    name.setJustificationType (juce::Justification::centredBottom);

    value.setVisible (false);

    addMouseListener (this, true);

    if (parameter->getModIndex() >= 0)
    {
        auto& mm = *parameter->getModMatrix();
        mm.addListener (this);
    }

    modTimer.onTimer = [this] ()
    {
        auto& mm = *parameter->getModMatrix();
        auto curModValues = liveValuesCallback ? liveValuesCallback() : mm.getLiveValues (parameter);
        if (curModValues != modValues)
        {
            modValues = curModValues;

            juce::Array<juce::var> vals;
            for (auto v : modValues)
                vals.add (v);

            knob.getProperties().set ("modValues", vals);

            repaint();
        }
    };
    shiftTimer.onTimer = [this] ()
    {
        bool shift = juce::ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
        knob.setInterceptsMouseClicks (! learning || shift, ! learning || shift );
    };

    modButton.onClick = [this] { showModMenu(); };
    modMatrixChanged();
}

Knob::~Knob()
{
    if (parameter->getModIndex() >= 0)
    {
        auto& mm = *parameter->getModMatrix();
        mm.removeListener (this);
    }
}

void Knob::showModMenu()
{
    juce::PopupMenu m;

    auto& mm = *parameter->getModMatrix();
    for (auto src : mm.getModSources (parameter))
    {
        m.addItem ("Remove " + mm.getModSrcName (src), [this, src]
        {
            parameter->getModMatrix()->clearModDepth (src, ModDstId (parameter->getModIndex()));
        });
    }

    m.showMenuAsync ({});
}

void Knob::resized()
{
    juce::Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    value.setBounds (rc);
    knob.setBounds (r.reduced (2));

    modButton.setBounds (knob.getBounds().removeFromTop (7).removeFromRight (7));
}

void Knob::mouseEnter (const juce::MouseEvent&)
{
    if (! isTimerRunning() && isEnabled())
    {
        startTimer (100);
        name.setVisible (false);
        value.setVisible (true);
    }
}

void Knob::timerCallback()
{
    auto p = getMouseXYRelative();
    if (! getLocalBounds().contains (p) &&
        ! juce::ModifierKeys::getCurrentModifiers().isAnyMouseButtonDown() &&
        ! value.isEditing())
    {
        name.setVisible (true);
        value.setVisible (false);

        stopTimer();
    }
}

void Knob::learnSourceChanged (ModSrcId src)
{
    learning = src.isValid();

    bool shift = juce::ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    knob.setInterceptsMouseClicks (! learning || shift, ! learning || shift );

    auto& mm = *parameter->getModMatrix();
    modDepth = mm.getModDepth (mm.getLearn(), ModDstId (parameter->getModIndex()));

    if (learning)
    {
        knob.getProperties().set ("modDepth", modDepth);
        knob.getProperties().set ("modBipolar", mm.getModSrcBipolar (mm.getLearn()));
     
        shiftTimer.startTimerHz (100);
    }
    else
    {
        knob.getProperties().remove ("modDepth");
        knob.getProperties().remove ("modBipolar");
        
        shiftTimer.stopTimer();
    }

    repaint();
}

void Knob::modMatrixChanged()
{
    if (auto mm = parameter->getModMatrix())
    {
        auto dst = ModDstId (parameter->getModIndex());
        
        if (mm->isModulated (dst) || liveValuesCallback)
        {
            modTimer.startTimerHz (30);
            modButton.setVisible (mm->isModulated (dst));
        }
        else
        {
            modTimer.stopTimer();
            knob.getProperties().remove ("modValues");
            modButton.setVisible (false);
        }

        if (learning && ! isMouseButtonDown (true))
        {
            modDepth = mm->getModDepth (mm->getLearn(), dst);
            knob.getProperties().set ("modDepth", modDepth);
            repaint();
        }
    }
}

void Knob::mouseDown (const juce::MouseEvent& e) 
{
    bool shift = juce::ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    if (shift || ! learning || ! knob.getBounds().contains (e.getMouseDownPosition()))
        return;

    auto& mm = *parameter->getModMatrix();
    auto dst = ModDstId (parameter->getModIndex());
    modDepth = mm.getModDepth (mm.getLearn(), dst);

    knob.getProperties().set ("modDepth", modDepth);

    repaint();
}

void Knob::mouseDrag (const juce::MouseEvent& e)
{
    bool shift = juce::ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    if (shift || ! learning || ! knob.getBounds().contains (e.getMouseDownPosition()))
         return;

    if (e.getDistanceFromDragStart() >= 3)
    {
        auto pt = e.getMouseDownPosition();
        auto delta = (e.position.x - pt.getX()) + (pt.getY() - e.position.y);

        float newModDepth = juce::jlimit (-1.0f, 1.0f, delta / 200.0f + modDepth);

        knob.getProperties().set ("modDepth", newModDepth);

        auto& mm = *parameter->getModMatrix();
        auto dst = ModDstId (parameter->getModIndex());
        mm.setModDepth (mm.getLearn(), dst, newModDepth);

        repaint();
    }
}


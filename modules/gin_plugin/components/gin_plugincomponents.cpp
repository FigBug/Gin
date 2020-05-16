
//==============================================================================
Readout::Readout (Parameter* parameter_)
  : parameter (parameter_)
{
    parameter->addListener (this);
    setText (parameter->getUserValueText(), dontSendNotification);

    addChildComponent (editor);
    editor.addListener (this);
    editor.setOpaque (false);

    setBorderSize (BorderSize<int> (0));
}

Readout::~Readout()
{
    parameter->removeListener (this);
}

void Readout::parameterChanged (Parameter*)
{
    setText (parameter->getUserValueText(), dontSendNotification);
}

void Readout::paint (Graphics& g)
{
    if (! editing)
        Label::paint (g);
}

void Readout::resized()
{
    editor.setBounds (getLocalBounds());
}

void Readout::mouseDown (const MouseEvent&)
{
    editing = true;
    editor.setVisible (true);
    editor.setFont (getFont());
    editor.setText (parameter->getUserValueText(), dontSendNotification);
    editor.setJustificationType (getJustificationType());
    editor.setSelectAllWhenFocused (true);
    editor.toFront (true);
    repaint();
}

void Readout::sltextEditorReturnKeyPressed (SingleLineTextEditor&)
{
    float v = editor.getText().getFloatValue();
    parameter->setUserValueAsUserAction (v);

    editor.setVisible (false);
    editing = false;
    repaint();
}

void Readout::sltextEditorEscapeKeyPressed (SingleLineTextEditor&)
{
    editor.setVisible (false);
    editing = false;
    repaint();
}

void Readout::sltextEditorFocusLost (SingleLineTextEditor&)
{
    float v = editor.getText().getFloatValue();
    parameter->setUserValueAsUserAction (v);

    editor.setVisible (false);
    editing = false;
    repaint();
}

//==============================================================================
ParamComponent::ParamComponent (Parameter* parameter_)
  : Component (""), parameter (parameter_)
{

}
//==============================================================================
Knob::Knob (Parameter* p, bool fromCentre)
  : ParamComponent (p),
    value (parameter),
    knob (parameter, Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (knob);
    addChildComponent (modButton);

    knob.setDoubleClickReturnValue (true, parameter->getUserDefaultValue());
    knob.setSkewFactor (parameter->getSkew(), parameter->isSkewSymmetric());
    if (fromCentre)
        knob.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), dontSendNotification);
    value.setJustificationType (Justification::centredTop);
    name.setJustificationType (Justification::centredBottom);

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

            Array<var> vals;
            for (auto v : modValues)
                vals.add (v);

            knob.getProperties().set ("modValues", vals);

            repaint();
        }
    };
    shiftTimer.onTimer = [this] ()
    {
        bool shift = ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
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
    PopupMenu m;

    auto& mm = *parameter->getModMatrix();
    for (auto src : mm.getModSources (parameter))
    {
        m.addItem ("Remove " + mm.getModSrcName (src), [this, src]
        {
            parameter->getModMatrix()->clearModDepth (src, parameter->getModIndex());
        });
    }

    m.showMenuAsync ({});
}

void Knob::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    value.setBounds (rc);
    knob.setBounds (r.reduced (2));

    modButton.setBounds (knob.getBounds().removeFromTop (7).removeFromRight (7));
}

void Knob::mouseEnter (const MouseEvent&)
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
        ! ModifierKeys::getCurrentModifiers().isAnyMouseButtonDown() &&
        ! value.isEditing())
    {
        name.setVisible (true);
        value.setVisible (false);

        stopTimer();
    }
}

void Knob::learnSourceChanged (int src)
{
    learning = src != -1;

    bool shift = ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    knob.setInterceptsMouseClicks (! learning || shift, ! learning || shift );

    auto& mm = *parameter->getModMatrix();
    modDepth = mm.getModDepth (mm.getLearn(), parameter->getModIndex());

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
        if (mm->isModulated (parameter->getModIndex()) || liveValuesCallback)
        {
            modTimer.startTimerHz (30);
            modButton.setVisible (mm->isModulated (parameter->getModIndex()));
        }
        else
        {
            modTimer.stopTimer();
            knob.getProperties().remove ("modValues");
            modButton.setVisible (false);
        }
    }
}

void Knob::mouseDown (const MouseEvent& e) 
{
    bool shift = ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    if (shift || ! learning || ! knob.getBounds().contains (e.getMouseDownPosition()))
        return;

    auto& mm = *parameter->getModMatrix();
    modDepth = mm.getModDepth (mm.getLearn(), parameter->getModIndex());

    knob.getProperties().set ("modDepth", modDepth);

    repaint();
}

void Knob::mouseDrag (const MouseEvent& e)
{
    bool shift = ModifierKeys::getCurrentModifiersRealtime().isShiftDown();
    if (shift || ! learning || ! knob.getBounds().contains (e.getMouseDownPosition()))
         return;

    if (e.getDistanceFromDragStart() >= 3)
    {
        auto pt = e.getMouseDownPosition();
        auto delta = (e.position.x - pt.getX()) + (pt.getY() - e.position.y);

        float newModDepth = jlimit (-1.0f, 1.0f, delta / 200.0f + modDepth);

        knob.getProperties().set ("modDepth", newModDepth);

        auto& mm = *parameter->getModMatrix();
        mm.setModDepth (mm.getLearn(), parameter->getModIndex(), newModDepth);

        repaint();
    }
}

//==============================================================================
HorizontalFader::HorizontalFader (Parameter* p, bool fromCentre)
  : ParamComponent (p),
    value (parameter),
    fader (parameter, Slider::LinearHorizontal, Slider::NoTextBox)
{
    addAndMakeVisible (name);
    addAndMakeVisible (value);
    addAndMakeVisible (fader);

    fader.setSkewFactor (parameter->getSkew());
    if (fromCentre)
        fader.getProperties().set ("fromCentre", true);

    name.setText (parameter->getShortName(), dontSendNotification);
    value.setJustificationType (Justification::centred);
    name.setJustificationType (Justification::centredRight);
}

void HorizontalFader::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (4);

    name.setBounds (r.removeFromLeft (90));
    value.setBounds (r.removeFromRight (30));
    fader.setBounds (r.reduced (2));
}
//==============================================================================
Switch::Switch (Parameter* p)
  : ParamComponent (p),
    button (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&button);

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Switch::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    button.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
}

//==============================================================================
Select::Select (Parameter* p)
  : ParamComponent (p),
    comboBox (parameter)
{
    addAndMakeVisible (&name);
    addAndMakeVisible (&comboBox);

    name.setText (parameter->getShortName(), dontSendNotification);
    name.setJustificationType (Justification::centred);
}

void Select::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (2);
    auto rc = r.removeFromBottom (15);

    name.setBounds (rc);
    comboBox.setBounds (r.withSizeKeepingCentre (getWidth() - 4, 15));
}

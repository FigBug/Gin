
Parameter::Parameter (Processor& p, juce::String uid_, juce::String name_, juce::String shortName_,
                      juce::String label_, float minValue, float maxValue,
                      float intervalValue, float defaultValue_, float skewFactor,
                      std::function<juce::String (const Parameter&, float)> textFunction_)
  : juce::AudioPluginInstance::HostedParameter (p.versionHint),
    processor (p),
    value (defaultValue_),
    defaultValue (defaultValue_),
    uid (uid_),
    name (name_),
    shortName (shortName_),
    label (label_),
    textFunction (textFunction_)
{
    if (shortName.isEmpty())
        shortName = name;

    range = juce::NormalisableRange<float> (minValue, maxValue, intervalValue, skewFactor);
}

Parameter::Parameter (Processor& p, juce::String uid_, juce::String name_, juce::String shortName_,
                      juce::String label_, juce::NormalisableRange<float> range_, float defaultValue_,
                      std::function<juce::String (const Parameter&, float)> textFunction_)
  : juce::AudioPluginInstance::HostedParameter (p.versionHint),
    processor (p),
    range (range_),
    value (defaultValue_),
    defaultValue (defaultValue_),
    uid (uid_),
    name (name_),
    shortName (shortName_),
    label (label_),
    textFunction (textFunction_)
{
    if (shortName.isEmpty())
        shortName = name;
}

void Parameter::setUserValue (float v)
{
    v = juce::jlimit(range.start, range.end, range.snapToLegalValue (v));
    if (! juce::approximatelyEqual (value, v))
    {
        value = v;
        triggerAsyncUpdate();
        changed();
    }
}

void Parameter::setUserValueNotifingHost (float v)
{
    v = juce::jlimit (range.start, range.end, range.snapToLegalValue (v));
    if (! juce::approximatelyEqual (value, v))
    {
        value = v;
        if (! internal)
            setValueNotifyingHost (getValue());

        triggerAsyncUpdate();
        changed();
    }
}

void Parameter::setUserValueAsUserAction (float f)
{
    beginUserAction();

    if (internal)
        setValue (f);
    else
        setUserValueNotifingHost (f);

    endUserAction();
}

void Parameter::beginUserAction()
{
    if (! internal)
    {
        userActionCount++;
        if (userActionCount == 1)
            beginChangeGesture();
    }
}

void Parameter::endUserAction()
{
    if (! internal)
    {
        userActionCount--;
        if (userActionCount == 0)
            endChangeGesture();
    }
}

void Parameter::beginUserTimedAction()
{
    if (! internal)
    {
        if (! isTimerRunning())
            beginUserAction();

        startTimer (2000);
    }
}

void Parameter::timerCallback()
{
    if (! internal)
        endUserAction();
    stopTimer();
}

void Parameter::addListener (ParameterListener* listener)
{
    listeners.add (listener);
}

void Parameter::removeListener (ParameterListener* listener)
{
    listeners.remove (listener);
}

void Parameter::handleAsyncUpdate()
{
    listeners.call (&ParameterListener::valueUpdated, this);
}

Parameter::ParamState Parameter::getState()
{
    ParamState state;
    state.uid = uid;
    state.value = getUserValue();
    return state;
}

void Parameter::setState (const ParamState& state)
{
    jassert (state.uid == uid);
    setUserValue (state.value);
}

void Parameter::setValue (float valueIn)
{
    valueIn = juce::jlimit (0.0f, 1.0f, valueIn);
    float newValue = range.snapToLegalValue (range.convertFrom0to1 (valueIn));

    if (! juce::approximatelyEqual (value, newValue))
    {
        value = newValue;

        triggerAsyncUpdate();
        changed();
    }
}

juce::String Parameter::getName (int maximumStringLength) const
{
    return name.substring (0, maximumStringLength);
}

int Parameter::getNumSteps() const
{
    if (juce::exactlyEqual (range.interval, 0.0f))
        return 0;
    return juce::roundToInt ((range.end - range.start) / range.interval);
}

juce::String Parameter::getText (float val, int /*maximumStringLength*/) const
{
    if (textFunction)
        return textFunction (*this, range.convertFrom0to1 (val));
    
    auto uv = range.snapToLegalValue (range.convertFrom0to1 (val));
    
    if (juce::exactlyEqual (range.interval, 1.0f))
        return juce::String (int (uv));
    
    return formatNumber (uv);
}

float Parameter::getValueForText (const juce::String& text) const
{
    return range.convertTo0to1 (text.getFloatValue());
}

bool Parameter::isOrientationInverted() const
{
    return false;
}

bool Parameter::isAutomatable() const
{
    return true;
}

bool Parameter::isMetaParameter() const
{
    return false;
}

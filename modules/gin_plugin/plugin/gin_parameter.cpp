
Parameter::Parameter (Processor& p, juce::String uid_, juce::String name_, juce::String shortName_,
                      juce::String label_, float minValue, float maxValue,
                      float intervalValue, float defaultValue_, float skewFactor,
                      std::function<juce::String (const Parameter&, float)> textFunction_)
  : processor (p),
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
  : processor (p),
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

bool Parameter::isOn()
{
    return range.start != getUserValue();
}

bool Parameter::isOnOff()
{
    return range.start == 0 && range.end == range.interval;
}

float Parameter::getProcValue()
{
    if (conversionFunction != nullptr)
        return conversionFunction (getUserValue());

    return getUserValue();
}

float Parameter::getProcValue (int)
{
    if (conversionFunction != nullptr)
        return conversionFunction (getUserValue());

    return getUserValue();
}

float Parameter::getUserValue() const
{
    return juce::jlimit (range.start, range.end, value);
}

int Parameter::getUserValueInt() const
{
    return int (juce::jlimit (range.start, range.end, value));
}

float Parameter::getUserDefaultValue() const
{
    return defaultValue;
}

void Parameter::setUserValue (float v)
{
    v = juce::jlimit(range.start, range.end, range.snapToLegalValue (v));
    if (! almostEqual (value, v))
    {
        value = v;
        triggerAsyncUpdate();
        changed();
    }
}

void Parameter::setUserValueNotifingHost (float v)
{
    v = juce::jlimit (range.start, range.end, range.snapToLegalValue (v));
    if (! almostEqual (value, v))
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

juce::String Parameter::getUserValueText() const
{
    return getText (getValue(), 1000) + label;
}

juce::String Parameter::userValueToText (float val)
{
    return getText (range.convertTo0to1 (val), 1000) + label;
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

void Parameter::addListener (Listener* listener)
{
    listeners.add (listener);
}

void Parameter::removeListener (Listener* listener)
{
    listeners.remove (listener);
}

void Parameter::handleAsyncUpdate()
{
    listeners.call (&Listener::parameterChanged, this);
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

float Parameter::getValue() const
{
    return juce::jlimit (0.0f, 1.0f, range.convertTo0to1 (value));
}

void Parameter::setValue (float valueIn)
{
    valueIn = juce::jlimit (0.0f, 1.0f, valueIn);
    float newValue = range.snapToLegalValue (range.convertFrom0to1 (valueIn));

    if (! almostEqual (value, newValue))
    {
        value = newValue;

        triggerAsyncUpdate();
        changed();
    }
}

float Parameter::getDefaultValue() const
{
    return range.convertTo0to1 (defaultValue);
}

juce::String Parameter::getName (int maximumStringLength) const
{
    return name.substring (0, maximumStringLength);
}

juce::String Parameter::getShortName() const
{
    return shortName;
}

juce::String Parameter::getLabel() const
{
    return label;
}

int Parameter::getNumSteps() const
{
    if (range.interval == 0)
        return 0;
    return juce::roundToInt ((range.end - range.start) / range.interval);
}

juce::String Parameter::getText (float val, int /*maximumStringLength*/) const
{
    if (textFunction)
        return textFunction (*this, range.convertFrom0to1 (val));
    
    auto uv = range.snapToLegalValue (range.convertFrom0to1 (val));
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

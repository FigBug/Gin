
Parameter::Parameter (Processor& p, String uid_, String name_, String shortName_,
                      String label_, float minValue, float maxValue,
                      float intervalValue, float defaultValue_, float skewFactor,
                      std::function<String (const Parameter&, float)> textFunction_)
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

    range = NormalisableRange<float> (minValue, maxValue, intervalValue, skewFactor);
}

Parameter::Parameter (Processor& p, String uid_, String name_, String shortName_,
                      String label_, NormalisableRange<float> range_, float defaultValue_,
                      std::function<String (const Parameter&, float)> textFunction_)
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
    return jlimit (range.start, range.end, value);
}

int Parameter::getUserValueInt() const
{
    return int (jlimit (range.start, range.end, value));
}

float Parameter::getUserDefaultValue() const
{
    return defaultValue;
}

void Parameter::setUserValue (float v)
{
    v = jlimit(range.start, range.end, range.snapToLegalValue (v));
    if (! almostEqual (value, v))
    {
        value = v;
        triggerAsyncUpdate();
        changed();
    }
}

void Parameter::setUserValueNotifingHost (float v)
{
    v = jlimit (range.start, range.end, range.snapToLegalValue (v));
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

String Parameter::getUserValueText() const
{
    if (textFunction)
        return textFunction (*this, getUserValue());
    return getText (getValue(), 1000);
}

String Parameter::userValueToText (float val)
{
    if (textFunction)
        return textFunction (*this, val);
    return getText (range.convertTo0to1 (val), 1000);
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
    return jlimit (0.0f, 1.0f, range.convertTo0to1 (value));
}

void Parameter::setValue (float valueIn)
{
    valueIn = jlimit (0.0f, 1.0f, valueIn);
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

String Parameter::getName (int maximumStringLength) const
{
    return name.substring (0, maximumStringLength);
}

String Parameter::getShortName() const
{
    return shortName;
}

String Parameter::getLabel() const
{
    return label;
}

int Parameter::getNumSteps() const
{
    if (range.interval == 0)
        return 0;
    return roundToInt ((range.end - range.start) / range.interval);
}

String Parameter::getText (float val, int /*maximumStringLength*/) const
{
    auto uv = range.snapToLegalValue (range.convertFrom0to1 (val));
    int dec = 1;
    if (uv < 10) dec = 2;
    if (uv < 1)  dec = 3;

    return String (uv, dec) + " " + label;
}

float Parameter::getValueForText (const String& text) const
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

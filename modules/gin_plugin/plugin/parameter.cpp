
Parameter::Parameter (GinProcessor& p, String uid_, String name_, String shortName_,
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

Parameter::Parameter (GinProcessor& p, String uid_, String name_, String shortName_,
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

void Parameter::prepareToPlay (double sampleRate, int /*samplesPerBlock*/)
{
    smoother.setSampleRate (sampleRate);
}

void Parameter::reset()
{
    smoother.snapToValue();
}

void Parameter::setSmoothed (bool s, float t)
{
    smoothed = s;
    if (smoothed)
        smoother.setTime (t);
}

bool Parameter::isOn()
{
    return range.start != getUserValue();
}

bool Parameter::isOnOff()
{
    return range.start == 0 && range.end == range.interval;
}

float Parameter::getProcValue() const
{
    if (conversionFunction != nullptr)
        return conversionFunction (getUserValue());
        
    return getUserValue();
}

float Parameter::getProcValueSmoothed (int stepSize)
{
    if (smoothed)
    {
        auto v = range.convertFrom0to1 (smoother.getCurrentValue());
        smoother.process (stepSize);
        
        if (conversionFunction != nullptr)
            return conversionFunction (v);
        
        return v;
    }
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
        smoother.setValue (range.convertTo0to1 (value));

        triggerAsyncUpdate();
    }
}

void Parameter::setUserValueNotifingHost (float v)
{
    v = jlimit (range.start, range.end, range.snapToLegalValue (v));
    if (! almostEqual (value, v))
    {
        value = v;
        smoother.setValue (range.convertTo0to1 (value));
        
        setValueNotifyingHost (getValue());

        triggerAsyncUpdate();
    }
}

void Parameter::setUserValueAsUserAction (float f)
{
    beginUserAction();
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
    userActionCount++;
    if (userActionCount == 1)
        beginChangeGesture();
}

void Parameter::endUserAction()
{
    userActionCount--;
    if (userActionCount == 0)
        endChangeGesture();
}

void Parameter::beginUserTimedAction()
{
    if (! isTimerRunning())
        beginUserAction();

    startTimer (2000);
}

void Parameter::timerCallback()
{
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
        smoother.setValue (range.convertTo0to1 (value));
        
        triggerAsyncUpdate();
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
    return String (range.snapToLegalValue (range.convertFrom0to1 (val)), 1) + " " + label;
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

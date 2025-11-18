/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

//==============================================================================
int versionStringToInt (const juce::String& versionString)
{
    juce::StringArray parts;
    parts.addTokens (versionString, ".", "");
    parts.trim();
    parts.removeEmptyStrings();

    int res = 0;

    for (auto part : parts)
        res = (res << 8) + part.getIntValue();

    return res;
}

//==============================================================================
class DelayedLambdaHelper : public juce::Timer
{
public:
    DelayedLambdaHelper (std::function<void ()>& cb, int ms)
        : callback (cb)
    {
        startTimer (ms);
    }

    void timerCallback() override
    {
        callback();
        delete this;
    }

    std::function<void ()> callback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayedLambdaHelper)
};

void delayedLambda (std::function<void ()> callback, int delayMS)
{
    new DelayedLambdaHelper (callback, delayMS);
}

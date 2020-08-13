/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

juce::Colour goldenRatioColor (int idx)
{
    double h = std::fmod (idx * 0.618033988749895, 1);
    return juce::Colour (float (h), 0.8f, 0.95f, 1.0f);
}

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
};

void delayedLambda (std::function<void ()> callback, int delayMS)
{
    new DelayedLambdaHelper (callback, delayMS);
}

/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/**
 Like juce::Timer, but it calls a std::function<>
 */
class LambdaTimer : public juce::Timer
{
public:
    LambdaTimer (std::function<void()> func = nullptr) : onTimer (func) {}

    std::function<void()> onTimer;

private:
    void timerCallback() override
    {
        if (onTimer)
            onTimer();
    }
};

/**
 Like juce::AsyncUpdater, but it calls a std::function<>
*/
class LambdaAsyncUpdater : public juce::AsyncUpdater
{
public:
    LambdaAsyncUpdater (std::function<void()> func) : onAsyncUpdate (func) {}

    std::function<void()> onAsyncUpdate;

private:
    void handleAsyncUpdate () override
    {
        if (onAsyncUpdate)
            onAsyncUpdate();
    }
};

void callOnMainThreadBlocking ( std::function<void ()> func );

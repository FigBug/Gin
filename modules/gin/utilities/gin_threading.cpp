/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
class BackgroundCaller : private juce::Thread,
                         private juce::AsyncUpdater
{
public:
    BackgroundCaller (std::function<void (void)> func)
        : Thread ("BackgroundCaller"), function (func)
    {
        startThread();
    }

    ~BackgroundCaller() override
    {
        stopThread (1000);
    }

    void run() override
    {
        function();
        triggerAsyncUpdate();
    }

    void handleAsyncUpdate() override
    {
        delete this;
    }

    std::function<void (void)> function;
};

void callInBackground (std::function<void (void)> function)
{
    new BackgroundCaller (function);
}

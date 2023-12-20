/*==============================================================================

 Copyright 2020 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** Run multiple timers from one juce::Timer per rate */
class CoalescedTimer
{
public:
    CoalescedTimer () = default;

    ~CoalescedTimer ()
    {
        stopTimer();
    }

    void startTimer (int ms)
    {
        stopTimer();

        delay = ms;
        sharedTimers->add (this);
    }

    void startTimerHz (int hz)
    {
        if (hz > 0)
            startTimer (1000 / hz);
        else
            stopTimer();
    }

    void stopTimer()
    {
        sharedTimers->remove (this);
        delay = 0;
    }

    std::function<void ()> onTimer;

private:
    class SharedTimer : public juce::Timer
    {
    public:
        void timerCallback() override
        {
            for (auto t : timers)
                if (t->onTimer)
                    t->onTimer ();
        }

        juce::Array<CoalescedTimer*> timers;
    };

    class SharedTimers
    {
    public:
        void add (CoalescedTimer* t)
        {
            auto itr = timers.find (t->delay);
            if (itr == timers.end())
            {
                auto st = std::make_unique<SharedTimer>();
                st->timers.add (t);
                st->startTimer (t->delay);

                timers[t->delay] = std::move (st);
            }
            else
            {
                itr->second->timers.add (t);
            }
        }

        void remove (CoalescedTimer* t)
        {
            auto itr = timers.find (t->delay);
            if (itr != timers.end())
            {
                itr->second->timers.removeFirstMatchingValue (t);
                if (itr->second->timers.size() == 0)
                    timers.erase (t->delay);
            }
        }

        std::map<int, std::unique_ptr<SharedTimer>> timers;
    };

    int delay = 0;
    juce::SharedResourcePointer<SharedTimers> sharedTimers;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CoalescedTimer)
};

/*==============================================================================

 Copyright 2018 - 2025 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

/** An efficient timer that shares a single juce::Timer between multiple instances.

    Instead of each timer instance creating its own juce::Timer thread, this class
    coalesces all timers with the same interval into a single shared juce::Timer.
    This reduces overhead when you have many timers running at the same rate.

    Benefits:
    - Lower CPU usage when running many timers
    - Reduced thread creation overhead
    - Automatic cleanup when not in use

    Example usage:
    @code
    CoalescedTimer timer;
    timer.onTimer = [this] { updateUI(); };
    timer.startTimerHz (60); // 60 Hz refresh rate
    @endcode

    @see startTimer, startTimerHz, stopTimer
*/
class CoalescedTimer
{
public:
    /** Creates a stopped CoalescedTimer. */
    CoalescedTimer () = default;

    /** Destructor. Stops the timer if running. */
    ~CoalescedTimer ()
    {
        stopTimer();
    }

    /** Starts the timer with the specified interval in milliseconds.

        If the timer is already running, it will be restarted with the new interval.
        Multiple CoalescedTimer instances with the same interval will share a
        single underlying juce::Timer.

        @param ms  The timer interval in milliseconds
    */
    void startTimer (int ms)
    {
        stopTimer();

        delay = ms;
        sharedTimers->add (this);
    }

    /** Starts the timer with the specified frequency in Hz.

        This is a convenience method that converts frequency to interval.
        For example, 60 Hz becomes approximately 16.67 milliseconds.

        @param hz  The desired frequency in Hz (must be > 0, or timer will stop)
    */
    void startTimerHz (int hz)
    {
        if (hz > 0)
            startTimer (1000 / hz);
        else
            stopTimer();
    }

    /** Stops the timer.

        The onTimer callback will no longer be called. If this was the last
        CoalescedTimer using this interval, the shared juce::Timer is deleted.
    */
    void stopTimer()
    {
        sharedTimers->remove (this);
        delay = 0;
    }

    /** Callback function called at the specified interval.

        Set this to a lambda or function to be executed when the timer fires.
    */
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

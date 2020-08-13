/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
// Calls a function in background
void callInBackground (std::function<void (void)> function);

//==============================================================================
// Run a for loop split between each core.
// for (int i = 0; i < 10; i++) becomes multiThreadedFor<int> (0, 10, 1, threadPool, [&] (int i) {});
// Make sure each iteration of the loop is independant
template <typename T>
void multiThreadedFor (T start, T end, T interval, juce::ThreadPool* threadPool, std::function<void (T idx)> callback)
{
    if (threadPool == nullptr)
    {
        for (int i = start; i < end; i += interval)
            callback (i);
    }
    else
    {
        int num = threadPool->getNumThreads();

        juce::WaitableEvent wait;
        juce::Atomic<int> threadsRunning (num);

        for (int i = 0; i < num; i++)
        {
            threadPool->addJob ([i, &callback, &wait, &threadsRunning, start, end, interval, num]
                                {
                                    for (int j = start + interval * i; j < end; j += interval * num)
                                        callback (j);

                                    int stillRunning = --threadsRunning;
                                    if (stillRunning == 0)
                                        wait.signal();
                                });
        }

        wait.wait();
    }
}

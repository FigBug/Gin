/*==============================================================================

 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com

 ==============================================================================*/

#pragma once

//==============================================================================
/** Executes a function on a background thread.

    This is a convenience function for running code asynchronously without blocking
    the current thread. The function will be executed on a separate thread and
    return immediately.

    @param function  The function to execute on the background thread
*/
void callInBackground (std::function<void (void)> function);

//==============================================================================
/** Executes a for loop in parallel across multiple threads.

    This function distributes loop iterations across available CPU cores using a thread pool.
    Each thread processes a subset of iterations, with work distributed evenly. This is useful
    for parallelizing computationally intensive loops.

    Example usage:
    @code
    // Serial loop:
    for (int i = 0; i < 10; i++)
        processItem (i);

    // Parallel version:
    multiThreadedFor<int> (0, 10, 1, threadPool, [&] (int i) {
        processItem (i);
    });
    @endcode

    @warning Each iteration must be independent - there should be no dependencies between
             iterations or race conditions when accessing shared data.

    @param start       The starting index (inclusive)
    @param end         The ending index (exclusive)
    @param interval    The step size between iterations
    @param threadPool  The thread pool to use (if nullptr, runs serially on current thread)
    @param callback    The function to call for each iteration, receiving the index as parameter
*/
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

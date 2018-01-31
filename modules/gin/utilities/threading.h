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
// for (int i = 0; i < 10; i++) becomes multiThreadedFor (0, 10, 1, [&] (int i) {});
// Make sure each iteration of the loop is independant
template <typename T>
void multiThreadedFor (T start, T end, T interval, std::function<void (T idx)> callback)
{
    static ThreadPool pool (SystemStats::getNumCpus());
    
    int num = SystemStats::getNumCpus();
    
    Array<T> todo;
    todo.ensureStorageAllocated ((end - start) / interval);
    for (T i = start; i < end; i += interval)
        todo.add (i);
    
    int each = int (std::ceil (todo.size() / float (num)));
    
    WaitableEvent wait;
    
    Atomic<int> threadsRunning (num);
    
    for (int i = 0; i < num; i++)
    {
        pool.addJob ([i, &callback, &wait, &todo, &threadsRunning, &each]
                     {
                         for (int j = i * each; j < jmin (todo.size(), (i + 1) * each); j++)
                             callback (todo[j]);
                         threadsRunning -= 1;
                         wait.signal();
                     });
    }
    
    while (threadsRunning.get() > 0)
        wait.wait();
}

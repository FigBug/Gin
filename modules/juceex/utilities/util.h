/*==============================================================================
 
 Copyright 2018 by Roland Rabien
 For more information visit www.rabiensoftware.com
 
 ==============================================================================*/

#pragma once

//==============================================================================
// Solves for the slope and intercept of a line.
template <typename T>
bool solveLine (T x1, T y1, T x2, T y2, T& m, T& b)
{
    if (x2 != x1)
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - *m * x2;
        
        return true;
    }
    else
    {
        m = 0;
        b = 0;
        
        return false;
    }
}

template <typename T>
bool solveLine (Line<T> l, T& m, T& b)
{
    T x1 = l.getStartX();
    T x2 = l.getEndX();
    T y1 = l.getStartY();
    T y2 = l.getEndY();

    if (x2 != x1)
    {
        m = (y2 - y1) / (x2 - x1);
        b = y2 - *m * x2;
        
        return true;
    }
    else
    {
        m = 0;
        b = 0;
        
        return false;
    }
}

//==============================================================================
// Keeps a rolling average of a series of numbers
class RollingAverage
{
public:
    RollingAverage (int numVals_)
      : numVals (numVals_),
        currAvg (0)
    {
    }

    double average (double nextValue)
    {
        return (nextValue + numVals * currAvg) / (double)(numVals + 1); 
    }

    double getAverage()
    {
        return currAvg;
    }

private:
    int numVals;
    double currAvg;
};

//==============================================================================
// Returns the next colour in a set where the hues differ by the golden ratio.
// Good for coming up with a random set of colours
Colour goldenRatioColor (int idx);

//==============================================================================
// Keeps a rolling average of a series of numbers
String getMidiMessageType (const MidiMessage& msg);

//==============================================================================
// Async Download. Doesn't have the main thread pause the URL::downloadToFile has
class AsyncDownload : private Thread,
                      private AsyncUpdater
{
public:
    AsyncDownload (String url_, std::function<void (AsyncDownload*,MemoryBlock, bool)> cb_)
      : Thread ("AsyncDownload"), url (url_), cb (cb_)
    {
        startThread();
    }
    
    ~AsyncDownload()
    {
        stopThread (100);
    }
    
    void run() override
    {
        ok = URL (url).readEntireBinaryStream (data);
        triggerAsyncUpdate();
    }
    
    void handleAsyncUpdate() override
    {
        if (cb)
            cb (this, data, ok);
    }
    
    String url;
    std::function<void (AsyncDownload*, MemoryBlock, bool)> cb;
    bool ok = false;
    MemoryBlock data;
};

//==============================================================================
// Calls a function in background
void callInBackground (std::function<void (void)> function);

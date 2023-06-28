/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

/**
 Linearly smooths a value in a 0 to 1 range at a constant rate. Time is for
 from duration from min to max.
*/
template <class T>
class ValueSmoother
{
public:
    void setSampleRate (double sr)  { sampleRate = sr; reset();     }
    void setTime (double t)         { time = t; reset();            }
            
    bool isSmoothing()              { return std::abs (currentValue - targetValue) > delta * 0.001; }
    void reset()                    { delta = 1.0 / (sampleRate * time);     }
            
    T getCurrentValue()             { return currentValue;          }
    T* getValuePtr()                { return &currentValue;         }
    void setValue (T v)             { targetValue = v;              }
    void snapToValue()              { currentValue = targetValue;   }

    void process (int n)
    {
        if (! juce::approximatelyEqual (targetValue, currentValue))
            for (int i = 0; i < n; i++)
                updateValue();
    }

    void updateValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, T (currentValue + delta));
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, T (currentValue - delta));
    }

    T getNextValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, T (currentValue + delta));
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, T (currentValue - delta));
        
        return currentValue;
    }

    void setValueUnsmoothed (T v)
    {
        targetValue = v;
        currentValue = v;
    }

private:
    double time = 0.1, sampleRate = 44100.0;
    double delta = 0;
    
    T targetValue = 0;
    T currentValue = 0;
};

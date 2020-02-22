/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
template <class T>
class ValueSmoother
{
public:
    void setSampleRate (double sr)
    {
        sampleRate = sr;
        reset();
    }
    
    void setTime (double t)
    {
        time = t;
        reset();
    }
    
    bool isSmoothing()
    {
        return std::abs (currentValue - targetValue) > delta * 0.001;
    }
    
    void reset()
    {
        delta = 1.0f / float (sampleRate * time);
    }

    T getCurrentValue()
    {
        return currentValue;
    }

    void process (int n)
    {
        if (targetValue != currentValue)
            for (int i = 0; i < n; i++)
                updateValue();
    }

    void updateValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, currentValue + delta);
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, currentValue - delta);
    }

    T getNextValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, currentValue + delta);
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, currentValue - delta);
        
        return targetValue;
    }

    T* getValuePtr()
    {
        return &currentValue;
    }

    void setValue (T v)
    {
        targetValue = v;
    }

    void snapToValue()
    {
        currentValue = targetValue;
    }

    void setValueUnsmoothed (T v)
    {
        targetValue = v;
        currentValue = v;
    }

private:
    double time = 0.1, sampleRate = 44100.0;
    
    T delta = 0;
    T targetValue = 0;
    T currentValue = 0;
};

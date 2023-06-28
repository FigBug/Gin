/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/**
 Linearly smooths a value in a 0 to 1 range at a constant rate. Time is for
 from duration from min to max.
*/
template <class T>
class EasedValueSmoother
{
public:
    void setSampleRate (double sr)  { sampleRate = sr; reset();     }
    void setTime (double t)         { time = t; reset();            }
            
    bool isSmoothing()              { return position < 1.0;        }
    void reset()                    {                               }
            
    T getCurrentValue()             { return currentValue;          }
    T* getValuePtr()                { return &currentValue;         }
    
    void snapToValue()
    {
        currentValue = targetValue;
        position = 1.0;
    }

    void process (int n)
    {
        if (! juce::approximatelyEqual (targetValue, currentValue))
            for (int i = 0; i < n; i++)
                updateValue();
    }
    
    void setValue (T v)
    {
        origValue = currentValue;
        targetValue = v;
        position = 0;
    }

    void updateValue()
    {
        if (position < 1.0)
        {
            double delta = 1.0 / double (sampleRate * time);
            position = std::min (position + delta, 1.0);
            
            auto f = easeQuadraticInOut (position);
            currentValue = T ((targetValue * f) + (origValue * (1.0 - f)));
        }
    }

    T getNextValue()
    {
        updateValue();
        return (position < 1.0) ? currentValue : targetValue;
    }

    void setValueUnsmoothed (T v)
    {
        targetValue  = v;
        currentValue = v;
        origValue    = v;
        position     = 1.0;
    }

private:
    double time = 0.1, sampleRate = 44100.0;
    double position = 1.0;
    
    T targetValue = 0;
    T origValue = 0;
    T currentValue = 0;
};

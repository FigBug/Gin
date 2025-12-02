/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

//==============================================================================
/** Linearly smooths parameter changes over time at a constant rate.

    ValueSmoother provides linear interpolation between parameter values, smoothing
    transitions to prevent zipper noise and clicks in audio processing. It operates
    by moving the current value toward the target value at a constant rate per sample,
    defined by the smoothing time.

    The smoother is designed for normalized values (0 to 1 range) but can work with
    any numeric type. The time parameter specifies how long it takes to transition
    from 0 to 1 (or 1 to 0), regardless of the actual range used.

    Common Use Cases:
    - Smoothing parameter changes in audio effects
    - Preventing zipper noise in gain controls
    - Smoothing filter cutoff frequency changes
    - Envelope-like parameter transitions

    Thread Safety:
    - NOT thread-safe
    - Use within a single audio processing thread

    Example usage:
    @code
    ValueSmoother<float> gainSmoother;
    gainSmoother.setSampleRate (44100.0);
    gainSmoother.setTime (0.05); // 50ms smoothing time

    // In parameter change callback
    gainSmoother.setValue (newGainValue);

    // In audio processing loop
    for (int i = 0; i < numSamples; ++i)
    {
        float smoothedGain = gainSmoother.getNextValue();
        output[i] = input[i] * smoothedGain;
    }
    @endcode

    @tparam T The numeric type for values (typically float or double)
*/
template <class T>
class ValueSmoother
{
public:
    /** Sets the sample rate for smoothing calculations.
        This resets the smoother's delta value.
        @param sr The sample rate in Hz
    */
    void setSampleRate (double sr)  { sampleRate = sr; reset();     }

    /** Sets the smoothing time - duration for a full 0 to 1 transition.
        This resets the smoother's delta value.
        @param t Smoothing time in seconds
    */
    void setTime (double t)         { time = t; reset();            }

    /** Checks if the smoother is currently transitioning to the target value.
        @return true if the current value hasn't reached the target yet
    */
    bool isSmoothing()              { return std::abs (currentValue - targetValue) > delta * 0.001; }

    /** Recalculates the delta value based on current sample rate and time.
        Called automatically when sample rate or time changes.
    */
    void reset()                    { delta = 1.0 / (sampleRate * time);     }

    /** Returns the current smoothed value without advancing.
        @return The current value
    */
    T getCurrentValue()             { return currentValue;          }

    /** Returns a pointer to the current value for direct access.
        @return Pointer to the internal current value
    */
    T* getValuePtr()                { return &currentValue;         }

    /** Sets the target value to smooth towards.
        The current value will gradually move toward this target.
        @param v The new target value
    */
    void setValue (T v)             { targetValue = v;              }

    /** Immediately snaps the current value to the target value.
        Useful for initializing or when you want to skip smoothing.
    */
    void snapToValue()              { currentValue = targetValue;   }

    /** Processes multiple samples, advancing the smoother by n steps.
        Updates the current value n times, moving it toward the target.
        @param n Number of samples to process
    */
    void process (int n)
    {
        if (! juce::approximatelyEqual (targetValue, currentValue))
            for (int i = 0; i < n; i++)
                updateValue();
    }

    /** Advances the current value one step toward the target.
        This is the core smoothing function, called internally by process()
        and getNextValue().
    */
    void updateValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, T (currentValue + delta));
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, T (currentValue - delta));
    }

    /** Advances the smoother by one sample and returns the new value.
        This is the typical method to call in per-sample audio processing.
        @return The next smoothed value
    */
    T getNextValue()
    {
        if (currentValue < targetValue)
            currentValue = std::min (targetValue, T (currentValue + delta));
        else if (currentValue > targetValue)
            currentValue = std::max (targetValue, T (currentValue - delta));

        return currentValue;
    }

    /** Sets both target and current value immediately without smoothing.
        Useful for initialization or when you need to jump to a value instantly.
        @param v The value to set immediately
    */
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

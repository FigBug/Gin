/*
==============================================================================

This file is part of the GIN library.
Copyright (c) 2019 - Roland Rabien.

==============================================================================
*/

#pragma once

/** Draws a comp, gate, limiter, expander graph
    Draws input > output curve
    Draws input and output levels
*/
class DynamicsMeter : public Component,
                      public Timer
{
public:
    DynamicsMeter (Dynamics&);

    ~DynamicsMeter() override;
    
    enum ColourIds
    {
        lineColourId             = 0x1291e10,
        backgroundColourId       = 0x1291e11,
        meterColourId            = 0x1291e12
    };
    
    //==============================================================================
    void paint (Graphics& g) override;
    void timerCallback() override;

private:
    Dynamics& dynamics;
    
    NormalisableRange<float> range {-60, 0};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynamicsMeter)
};

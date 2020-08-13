/*
==============================================================================

This file is part of the GIN library.
Copyright (c) 2019 - Roland Rabien.

==============================================================================
*/

#pragma once

/** Draws a simple level meter
*/
class LevelMeter : public juce::Component,
                   public juce::Timer
{
public:
    LevelMeter (const LevelTracker&, juce::NormalisableRange<float> r = {-60, 0} );
    ~LevelMeter() override;
    
    void setTopDown (bool td)   { topDown = td; }
    
    enum ColourIds
    {
        lineColourId             = 0x1291e10,
        backgroundColourId       = 0x1291e11,
        meterColourId            = 0x1291e12
    };

private:
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void timerCallback() override;

    const LevelTracker& tracker;
    
    bool topDown = false;
    juce::NormalisableRange<float> range {-60, 0};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeter)
};

#pragma once

//==============================================================================
/** Draws an LFO curve and shows a little dot moving long
*/
class LFOComponent : public MultiParamComponent,
                     private juce::Timer
{
public:
    LFOComponent();
    ~LFOComponent() override = default;
    
    void setUnclamped (bool b) { unclamped = b; }

    void setParams (Parameter::Ptr wave, Parameter::Ptr sync, Parameter::Ptr rate,
                    Parameter::Ptr beat, Parameter::Ptr depth, Parameter::Ptr offset,
                    Parameter::Ptr phase, Parameter::Ptr enable);
    
    std::function<std::vector<float>()> phaseCallback;

private:
    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void paramChanged () override;

    void createPath (juce::Rectangle<int> area);
    float getSample (float phase);
    int getNumSteps();

    Parameter::Ptr wave, sync, rate, beat, depth, offset, phase, enable;

    LFO lfo;
    juce::Path path;
    bool dirty = true;
    std::vector<float> curPhases;
    std::map<int,float> curve;

private:
    bool unclamped = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOComponent)
};

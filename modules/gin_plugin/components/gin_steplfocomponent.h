#pragma once

//==============================================================================
/** Draws a step LFO curve and shows a little dot moving long
*/
class StepLFOComponent : public MultiParamComponent,
                         private juce::Timer
{
public:
    StepLFOComponent (int maxSteps_);
    ~StepLFOComponent() override = default;

    void setParams (Parameter::Ptr beat, Parameter::Ptr length, Parameter::Ptr* level, Parameter::Ptr enable);

    std::function<std::vector<float>()> phaseCallback;

private:
    void paint (juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void paramChanged () override;
    void mouseDrag (const juce::MouseEvent& e) override;

    void createPath (juce::Rectangle<int> area);
    float getSample (float phase);
    int getNumSteps();

    Parameter::Ptr beat, length, enable;
    std::vector<Parameter::Ptr> level;

    StepLFO lfo;
    juce::Path path;
    int maxSteps;
    bool dirty = true;
    std::vector<float> curPhases;
    std::map<int,float> curve;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepLFOComponent)
};

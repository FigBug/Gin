#pragma once

//==============================================================================
/** Draws a step LFO curve and shows a little dot moving long
*/
class StepLFOComponent : public MultiParamComponent,
                         private juce::Timer
{
public:
    StepLFOComponent() = default;
    ~StepLFOComponent() override = default;

    void setBPM (float bpm);
    void setParams (Parameter::Ptr beat, Parameter::Ptr length, Parameter::Ptr* level, Parameter::Ptr enable);

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
    Parameter::Ptr level[32];

    StepLFO lfo;
    juce::Path path;
    bool dirty = true;
    float bpm = 120.0f, curPhase = 0;
    double lastUpdate = -1;
    std::map<int,float> curve;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepLFOComponent)
};

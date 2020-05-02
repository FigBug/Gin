#pragma once

//==============================================================================
class LFOComponent : public MultiParamComponent,
                     private Timer
{
public:
    LFOComponent() = default;
    ~LFOComponent() override = default;

    void setBPM (float bpm);
    void setParams (Parameter::Ptr wave, Parameter::Ptr sync, Parameter::Ptr rate,
                    Parameter::Ptr beat, Parameter::Ptr depth, Parameter::Ptr offset,
                    Parameter::Ptr phase);

private:
    void paint (Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void paramChanged () override;

    void createPath (juce::Rectangle<int> area);
    float getSample (float phase);
    int getNumSteps();

    Parameter::Ptr wave, sync, rate, beat, depth, offset, phase;

    LFO lfo;
    Path path;
    bool dirty = true;
    float bpm = 120.0f, curPhase = 0;
    double lastUpdate = -1;
    std::map<int,float> curve;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LFOComponent)
};

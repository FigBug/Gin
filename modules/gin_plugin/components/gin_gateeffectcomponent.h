#pragma once

//==============================================================================
/** An editor for the gate effect pattern
*/
class GateEffectComponent : public MultiParamComponent
{
public:
    GateEffectComponent (int maxSteps_);
    ~GateEffectComponent() override = default;

    void setParams (Parameter::Ptr length, Parameter::Ptr* l, Parameter::Ptr* r, Parameter::Ptr enable);

private:
    void paint (juce::Graphics& g) override;
    void paramChanged () override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    int getNumSteps();

    Parameter::Ptr length, enable;
    std::vector<Parameter::Ptr> l, r;

private:
    int maxSteps;
    bool dragging = false;
    bool setOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateEffectComponent)
};

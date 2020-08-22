#pragma once

//==============================================================================
/** An editor for the gate effect pattern
*/
class GateEffectComponent : public MultiParamComponent
{
public:
    GateEffectComponent() = default;
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
    Parameter::Ptr l[32], r[32];

private:
    bool dragging = false;
    bool setOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GateEffectComponent)
};

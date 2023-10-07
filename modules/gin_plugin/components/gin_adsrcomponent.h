#pragma once

//==============================================================================
/** Component for adjuting ADSR curves
*/
class ADSRComponent : public MultiParamComponent
                    , private juce::Timer
{
public:
    ADSRComponent();
    ~ADSRComponent() override = default;
    
    void setParams (Parameter::Ptr attack, Parameter::Ptr decay,
                    Parameter::Ptr sustain, Parameter::Ptr release);

    std::function<std::vector<std::pair<int,float>>()> phaseCallback;

private:
    void timerCallback() override;
    void paint (juce::Graphics& g) override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;

    juce::MouseCursor getMouseCursor() override;

    enum class Handle
    {
        none,
        attack,
        decaySustain,
        release,
    };

    juce::String getBubbleText();

    Handle getHandleAt (juce::Point<int> pt);
    juce::Point<int> getHandlePos (Handle);
    juce::Rectangle<int> getHandleRect (Handle);
    juce::Rectangle<int> getArea();

    int paramToX (float t);
    float xToParam (float x);

    Handle handle = Handle::none;
    int dx = 0, dy = 0;
    Parameter::Ptr attack, decay, sustain, release;
    int handleSz = 8;

    std::vector<std::pair<int,float>> curPhases;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRComponent)
};

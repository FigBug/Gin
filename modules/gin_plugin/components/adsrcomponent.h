#pragma once

//==============================================================================
class ADSRComponent : public MultiParamComponent
{
public:
    ADSRComponent () = default;
    ~ADSRComponent () override = default;
    
    void setParams (Parameter::Ptr attack, Parameter::Ptr decay,
                    Parameter::Ptr sustain, Parameter::Ptr release);

private:
    void paint (Graphics& g) override;

    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    MouseCursor getMouseCursor() override;

    enum class Handle
    {
        none,
        attack,
        decaySustain,
        release,
    };

    String getBubbleText();

    Handle getHandleAt (Point<int> pt);
    Point<int> getHandlePos (Handle);
    juce::Rectangle<int> getHandleRect (Handle);
    juce::Rectangle<int> getArea();

    int paramToX (float t);
    float xToParam (float x);

    Handle handle = Handle::none;
    int dx = 0, dy = 0;
    Parameter::Ptr attack, decay, sustain, release;
    int handleSz = 8;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ADSRComponent)
};

#pragma once

//==============================================================================
/** Draws an MSEG LFO curve and shows a little dot moving long
*/
class MSEGComponent : public MultiParamComponent,
                      private juce::Timer
{
public:
    MSEGComponent (MSEG::Data& d);
    ~MSEGComponent() override = default;

    void setParams (Parameter::Ptr wave, Parameter::Ptr sync, Parameter::Ptr rate,
                    Parameter::Ptr beat, Parameter::Ptr depth, Parameter::Ptr offset,
                    Parameter::Ptr phase, Parameter::Ptr enable);

    std::function<std::vector<float>()> phaseCallback;

private:
    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

    void timerCallback() override;
    void paramChanged () override;

    void createPath (juce::Rectangle<int> area);
    float getSample (float phase);
    int getNumSteps();

    Parameter::Ptr wave, sync, rate, beat, depth, offset, phase, enable;

    MSEG::Data& data;
    MSEG mseg {data};

    juce::Path path;
    bool dirty = true;
    std::vector<float> curPhases;
    std::map<int,float> curve;

    int draggingPoint = -1;
    int draggingCurve = -1;

    float lastY = 0.0f;

    juce::Rectangle<float> getArea() { return getLocalBounds().toFloat().reduced (2.0f); }

    int getPointAt (juce::Point<float> p);
    int getCurveAt (juce::Point<float> p);

    float valueToY (float v);
    float timeToX (float t);

    float xToTime (float x);
    float yToValue (float y);

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MSEGComponent)
};

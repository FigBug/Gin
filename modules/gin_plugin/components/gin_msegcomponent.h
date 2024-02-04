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
                    Parameter::Ptr phase, Parameter::Ptr enable, Parameter::Ptr xgrid,
                    Parameter::Ptr ygrid, Parameter::Ptr loop);

    void setEditable (bool e) { editable = e; }
    void markDirty() { dirty = true; repaint(); }

    std::function<std::vector<float>()> phaseCallback;
    std::function<void()> onClick;
    std::function<void()> onLoad;
    std::function<void()> onSave;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    float snapT (float t);
    float snapV (float v);
    
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseMove (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override;
    void mouseExit (const juce::MouseEvent&) override;

    void showBubbleMessage (const juce::Rectangle<int>& rc, const juce::String& message);
    void hideBubbleMessage();

    void timerCallback() override;
    void paramChanged () override;

    void createPath();
    float getSample (float phase);

    Parameter::Ptr wave, sync, rate, beat, depth, offset, phase, enable, xgrid, ygrid, loop;

    MSEG::Data& data;
    MSEG mseg {data};

    juce::Path path;
    bool dirty = true;
    std::vector<float> curPhases;

    int draggingPoint = -1;
    int draggingCurve = -1;

    float lastY = 0.0f;
    bool editable = false;

    juce::Rectangle<float> getArea() { return getLocalBounds().toFloat().reduced (editable ? 4.0f : 2.0f); }

    int getPointAt (juce::Point<float> p);
    int getCurveAt (juce::Point<float> p);

    float valueToY (float v);
    float timeToX (float t);

    float xToTime (float x);
    float yToValue (float y);

private:
    std::unique_ptr<juce::BubbleMessageComponent> bubbleMessage;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MSEGComponent)
};
